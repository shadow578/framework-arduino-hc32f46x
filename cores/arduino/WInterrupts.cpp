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

    CORE_ASSERT_FAIL("Invalid external interrupt mode")
    return ExIntFallingEdge;
}

inline en_exti_ch_t mapToExternalInterruptChannel(uint32_t pin)
{
    // check range
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

inline en_int_src_t mapToInterruptSource(uint32_t pin)
{
    // check range
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

void _attachInterrupt(uint32_t pin, voidFuncPtr handler, IRQn_Type irqn, uint32_t mode)
{
    // check inputs
    if (pin >= BOARD_NR_GPIO_PINS || !handler)
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
    NVIC_SetPriority(irqReg.enIRQn, DDL_IRQ_PRIORITY_06);
    NVIC_EnableIRQ(irqReg.enIRQn);
}

void _detachInterrupt(uint32_t pin, IRQn_Type irqn)
{
    // check inputs
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
    uint32_t pin;
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
inline void insert_pin_to_irqn_mapping(uint32_t pin, IRQn_Type irqn)
{
    // create new node
    pin_to_irqn_mapping_t node = {
        .pin = pin,
        .irqn = irqn,
        .next = NULL,
    };

    // insert node...
    if (pin_to_irqn_mapping == NULL)
    {
        // ... as head
        pin_to_irqn_mapping = &node;
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
        parent->next = &node;
    }
}

/**
 * @brief get a pin -> irqn mapping by pin number
 */
inline bool get_pin_to_irqn_mapping(uint32_t pin, pin_to_irqn_mapping_t &mapping)
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
inline void remove_pin_to_irqn_mapping(uint32_t pin)
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

// #endregion

int attachInterrupt(uint32_t pin, voidFuncPtr callback, uint32_t mode)
{
    // detach any existing interrupt
    detachInterrupt(pin);

    // auto-assign a irqn
    IRQn_Type irqn;
    if (irqn_aa_get(irqn, "external interrupt") != Ok)
    {
        // no more IRQns available...
        // CORE_ASSERT_FAIL("no more IRQns available for external interrupts")
        CORE_DEBUG_PRINTF("attachInterrupt: no IRQn available for pin=%lu\n", pin);
        return -1;
    }

    // insert pin -> irqn mapping
    insert_pin_to_irqn_mapping(pin, irqn);

    // set the interrupt
    _attachInterrupt(pin, callback, irqn, mode);
    CORE_DEBUG_PRINTF("attachInterrupt: pin=%lu, irqn=%d, mode=%lu\n", pin, int(irqn), mode);

    // return assigned irqn
    return irqn;
}

void detachInterrupt(uint32_t pin)
{
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
    CORE_DEBUG_PRINTF("detachInterrupt: pin=%ld, irqn=%u\n", pin, irqn);

    // clear irqn assignment and remove mapping
    remove_pin_to_irqn_mapping(pin);
    irqn_aa_resign(irqn, "external interrupt");
}

bool checkIRQFlag(uint32_t pin, bool clear)
{
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
