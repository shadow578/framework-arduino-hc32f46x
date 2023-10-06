#include "IWatchdog.h"
#include "core_debug.h"
#include "drivers/sysclock/sysclock.h"
#include "drivers/irqn/irqn.h"
#include "core_hooks.h"

//
// global WDT object
//
IWatchdog WDT;

//
// IWatchdog class implementation
//

/**
 * @brief convert count cycle from uint32_t to en_wdt_count_cycle_t
 */
inline en_wdt_count_cycle_t convert_count_cycle(const uint32_t count_cycle)
{
    switch (count_cycle)
    {
    case 256:
        return WdtCountCycle256;
    case 4096:
        return WdtCountCycle4096;
    case 16384:
        return WdtCountCycle16384;
    case 65536:
        return WdtCountCycle65536;
    default:
        CORE_ASSERT_FAIL("IWatchdog::begin: invalid count cycle")
        return WdtCountCycle65536;
    }
}

/**
 * @brief convert clock divider from uint32_t to en_wdt_clk_div_t
 */
inline en_wdt_clk_div_t convert_clock_divider(const uint32_t clock_divider)
{
    switch (clock_divider)
    {
    case 4:
        return WdtPclk3Div4;
    case 64:
        return WdtPclk3Div64;
    case 128:
        return WdtPclk3Div128;
    case 256:
        return WdtPclk3Div256;
    case 512:
        return WdtPclk3Div512;
    case 1024:
        return WdtPclk3Div1024;
    case 2048:
        return WdtPclk3Div2048;
    case 8192:
        return WdtPclk3Div8192;
    default:
        CORE_ASSERT_FAIL("IWatchdog::begin: invalid clock divider")
        return WdtPclk3Div8192;
    }
}

/**
 * @brief auto-configure watchdog counter and clock divider
 *
 * @param target_timeout target timeout in milliseconds
 * @param base_clock base clock frequency in Hz
 * @param count_cycle watchdog count cycle output
 * @param clock_divider watchdog clock divider output
 * @return actual timeout in milliseconds
 */
inline uint32_t configure_wdt_counter(const uint32_t target_timeout,
                                      const uint32_t base_clock,
                                      uint32_t &count_cycle,
                                      uint32_t &clock_divider)
{
    // configure available values
    constexpr uint32_t clock_dividers[] = {4, 64, 128, 256, 512, 1024, 2048, 8192};
    constexpr uint32_t clock_dividers_length = sizeof(clock_dividers) / sizeof(clock_dividers[0]);
    constexpr uint32_t count_cycles[] = {256, 4096, 16384, 65536};
    constexpr uint32_t count_cycles_length = sizeof(count_cycles) / sizeof(count_cycles[0]);

    // test every clock divider and count cycle combination
    uint32_t min_error = target_timeout;
    uint32_t best_timeout = 0;
    for (uint32_t cdi = 0; cdi < clock_dividers_length; cdi++)
        for (uint32_t cci = 0; cci < count_cycles_length; cci++)
        {
            const uint32_t cd = clock_dividers[cdi];
            const uint32_t cc = count_cycles[cci];

            // calculate timeout for this combination
            // timeout = 1000 * (cc / (base_clock / cd))
            const uint32_t timeout = ((cc * cd) / base_clock) * 1000;

            // if this combination is better than the current one, use it
            const uint32_t error = abs(target_timeout - timeout);
            if (error < min_error)
            {
                min_error = error;
                count_cycle = cc;
                clock_divider = cd;
                best_timeout = timeout;
            }
        }

    // return calculated timeout
    return best_timeout;
}

/**
 * @brief get watchdog base clock frequency (PCLK3)
 */
inline uint32_t get_wdt_base_clock()
{
    update_system_clock_frequencies();
    return SYSTEM_CLOCK_FREQUENCIES.pclk3;
}

/**
 * @brief register watchdog callback interrupt
 *
 * @param callback callback function pointer
 *
 * @note enRequestType must be set to WdtTriggerInterruptRequest
 */
inline void register_wdt_callback(const voidFuncPtr callback)
{
    // get auto-assigned irqn
    IRQn_Type irqn;
    irqn_aa_get(irqn, "WDT callback");

    // create IRQ config
    stc_irq_regi_conf_t irq_config = {
        .enIntSrc = INT_WDT_REFUDF,
        .enIRQn = irqn,
        .pfnCallback = callback,
    };

    // register and enable IRQ
    enIrqRegistration(&irq_config);
    NVIC_ClearPendingIRQ(irqn);
    NVIC_SetPriority(irqn, DDL_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(irqn);
}

uint32_t IWatchdog::begin(const uint32_t timeout_ms, const voidFuncPtr callback)
{
    // auto-configure counter and clock divider
    const uint32_t base_clock = get_wdt_base_clock();
    uint32_t count_cycle = 0;
    uint32_t clock_divider = 0;
    const uint32_t actual_timeout = configure_wdt_counter(timeout_ms, base_clock, count_cycle, clock_divider);

    // call begin
    begin(clock_divider, count_cycle, callback);

    CORE_DEBUG_PRINTF("IWatchdog::begin: timeout=%dms, actual_timeout=%dms, base_clock=%d\n",
                      int(timeout_ms), int(actual_timeout), int(base_clock));
    return actual_timeout;
}

void IWatchdog::begin(const uint32_t divider, const uint32_t count_cycle, const voidFuncPtr callback)
{
    // create configuration
    stc_wdt_init_t config = {
        .enCountCycle = convert_count_cycle(count_cycle),
        .enClkDiv = convert_clock_divider(divider),
        .enRefreshRange = WdtRefresh0To100Pct,
        .enSleepModeCountEn = Disable,

        // request interrupt if callback is set, otherwise reset
        .enRequestType = callback != NULL ? WdtTriggerInterruptRequest : WdtTriggerResetRequest,
    };

    // register callback if set
    if (callback != NULL)
    {
        register_wdt_callback(callback);
    }

    // call begin() with configuration
    begin(&config);

    CORE_DEBUG_PRINTF("IWatchdog::begin: divider=%d, count_cycle=%d\n",
                      int(divider), int(count_cycle));
}

void IWatchdog::begin(const stc_wdt_init_t *config)
{
    CORE_ASSERT(config != NULL, "IWatchdog::begin: config is NULL")

    en_result_t rc = WDT_Init(config);
    CORE_ASSERT(rc == Ok, "IWatchdog::begin: WDT_Init failed")

    this->initialized = true;

    // reload watchdog after init to start it
    reload();
}

void IWatchdog::reload(void)
{
    if (!this->initialized)
    {
        return;
    }

    WDT_RefreshCounter();
}

uint16_t IWatchdog::getCounter(void)
{
    if (!this->initialized)
    {
        return 0;
    }

    return WDT_GetCountValue();
}

void core_hook_yield_wdt_reload(void)
{
    // reload watchdog on yield() calls
    WDT.reload();
}
