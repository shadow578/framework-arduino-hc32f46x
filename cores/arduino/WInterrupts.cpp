#include "WInterrupts.h"
#include "wiring_constants.h"
#include "core_debug.h"
#include "drivers/gpio/gpio.h"
#include <hc32_ddl.h>

// configuration for IRQn auto-assignment
#define AVAILABLE_IRQn_COUNT 10                       // make 10 IRQn available for use with external interrupts
#define FIRST_IRQn 10                                 // the first available is IRQ10
#define LAST_IRQn (FIRST_IRQn + AVAILABLE_IRQn_COUNT) // the last available will be IRQ20

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
    uint8_t ch = PIN_MAP[pin].gpio_bit;
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
    uint8_t ch = PIN_MAP[pin].gpio_bit;
    if (ch < 0 || ch > 15)
    {
        // invalid source, only 00 - 15 are valid
        CORE_ASSERT_FAIL("invalid external interrupt source")
        return INT_PORT_EIRQ0;
    }

    // interrupt source maps 1-to-1
    return (en_int_src_t)ch;
}

inline IRQn_Type mapToIQRVector(uint8_t n)
{
    // map 0-10 to IRQ 10-20
    uint8_t vec = FIRST_IRQn + n;
    CORE_ASSERT(vec >= FIRST_IRQn && vec <= LAST_IRQn, "external interrupt IRQn out of range")
    return (IRQn_Type)vec;
}

void _attachInterrupt(uint32_t pin, voidFuncPtr handler, uint8_t irqNum, uint32_t mode)
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
        .enIRQn = mapToIQRVector(irqNum),
        .pfnCallback = handler};
    enIrqRegistration(&irqReg);

    // clear pending, set priority and enable
    NVIC_ClearPendingIRQ(irqReg.enIRQn);
    NVIC_SetPriority(irqReg.enIRQn, DDL_IRQ_PRIORITY_06);
    NVIC_EnableIRQ(irqReg.enIRQn);
}

void _detachInterrupt(uint32_t pin, uint8_t irqNum)
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
    IRQn_Type irqVec = mapToIQRVector(irqNum);
    NVIC_ClearPendingIRQ(irqVec);
    NVIC_DisableIRQ(irqVec);
    enIrqResign(irqVec);
}
// #endregion

// #region IRQn auto-assignment
#define IRQn_MAPPING_NONE 0xff

/**
 * maps IRQn to pin number using that IRQn
 * [IRQn] = <pin_nr>|IRQn_MAPPING_NONE
 */
static uint32_t pinToIRQnMapping[AVAILABLE_IRQn_COUNT];

/**
 * get the next IRQn that is available for use
 */
inline bool getNextFreeIRQn(uint8_t &irqn)
{
    // find the first free IRQn
    for (int i = 0; i < AVAILABLE_IRQn_COUNT; i++)
    {
        if (pinToIRQnMapping[i] == IRQn_MAPPING_NONE)
        {
            irqn = i;
            return true;
        }
    }

    // if we got here, no more IRQns are available...
    return false;
}

/**
 * get the IRQn that is assigned to a pin's interrupt.
 */
inline bool getIRQnForPin(uint32_t pin, uint8_t &irqn)
{
    // linear search the pin in the mapping
    for (int i = 0; i < AVAILABLE_IRQn_COUNT; i++)
    {
        if (pinToIRQnMapping[i] == pin)
        {
            irqn = i;
            return true;
        }
    }

    // if we got here, no IRQn is assigned to the pin...
    return false;
}

/**
 * assign a IRQn to a pin
 */
inline void assignIRQn(uint32_t pin, uint8_t irqn)
{
    CORE_ASSERT(pinToIRQnMapping[irqn] == IRQn_MAPPING_NONE, "attempted to assign already assigned IRQn");
    pinToIRQnMapping[irqn] = pin;
}

/**
 * clear the assignment of a IRQn to a pin
 */
inline void clearIRQnAssignment(uint32_t pin, uint8_t irqn)
{
    CORE_ASSERT(pinToIRQnMapping[irqn] == pin, "attempted to clear IRQn assignment for unassigned IRQn");
    pinToIRQnMapping[irqn] = IRQn_MAPPING_NONE;
}
// #endregion

int attachInterrupt(uint32_t pin, voidFuncPtr callback, uint32_t mode)
{
    // detach any existing interrupt
    detachInterrupt(pin);

    // auto-assign a irqn
    uint8_t irqn;
    if (!getNextFreeIRQn(irqn))
    {
        // no more IRQns available...
        //CORE_ASSERT_FAIL("no more IRQns available for external interrupts")
        CORE_DEBUG_PRINTF("attachInterrupt: no IRQn available for pin=%lu\n", pin);
        return -1;
    }

    // set the assignment
    assignIRQn(pin, irqn);

    // set the interrupt
    _attachInterrupt(pin, callback, irqn, mode);
    CORE_DEBUG_PRINTF("attachInterrupt: pin=%lu, irqn=%u, mode=%lu\n", pin, irqn, mode);

    // return assigned irqn
    return irqn;
}

void detachInterrupt(uint32_t pin)
{
    // get IRQn for the pin
    uint8_t irqn;
    if (!getIRQnForPin(pin, irqn))
    {
        // no IRQn assigned to the pin...
        //CORE_ASSERT_FAIL("attempted to free IRQn that was not assigned yet")
        return;
    }

    // remove the interrupt
    _detachInterrupt(pin, irqn);
    CORE_DEBUG_PRINTF("detachInterrupt: pin=%ld, irqn=%u\n", pin, irqn);

    // clear irqn assignment
    clearIRQnAssignment(pin, irqn);
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

void interrupts_init()
{
    // set all IRQn assignments to NONE
    for (int i = 0; i < AVAILABLE_IRQn_COUNT; i++)
    {
        pinToIRQnMapping[i] = IRQn_MAPPING_NONE;
    }
}
