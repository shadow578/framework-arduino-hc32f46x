#include "Timer0.h"
#include <drivers/sysclock/sysclock.h>
#include <drivers/irqn/irqn.h>

//
// helpers
//

/**
 * @brief timer register to timer number
 */
#define TIMER0x_REG_TO_X(reg) reg == M4_TMR01 ? 1 : 2

/**
 * @brief timer channel to channel string
 */
#define TIMER0x_CHANNEL_TO_CH_STR(channel) \
    channel == Tim0_ChannelA ? "A" : "B"

/**
 * @brief debug print helper
 */
#define TIMER0_DEBUG_PRINTF(fmt, ...)                                              \
    CORE_DEBUG_PRINTF("[Timer0%d%s] " fmt,                                         \
                      TIMER0x_REG_TO_X(this->config->peripheral.register_base),    \
                      TIMER0x_CHANNEL_TO_CH_STR(this->config->peripheral.channel), \
                      ##__VA_ARGS__)

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
inline void timer0_irq_register(timer0_interrupt_config_t &irq, voidFuncPtr callback, const char *name)
{
    // get auto-assigned irqn and set in irq struct
    IRQn_Type irqn;
    irqn_aa_get(irqn, name);
    irq.interrupt_number = irqn;

    // create irq registration struct
    stc_irq_regi_conf_t irqConf = {
        .enIntSrc = irq.interrupt_source,
        .enIRQn = irq.interrupt_number,
        .pfnCallback = callback,
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
Timer0::Timer0(timer0_channel_config_t *config, voidFuncPtr callback)
{
    CORE_ASSERT(config != nullptr, "Timer0 config must not be null")
    CORE_ASSERT(callback != nullptr, "Timer0 callback must not be null")

    this->config = config;
    this->callback = callback;
}

void Timer0::start(const uint32_t frequency, const uint16_t prescaler)
{
    stc_tim0_base_init_t channel_config;
    channel_config.Tim0_AsyncClockSource = Tim0_LRC;
    channel_config.Tim0_SyncClockSource = Tim0_Pclk1;

    // setup channel clock source and set base frequency
    uint32_t base_frequency;
    if (this->config->peripheral.register_base == M4_TMR01 && this->config->interrupt.interrupt_source == INT_TMR01_GCMA)
    {
        // Timer0 Unit 1 Channel A does not support Sync mode, and thus does not support PCLK1 as clock source
        // instead, LRC is used as clock source

        // get LRC frequency (fixed, calibrated at factory)
        base_frequency = LRC_VALUE;

        // set channel clock source:
        // Async mode, LRC
        channel_config.Tim0_CounterMode = Tim0_Async;
        TIMER0_DEBUG_PRINTF("using Timer01A is not recommended, as it does not support sync mode\n");
    }
    else
    {
        // all other Timer channels support PCLK1 as clock source
        // update clock frequencies and get PCLK1 frequency
        update_system_clock_frequencies();
        base_frequency = SYSTEM_CLOCK_FREQUENCIES.pclk1;

        // set channel clock source:
        // Sync mode, PCLK1
        channel_config.Tim0_CounterMode = Tim0_Sync;
    }

    // calculate the compare value needed to match the target frequency
    // CMP = (base_freq / prescaler) / frequency
    uint32_t compare = (base_frequency / uint32_t(prescaler)) / frequency;

    // ensure compare value does not exceed 16 bits, and larger than 0
    CORE_ASSERT(compare > 0 && compare <= 0xFFFF, "Timer0::start(): compare value exceeds 16 bits");

    // set prescaler and compare value
    channel_config.Tim0_ClockDivision = numeric_to_clock_div(prescaler);
    channel_config.Tim0_CmpValue = uint16_t(compare);

    // debug print auto-config values
    TIMER0_DEBUG_PRINTF("auto-found cmp= %d for fBase=%d and prescaler=%d\n", int(compare), int(base_frequency), int(prescaler));

    // start timer channel with config
    start(&channel_config);
}

void Timer0::start(const stc_tim0_base_init_t *channel_config)
{
    CORE_ASSERT(channel_config != nullptr, "Timer0::start(): channel_config is null");

    // if already started, stop first
    if (this->isStarted)
    {
        stop();
    }

    // enable Timer0 peripheral clock
    PWC_Fcg2PeriphClockCmd(this->config->peripheral.clock_id, Enable);

    // enable LRC clock if used
    if (channel_config->Tim0_CounterMode == Tim0_Async && channel_config->Tim0_AsyncClockSource == Tim0_LRC)
    {
        CLK_LrcCmd(Enable);
    }

    // initialize timer channel
    TIMER0_BaseInit(this->config->peripheral.register_base, this->config->peripheral.channel, channel_config);

    // register interrupt
    CORE_ASSERT(this->callback != nullptr, "Timer0::start(): callback not set");
    timer0_irq_register(this->config->interrupt, this->callback, "Timer0");

    // enable timer interrupt
    TIMER0_IntCmd(this->config->peripheral.register_base, this->config->peripheral.channel, Enable);

    // set channel initialized flag
    this->isStarted = true;

    TIMER0_DEBUG_PRINTF("started with compare value %d\n",
                        channel_config->Tim0_CmpValue);
}

void Timer0::stop()
{
    if (!this->isStarted)
    {
        return;
    }

    // pause timer
    pause();

    // reset channel initialized flag early
    this->isStarted = false;

    // disable timer interrupt
    TIMER0_IntCmd(this->config->peripheral.register_base, this->config->peripheral.channel, Disable);

    // resign interrupt
    timer0_irq_resign(this->config->interrupt, "Timer0");

    // de-init timer channel
    TIMER0_DeInit(this->config->peripheral.register_base, this->config->peripheral.channel);

    TIMER0_DEBUG_PRINTF("stopped channel\n");
}
