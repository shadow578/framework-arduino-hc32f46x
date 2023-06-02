#include "Timer0.h"
#include <drivers/sysclock/sysclock.h>
#include <drivers/irqn/irqn.h>
#include <core_debug.h>

//
// helpers
//
#define CH_A Timer0Channel::A
#define CH_B Timer0Channel::B

/**
 * @brief Convert Timer0Channel to en_tim0_channel_t
 */
#define CHANNEL_TO_DDL_CHANNEL(channel) \
    ((channel == CH_A) ? Tim0_ChannelA : Tim0_ChannelB)

/**
 * @brief timer register to timer number
 */
#define TIMER0x_REG_TO_X(reg) reg == M4_TMR01 ? 1 : 2

/**
 * @brief timer channel to string
 */
#define TIMER0_CHANNEL_TO_STR(channel) \
    (channel == CH_A) ? "A" : "B"

/**
 * @brief debug print helper
 */
#define TIMER0_DEBUG_PRINTF(fmt, ...) \
    CORE_DEBUG_PRINTF("[Timer0%d] " fmt, TIMER0x_REG_TO_X(this->config->peripheral.register_base), ##__VA_ARGS__)

/**
 * @brief convert numerical value to en_tim0_clock_div_t
 * @note assert fails if invalid value
 */
inline en_tim0_clock_div_t numeric_to_clock_div(const uint16_t n)
{
    switch (n)
    {
    case 0:
    case 1:
        return Tim0_ClkDiv0;
    case 2:
        return Tim0_ClkDiv2;
    case 4:
        return Tim0_ClkDiv4;
    case 8:
        return Tim0_ClkDiv8;
    case 16:
        return Tim0_ClkDiv16;
    case 32:
        return Tim0_ClkDiv32;
    case 64:
        return Tim0_ClkDiv64;
    case 128:
        return Tim0_ClkDiv128;
    case 256:
        return Tim0_ClkDiv256;
    case 512:
        return Tim0_ClkDiv512;
    case 1024:
        return Tim0_ClkDiv1024;
    default:
        CORE_ASSERT_FAIL("Invalid clock divider value");
        return Tim0_ClkDiv0;
    }
}

/**
 * @brief timer0 interrupt registration
 */
inline void timer0_irq_register(timer0_interrupt_config_t &irq, const char *name)
{
    // get auto-assigned irqn and set in irq struct
    IRQn_Type irqn;
    irqn_aa_get(irqn, name);
    irq.interrupt_number = irqn;

    // ensure user callback is set to NULL
    irq.user_callback = NULL;

    // create irq registration struct
    stc_irq_regi_conf_t irqConf = {
        .enIntSrc = irq.interrupt_source,
        .enIRQn = irq.interrupt_number,
        .pfnCallback = irq.interrupt_handler,
    };

    // register and enable irq with default priority
    enIrqRegistration(&irqConf);
    NVIC_SetPriority(irqConf.enIRQn, DDL_IRQ_PRIORITY_DEFAULT);
    NVIC_ClearPendingIRQ(irqConf.enIRQn);
    NVIC_EnableIRQ(irqConf.enIRQn);
}

/**
 * @brief timer0 interrupt resign
 */
inline void timer0_irq_resign(timer0_interrupt_config_t &irq, const char *name)
{
    // disable interrupt and clear pending
    NVIC_DisableIRQ(irq.interrupt_number);
    NVIC_ClearPendingIRQ(irq.interrupt_number);
    enIrqResign(irq.interrupt_number);

    // resign auto-assigned irqn
    irqn_aa_resign(irq.interrupt_number, name);
}

//
// Timer0 class implementation
//
Timer0::Timer0(timer0_config_t *config)
{
    this->config = config;
}

void Timer0::start(const Timer0Channel channel, const uint32_t frequency, const uint16_t prescaler)
{
    // TODO non-custom start() only supports sync mode, and thus only supports Unit 2
    CORE_ASSERT(this->config->peripheral.register_base == M4_TMR02, "automatic start() is only supported for Timer0 Unit 2 (M4_TMR02)");

    // update clock frequencies and get PCLK1 frequency
    update_system_clock_frequencies();
    uint32_t pclk1_freq = SYSTEM_CLOCK_FREQUENCIES.pclk1;

    // calculate the compare value needed to match the target frequency
    // CMP = (PCLK1 / prescaler) / frequency
    uint32_t compare = (pclk1_freq / uint32_t(prescaler)) / frequency;

    // ensure compare value does not exceed 16 bits
    CORE_ASSERT(compare <= 0xFFFF, "Timer0::start(): compare value exceeds 16 bits");

    // build timer channel config
    stc_tim0_base_init_t channel_config = {
        .Tim0_ClockDivision = numeric_to_clock_div(prescaler),
        .Tim0_SyncClockSource = Tim0_Pclk1,
        .Tim0_CounterMode = Tim0_Sync,
        .Tim0_CmpValue = uint16_t(compare),
    };

    // start timer channel with config
    start(channel, &channel_config);
}

void Timer0::start(const Timer0Channel channel, const stc_tim0_base_init_t *channel_config)
{
    // enable Timer0 peripheral clock
    PWC_Fcg2PeriphClockCmd(this->config->peripheral.clock_id, Enable);

    // initialize timer channel
    TIMER0_BaseInit(this->config->peripheral.register_base, CHANNEL_TO_DDL_CHANNEL(channel), channel_config);

    // register interrupt
    if (channel == CH_A)
    {
        timer0_irq_register(this->config->channel_a_interrupt, "Timer0x Channel A");
    }
    else
    {
        timer0_irq_register(this->config->channel_b_interrupt, "Timer0x Channel B");
    }

    // enable timer interrupt
    TIMER0_IntCmd(this->config->peripheral.register_base, CHANNEL_TO_DDL_CHANNEL(channel), Enable);

    // set channel initialized flag
    setChannelInitialized(channel, true);

    TIMER0_DEBUG_PRINTF("started channel %s with compare value %d\n",
                        TIMER0_CHANNEL_TO_STR(channel),
                        channel_config->Tim0_CmpValue);
}

void Timer0::stop(const Timer0Channel channel)
{
    // pause timer
    pause(channel);

    // reset channel initialized flag early
    setChannelInitialized(channel, false);

    // disable timer interrupt
    TIMER0_IntCmd(this->config->peripheral.register_base, CHANNEL_TO_DDL_CHANNEL(channel), Disable);

    // resign interrupt
    if (channel == CH_A)
    {
        timer0_irq_resign(this->config->channel_a_interrupt, "Timer0x Channel A");
    }
    else
    {
        timer0_irq_resign(this->config->channel_b_interrupt, "Timer0x Channel B");
    }

    // de-init timer channel
    TIMER0_DeInit(this->config->peripheral.register_base, CHANNEL_TO_DDL_CHANNEL(channel));

    TIMER0_DEBUG_PRINTF("stopped channel %s\n", TIMER0_CHANNEL_TO_STR(channel));
}

void Timer0::pause(const Timer0Channel channel)
{
    // if not initialized, return false
    if (!isChannelInitialized(channel))
    {
        return;
    }

    TIMER0_Cmd(this->config->peripheral.register_base, CHANNEL_TO_DDL_CHANNEL(channel), Disable);
    // TIMER0_DEBUG_PRINTF("paused channel %s\n", TIMER0_CHANNEL_TO_STR(channel));
}

void Timer0::resume(const Timer0Channel channel)
{
    // if not initialized, return false
    if (!isChannelInitialized(channel))
    {
        return;
    }

    TIMER0_Cmd(this->config->peripheral.register_base, CHANNEL_TO_DDL_CHANNEL(channel), Enable);
    // TIMER0_DEBUG_PRINTF("resumed channel %s\n", TIMER0_CHANNEL_TO_STR(channel));
}

bool Timer0::isPaused(const Timer0Channel channel)
{
    // if not initialized, return false
    if (!isChannelInitialized(channel))
    {
        return false;
    }

    // otherwise, return the channel status
    if (channel == CH_A)
    {
        return !bool(this->config->peripheral.register_base->BCONR_f.CSTA);
    }
    else
    {
        return !bool(this->config->peripheral.register_base->BCONR_f.CSTB);
    }
}

void Timer0::setCompareValue(const Timer0Channel channel, const uint16_t compare)
{
    CORE_ASSERT(isChannelInitialized(channel), "Timer0::setCompare(): channel not initialized");
    TIMER0_WriteCmpReg(this->config->peripheral.register_base, CHANNEL_TO_DDL_CHANNEL(channel), compare);
}

uint16_t Timer0::getCount(const Timer0Channel channel)
{
    CORE_ASSERT(isChannelInitialized(channel), "Timer0::getCount(): channel not initialized");
    return TIMER0_GetCntReg(this->config->peripheral.register_base, CHANNEL_TO_DDL_CHANNEL(channel));
}

void Timer0::setCallback(const Timer0Channel channel, voidFuncPtr callback)
{
    if (channel == CH_A)
    {
        this->config->channel_a_interrupt.user_callback = callback;
    }
    else
    {
        this->config->channel_b_interrupt.user_callback = callback;
    }

    TIMER0_DEBUG_PRINTF("set user callback for channel %s\n", TIMER0_CHANNEL_TO_STR(channel));
}

void Timer0::setCallbackPriority(const Timer0Channel channel, const uint32_t priority)
{
    if (channel == CH_A)
    {
        NVIC_SetPriority(this->config->channel_a_interrupt.interrupt_number, priority);
    }
    else
    {
        NVIC_SetPriority(this->config->channel_b_interrupt.interrupt_number, priority);
    }

    TIMER0_DEBUG_PRINTF("set user callback priority for channel %s to %d\n", TIMER0_CHANNEL_TO_STR(channel), int(priority));
}
