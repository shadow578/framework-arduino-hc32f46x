#include "WInterrupts.h"
#include "wiring_constants.h"
#include "core_debug.h"
#include "drivers/gpio/gpio.h"
#include "drivers/irqn/irqn.h"
#include <hc32_ddl.h>

// #region Utilities
inline en_exti_lvl_t mapToTriggerMode(uint32_t mode)
{
    switch (mode)
    {
    case RISING:
        return ExIntRisingEdge;
    case FALLING:
        return ExIntFallingEdge;
    case CHANGE:
        return ExIntBothEdge;
    case LOW:
        return ExIntLowLevel;
    }

    CORE_ASSERT_FAIL("Invalid external interrupt mode. only RISING, FALLING, CHANGE, LOW are valid");
    return ExIntFallingEdge;
}

inline en_exti_ch_t mapToExternalInterruptChannel(gpio_pin_t pin)
{
    // check range
    ASSERT_GPIO_PIN_VALID(pin, "mapToExternalInterruptChannel");
    if (pin > BOARD_NR_GPIO_PINS)
    {
        return ExtiCh00;
    }

    // map to exti channel
    uint8_t ch = PIN_MAP[pin].bit_pos;
    if (ch < 0 || ch > 15)
    {
        // invalid channel, only 00 - 15 are valid
        CORE_ASSERT_FAIL("Invalid external interrupt channel")
        return ExtiCh00;
    }

    // exti channel maps 1-to-1
    return (en_exti_ch_t)ch;
}

inline en_int_src_t mapToInterruptSource(gpio_pin_t pin)
{
    // check range
    ASSERT_GPIO_PIN_VALID(pin, "mapToInterruptSource");
    if (pin > BOARD_NR_GPIO_PINS)
    {
        return INT_PORT_EIRQ0;
    }

    // map to interrupt source
    uint8_t ch = PIN_MAP[pin].bit_pos;
    if (ch < 0 || ch > 15)
    {
        // invalid source, only 00 - 15 are valid
        CORE_ASSERT_FAIL("invalid external interrupt source")
        return INT_PORT_EIRQ0;
    }

    // interrupt source maps 1-to-1
    return (en_int_src_t)ch;
}

void _attachInterrupt(gpio_pin_t pin, voidFuncPtr handler, IRQn_Type irqn, uint32_t mode)
{
    // check inputs
    ASSERT_GPIO_PIN_VALID(pin, "_attachInterrupt");
    CORE_ASSERT(handler != NULL, "_attachInterrupt: handler cannot be NULL")
    if (pin >= BOARD_NR_GPIO_PINS || handler == NULL)
    {
        return;
    }

    // initialize external interrupt channel
    stc_exint_config_t extiConf = {
        .enExitCh = mapToExternalInterruptChannel(pin),
        .enFilterEn = Enable,
        .enFltClk = Pclk3Div8,
        .enExtiLvl = mapToTriggerMode(mode)};
    EXINT_Init(&extiConf);

    // configure port for external interrupt
    stc_port_init_t portConf;
    MEM_ZERO_STRUCT(portConf);
    portConf.enExInt = Enable;
    GPIO_Init(pin, &portConf);

    // register IRQ
    stc_irq_regi_conf_t irqReg = {
        .enIntSrc = mapToInterruptSource(pin),
        .enIRQn = irqn,
        .pfnCallback = handler};
    enIrqRegistration(&irqReg);

    // clear pending, set priority and enable
    NVIC_ClearPendingIRQ(irqReg.enIRQn);
    NVIC_SetPriority(irqReg.enIRQn, DDL_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(irqReg.enIRQn);
}

void _detachInterrupt(gpio_pin_t pin, IRQn_Type irqn)
{
    // check inputs
    ASSERT_GPIO_PIN_VALID(pin, "_detachInterrupt");
    if (pin >= BOARD_NR_GPIO_PINS)
    {
        return;
    }

    // configure port to disable external interrupt
    stc_port_init_t portConf;
    MEM_ZERO_STRUCT(portConf);
    portConf.enExInt = Disable;
    GPIO_Init(pin, &portConf);

    // clear pending and disable IRQ
    NVIC_ClearPendingIRQ(irqn);
    NVIC_DisableIRQ(irqn);
    enIrqResign(irqn);
}
// #endregion

// #region pin to IRQn mapping
typedef struct pin_to_irqn_mapping_t
{
    gpio_pin_t pin;
    IRQn_Type irqn;
    struct pin_to_irqn_mapping_t *next;
} pin_to_irqn_mapping_t;

/**
 * @brief Linked list of pin to IRQn mappings
 */
pin_to_irqn_mapping_t *pin_to_irqn_mapping = NULL;

/**
 * @brief insert a pin -> irqn mapping
 */
inline void insert_pin_to_irqn_mapping(gpio_pin_t pin, IRQn_Type irqn)
{
    // create new node
    pin_to_irqn_mapping_t *node = new pin_to_irqn_mapping_t;
    node->pin = pin;
    node->irqn = irqn;
    node->next = NULL;

    // insert node...
    if (pin_to_irqn_mapping == NULL)
    {
        // ... as head
        pin_to_irqn_mapping = node;
    }
    else
    {
        // ... as child of last node:
        // find last node
        pin_to_irqn_mapping_t *parent = pin_to_irqn_mapping;
        while (parent->next != NULL)
        {
            parent = parent->next;
        }

        // set as child
        parent->next = node;
    }
}

/**
 * @brief get a pin -> irqn mapping by pin number
 */
inline bool get_pin_to_irqn_mapping(gpio_pin_t pin, pin_to_irqn_mapping_t &mapping)
{
    // find node
    pin_to_irqn_mapping_t *node = pin_to_irqn_mapping;
    while (node != NULL && node->pin != pin)
    {
        node = node->next;
    }

    // return mapping
    if (node != NULL && node->pin == pin)
    {
        mapping = *node;
        return true;
    }

    // not found
    return false;
}

/**
 * @brief remove a pin -> irqn mapping by pin number
 */
inline void remove_pin_to_irqn_mapping(gpio_pin_t pin)
{
    // find node
    pin_to_irqn_mapping_t *node = pin_to_irqn_mapping;
    pin_to_irqn_mapping_t *parent = NULL;
    while (node != NULL && node->pin != pin)
    {
        parent = node;
        node = node->next;
    }

    // remove node
    if (node != NULL && node->pin == pin)
    {
        // remove node from linked list
        if (parent != NULL)
        {
            parent->next = node->next;
        }
        else
        {
            pin_to_irqn_mapping = node->next;
        }

        // free node
        delete[] node;
    }
}

/**
 * @brief check if a pin -> irqn mapping exists that uses the given EXTi channel
 */
inline bool is_exti_channel_in_use(en_int_src_t ch)
{
    // loop over all pin to irqn mappings, check if any of them uses the given channel
    pin_to_irqn_mapping_t *node = pin_to_irqn_mapping;
    while (node != NULL)
    {
        // check if channel is in use
        if (mapToInterruptSource(node->pin) == ch)
        {
            return true;
        }

        // next node
        node = node->next;
    }

    // channel is not in use
    return false;
}

// #endregion

int attachInterrupt(gpio_pin_t pin, voidFuncPtr callback, uint32_t mode)
{
    ASSERT_GPIO_PIN_VALID(pin, "attachInterrupt");
    CORE_ASSERT(callback != NULL, "interrupt callback must not be NULL");

    // detach any existing interrupt
    detachInterrupt(pin);

    // assert EXTI channel is not already in use
    if (is_exti_channel_in_use(mapToInterruptSource(pin)))
    {
        // EXTI channel is already in use
        CORE_DEBUG_PRINTF("attachInterrupt: EXTI channel is already in use for pin=%d\n", pin);
        return -1;
    }

    // auto-assign a irqn
    IRQn_Type irqn;
    if (irqn_aa_get(irqn, "external interrupt") != Ok)
    {
        // no more IRQns available...
        CORE_DEBUG_PRINTF("attachInterrupt: no IRQn available for pin=%d\n", pin);
        return -1;
    }

    // insert pin -> irqn mapping
    insert_pin_to_irqn_mapping(pin, irqn);

    // set the interrupt
    _attachInterrupt(pin, callback, irqn, mode);
    CORE_DEBUG_PRINTF("attachInterrupt: pin=%d, irqn=%d, mode=%lu\n", pin, int(irqn), mode);

    // return assigned irqn
    return irqn;
}

void detachInterrupt(gpio_pin_t pin)
{
    ASSERT_GPIO_PIN_VALID(pin, "detachInterrupt");

    // get irqn for pin from mapping
    pin_to_irqn_mapping_t mapping;
    if (!get_pin_to_irqn_mapping(pin, mapping))
    {
        // no mapping for this pin...
        // CORE_ASSERT_FAIL("attempted to detach interrupt with no pin mapping")
        return;
    }

    // remove the interrupt
    IRQn_Type irqn = mapping.irqn;
    _detachInterrupt(pin, irqn);
    CORE_DEBUG_PRINTF("detachInterrupt: pin=%d, irqn=%u\n", pin, irqn);

    // clear irqn assignment and remove mapping
    remove_pin_to_irqn_mapping(pin);
    irqn_aa_resign(irqn, "external interrupt");
}

bool checkIRQFlag(gpio_pin_t pin, bool clear)
{
    ASSERT_GPIO_PIN_VALID(pin, "checkIRQFlag");

    en_exti_ch_t ch = mapToExternalInterruptChannel(pin);
    if (EXINT_IrqFlgGet(ch) == Set)
    {
        if (clear)
        {
            EXINT_IrqFlgClr(ch);
        }
        return true;
    }

    return false;
}

void setInterruptPriority(gpio_pin_t pin, uint32_t priority)
{
    ASSERT_GPIO_PIN_VALID(pin, "detachInterrupt");

    // get irqn for pin from mapping
    pin_to_irqn_mapping_t mapping;
    if (!get_pin_to_irqn_mapping(pin, mapping))
    {
        // no mapping for this pin...
        // CORE_ASSERT_FAIL("attempted to detach interrupt with no pin mapping")
        return;
    }

    // set the interrupt priority
    NVIC_SetPriority(mapping.irqn, priority);
}