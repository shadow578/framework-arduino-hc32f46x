/**
 * TimerA PWM utility functions:
 *
 * for PWM (and Servo) output, the TimerA unit is configured in Sawtooth counting mode.
 * the TimerA unit is configured to count up to a given value (PERAR) and then reset to 0.
 * when the timer value matches the PERAR value, the output is set to HIGH.
 * when the timer value matches the channel compare value (cmp_s), the output is set to LOW.
 * Thus, the pulse on and off time can be controlled precisely by setting the PERAR and cmp_s values,
 * where PERAR defines the overall frequency and cmp_s defines the pulse (on) time (= duty cycle).
 *
 *
 *              |<------------ T ------------->|
 *
 *   PERAR |---/|-----------------------------/|----
 *         | /  |                           /  |   ^
 *         |    |                         /    |   |
 *         |    |                       /      |   |
 *         |    |                     /        |   | delta_cmp_s
 *         |    |                   /          |   |
 *         |    |                 /            |   v
 *   cmp_s |--- | --------------/|------------ | ---
 *         |    |             /  |             |
 *         |    |           /    |             |
 *         |    |         /      |             |
 *         |    |       /        |             |
 *         |    |     /          |             |
 *         |    |   /            |             |  /
 *         |    | /              |             |/
 *       0 |-----------------------------------------> t
 *              |                |             |
 *              |                |             |
 *              |                |             |
 *              |                |             |
 *      5v |     ________________              |
 *         |    |                |             |
 *         |    ^                v             |
 *         |    |                |             |
 *         |    |                |             |
 *      0v |-----------------------------------------> t
 *              |<--- T_s_on --->|<- T_s_off ->|
 *
 * * T           = "wave period time"
 * * T_s_on      = "pulse on time"
 * * t_s_off     = "pulse off time"
 * * PERAR       = "period compare value"
 * * cmp_s       = "channel compare value"
 * * delta_cmp_s = "difference between PERAR and cmp_s"
 * * f_base      = "base counting frequency (PCLK1 / divider)"
 *
 *
 * PERAR = (f_base [Hz] * T [s]) / 1 [s/s]
 *       = (f_base [Hz] * T [ms]) / 1.000 [ms/s]
 *       = (f_base [Hz] * T [us]) / 1.000.000 [us/s]
 *
 *
 * cmp_s = (f_base [Hz] * T_s_on [s]) / 1 [s/s]
 *       = (f_base [Hz] * T_s_on [ms]) / 1.000 [ms/s]
 *       = (f_base [Hz] * T_s_on [us]) / 1.000.000 [us/s]
 *
 * delta_cmp_s = PERAR - cmp_s
 *
 * - OR -
 *
 * delta_cmp_s = (f_base [Hz] * T_s_on [s]) / 1 [s/s]
 *             = (f_base [Hz] * T_s_on [ms]) / 1.000 [ms/s]
 *             = (f_base [Hz] * T_s_on [us]) / 1.000.000 [us/s]
 *
 * cmp_s = PERAR - delta_cmp_s
 *
 * - OR -
 *
 * cmp_s = (f_base [Hz] * (T [s] - T_s_off [s])) / 1 [s/s]
 *       = (f_base [Hz] * (T [ms] - T_s_off [ms])) / 1.000 [ms/s]
 *       = (f_base [Hz] * (T [us] - T_s_off [us])) / 1.000.000 [us/s]
 *
 * delta_cmp_s = PERAR - cmp_s
 *
 *
 * example 1 (servo signal @ 50 Hz, 0° position):
 * f_base = (50 MHz / 16)   = 3.125.000 Hz
 * T      = (1.000 / 50 Hz) = 20 ms
 * T_s_on = 500 us
 *
 *
 * PERAR = (3.125.000 [Hz] * 20 [ms]) / 1.000 [ms/s]
 *       = 62.500
 *
 * cmp_s = (3.125.000 [Hz] * 500 [us]) / 1.000.000 [us/s]
 *       = 1.562,5
 *       =~ 1.562
 *
 * delta_cmp_s = 62.500 - 1.562
 *             = 60.938
 *
 *
 * example 2 (PWM signal @ 1KHz, 200us off-time):
 * f_base  = (50 MHz / 16) = 3.125.000 Hz
 * T       = (1.000 / 1 KHz) = 1 ms
 * T_s_off = 200 us
 *
 * PERAR = (3.125.000 [Hz] * 1 [ms]) / 1.000 [ms/s]
 *       = 3.125
 *
 * T_s_on = 1 [ms] - 200 [us]
 *        = 800 [us]
 *
 * cmp_s = (3.125.000 [Hz] * 800 [us]) / 1.000.000 [us/s]
 *        = 2.500
 *
 * delta_cmp_s = 3.125 - 2.500
 *             = 625
 */

#include "timera_util.h"

#define TIMERA_PWM_UNIT_S 1
#define TIMERA_PWM_UNIT_MS 1000
#define TIMERA_PWM_UNIT_US 1000000

/**
 * @brief calculate counter compare value (PERAR or cmp_s) given period time
 * @param f_base base counting frequency (PCLK1 / divider)
 * @param T period time, in the given unit
 * @param unit unit of period time (TIMERA_PWM_UNIT_S, TIMERA_PWM_UNIT_MS, TIMERA_PWM_UNIT_US)
 *
 * @note using to-low bitness variables can lead to errors. use 64-bit variables if possible.
 */
#define TIMERA_PWM_CALC_CMP_FROM_PERIOD(f_base, T, unit) ((f_base * T) / unit)

/**
 * @brief calculate period time given counter compare value (PERAR or cmp_s)
 * @param f_base base counting frequency (PCLK1 / divider)
 * @param cmp_s counter compare value (PERAR or cmp_s)
 * @param unit unit of period time (TIMERA_PWM_UNIT_S, TIMERA_PWM_UNIT_MS, TIMERA_PWM_UNIT_US)
 *
 * @note using to-low bitness variables can lead to errors. use 64-bit variables if possible.
 */
#define TIMERA_PWM_CALC_PERIOD_FROM_CMP(f_base, cmp_s, unit) ((cmp_s * unit) / f_base)

/**
 * @brief frequency to period time
 */
#define FREQ_TO_PERIOD(f, unit) ((unit) / (f))

/**
 * @brief period time to frequency
 */
#define PERIOD_TO_FREQ(p, unit) ((unit) / (p))

/**
 * @brief constrain a value to a given range
 * @note copy from Arduino.h to avoid dependency
 */
#define TIMERA_CONSTRAIN(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

en_result_t timera_pwm_channel_stop(timera_config_t *unit, const en_timera_channel_t channel);

/**
 * @brief initialize a timera unit for PWM output
 * @param unit pointer to timera unit config
 * @param frequency PWM frequency in Hz
 * @param divider clock divider. one of [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024]
 * @param allow_use_incompatible_config if true, if the unit is already initialized with a different config, the
 *                                      function will try to use the existing config if possible.
 *                                      timera_pwm* functions will still work, but the frequency may be wrong.
 *                                      if false, the function will fail with ErrorOperationInProgress
 * @return Ok on success,
 *         ErrorInvalidParameter if parameters not valid,
 *         ErrorOperationInProgress if unit already initialized with a incompatible config
 */
inline en_result_t timera_pwm_start(timera_config_t *unit,
                                    const uint32_t frequency,
                                    const uint32_t divider,
                                    const bool allow_use_incompatible_config = true)
{
    CORE_ASSERT(unit != nullptr, "timera_pwm_unit_start: unit is nullptr", return ErrorInvalidParameter);
    CORE_ASSERT(divider > 0 && divider <= 1024, "timera_pwm_unit_start: divider is invalid", return ErrorInvalidParameter);

    // get base counting frequency /w given divider
    const int64_t f_base = timera_get_base_clock() / divider;

    // calculate wave period in us for maximum precision
    const int64_t T = FREQ_TO_PERIOD(frequency, TIMERA_PWM_UNIT_US);

    // calculate compare value for PERAR
    const int64_t PERAR = TIMERA_PWM_CALC_CMP_FROM_PERIOD(f_base, T, TIMERA_PWM_UNIT_US);
    CORE_ASSERT(PERAR > 0 && PERAR <= 0xFFFF, "timera_pwm_unit_start: PERAR is out-of-range", return Error);

    // prepare unit config
    // (when initializing, a pointer to this is stored in the unit's state. so we need to allocate it on the heap)
    stc_timera_base_init_t *unit_config = new stc_timera_base_init_t;
    unit_config->enClkDiv = timera_n_to_clk_div(divider);     // PCLK1 / divider
    unit_config->enCntMode = TimeraCountModeSawtoothWave;     // sawtooth wave mode
    unit_config->enCntDir = TimeraCountDirUp;                 // count up
    unit_config->enSyncStartupEn = Disable;                   // no sync startup
    unit_config->u16PeriodVal = static_cast<uint16_t>(PERAR); // 0..PERAR,0

    TIMERA_DEBUG_PRINTF(unit, -2, "pwm_start: pwm init with f=%ldHz (PCLK1/%ld), f_base=%ld (T=%ld), PERAR=%d\n", frequency, divider, int32_t(f_base), int32_t(T), uint16_t(PERAR));

    // initialize unit
    if (timera_is_unit_initialized(unit))
    {
        TIMERA_DEBUG_PRINTF(unit, -2, "pwm_start: check existing config\n");

        // already initialized, get config from state
        stc_timera_base_init_t *current_config = unit->state.base_init;

        // check if current config happens to match the one we want to set:
        // - if the counting mode or direction are different, PWM cannot work
        if (current_config->enCntMode != unit_config->enCntMode ||
            current_config->enCntDir != unit_config->enCntDir)
        {
            // free heap memory
            delete unit_config;
            return ErrorOperationInProgress;
        }

        // - if clock divider or PERAR are different, we can still use the unit, but the frequency will be wrong
        if (current_config->enClkDiv != unit_config->enClkDiv ||
            current_config->u16PeriodVal != unit_config->u16PeriodVal)
        {
            // configs are different, if we are not allowed to just roll with it, error out
            if (!allow_use_incompatible_config)
            {
                // free heap memory
                delete unit_config;
                return ErrorOperationInProgress;
            }

            TIMERA_DEBUG_PRINTF(unit, -2, "pwm_start: semi-compatible config. rolling with it\n");
        }

        // sync startup really shouldn't matter...
    }
    else
    {
        TIMERA_DEBUG_PRINTF(unit, -2, "pwm_start: init now\n");

        // not initialized, initialize now
        // start peripheral clocks for TimerA unit and AOS
        PWC_Fcg2PeriphClockCmd(unit->peripheral.clock_id, Enable);
        PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_AOS, Enable);

        // initialize unit
        TIMERA_BaseInit(unit->peripheral.register_base, unit_config);
        unit->state.base_init = unit_config;
    }

    // ensure timer is started
    return TIMERA_Cmd(unit->peripheral.register_base, Enable);
}

/**
 * @brief stop TimerA unit and all channels
 * @param unit pointer to timera unit config
 */
inline en_result_t timera_pwm_stop_hard(timera_config_t *unit)
{
    CORE_ASSERT(unit != nullptr, "timera_pwm_unit_stop: unit is nullptr", return ErrorInvalidParameter);
    TIMERA_DEBUG_PRINTF(unit, -2, "pwm_stop_hard\n");

    // stop timer unit
    TIMERA_Cmd(unit->peripheral.register_base, Disable);

    // stop channels
    for (uint8_t ch = TimeraCh1; ch < TimeraCh8; ch++)
    {
        timera_pwm_channel_stop(unit, static_cast<en_timera_channel_t>(ch));
    }

    // reset state
    delete unit->state.base_init;
    unit->state.base_init = nullptr;
    unit->state.active_channels = 0;
    return Ok;
}

/**
 * @brief stop timerA unit if all channels are stopped
 * @param unit pointer to timera unit config
 * @return Ok on success, ErrorOperationInProgress if channels are still active
 */
inline en_result_t timera_pwm_stop_if_not_in_use(timera_config_t *unit)
{
    CORE_ASSERT(unit != nullptr, "timera_pwm_unit_stop_if_not_in_use: unit is nullptr", return ErrorInvalidParameter);
    TIMERA_DEBUG_PRINTF(unit, -2, "pwm_stop_if_not_in_use\n");

    // check if any channels are still active
    if (unit->state.active_channels != 0)
    {
        return ErrorOperationInProgress;
    }

    // stop timer unit
    return timera_pwm_stop_hard(unit);
}

/**
 * @brief start a PWM channel
 * @param unit pointer to timera unit config
 * @param channel channel to start
 * @param start_now if true, start channel immediately. if false, channel must be started manually
 */
inline en_result_t timera_pwm_channel_start(timera_config_t *unit, const en_timera_channel_t channel, const bool start_now = true)
{
    CORE_ASSERT(unit != nullptr, "timera_pwm_channel_start: unit is nullptr", return ErrorInvalidParameter);
    TIMERA_DEBUG_PRINTF(unit, channel, "pwm_channel_start: start_now=%d\n", start_now ? 1 : 0);

    // configure channel and enable
    stc_timera_compare_init_t cmp_config = {
        .u16CompareVal = 0,                                  // set later
        .enStartCountOutput = TimeraCountStartOutputLow,     // when not active, output LOW
        .enStopCountOutput = TimeraCountStopOutputLow,       // "
        .enCompareMatchOutput = TimeraCompareMatchOutputLow, // transition LOW on compare match
        .enPeriodMatchOutput = TimeraPeriodMatchOutputHigh,  // transition HIGH on period match
        .enSpecifyOutput = TimeraSpecifyOutputInvalid,       //
    };
    TIMERA_CompareInit(unit->peripheral.register_base, channel, &cmp_config);

    if (start_now)
    {
        TIMERA_CompareCmd(unit->peripheral.register_base, channel, Enable);
    }

    // set active flag
    timera_set_channel_active_flag(unit, channel, true);
    return Ok;
}

/**
 * @brief stop a PWM channel
 * @param unit pointer to timera unit config
 * @param channel channel to stop
 *
 * @note this does not affect any pin function. it only disables the compare function of the channel.
 */
inline en_result_t timera_pwm_channel_stop(timera_config_t *unit, const en_timera_channel_t channel)
{
    CORE_ASSERT(unit != nullptr, "timera_pwm_channel_stop: unit is nullptr", return ErrorInvalidParameter);
    TIMERA_DEBUG_PRINTF(unit, channel, "pwm_channel_stop\n");
    timera_set_channel_active_flag(unit, channel, false);
    return TIMERA_CompareCmd(unit->peripheral.register_base, channel, Disable);
}

/**
 * @brief set PWM on period for a channel
 * @param unit pointer to timera unit config
 * @param channel channel to set period for
 * @param period ON period time in period_unit
 * @param period_unit unit of period. one of TIMERA_PWM_UNIT_US, TIMERA_PWM_UNIT_MS, TIMERA_PWM_UNIT_S
 * @param invert if true, invert the PWM signal
 *
 * @note this function will set the period to the closest possible value. it may be off by a bit.
 */
inline en_result_t timera_pwm_set_period(timera_config_t *unit,
                                         const en_timera_channel_t channel,
                                         const int32_t period,
                                         const uint32_t period_unit = TIMERA_PWM_UNIT_US,
                                         const bool invert = false)
{
    CORE_ASSERT(unit != nullptr, "timera_pwm_set_period: unit is nullptr", return ErrorInvalidParameter);
    CORE_ASSERT(unit->state.base_init != nullptr, "timera_pwm_set_period: unit not initialized", return ErrorInvalidParameter);
    CORE_ASSERT(period >= 0, "timera_pwm_set_period: period must be positive", return ErrorInvalidParameter);

    if (!timera_is_channel_active(unit, channel))
    {
        // channel not active, initialize and enable it
        // start it later manually
        timera_pwm_channel_start(unit, channel, false);
    }

    // get divider from unit config
    const int64_t divider = timera_clk_div_to_n(unit->state.base_init->enClkDiv);

    // get base clock frequency
    const int64_t f_base = timera_get_base_clock() / divider;

    // calculate compare value for cmp_s
    uint32_t cmp_s = TIMERA_PWM_CALC_CMP_FROM_PERIOD(f_base, period, int64_t(period_unit));
    CORE_ASSERT(cmp_s > 0 && cmp_s <= 0xFFFF, "timera_pwm_set_period: calculated compare value out of range", return Error);

    // invert if requested
    const uint16_t PERAR = TIMERA_GetPeriodValue(unit->peripheral.register_base);
    if (invert)
    {
        cmp_s = PERAR - cmp_s;
    }

    // constrain to 0 <= cmp_s <= PERAR
    cmp_s = TIMERA_CONSTRAIN(cmp_s, 0, PERAR);

    // set compare value
    TIMERA_DEBUG_PRINTF(unit, channel, "pwm_set_period: period=%ld, period_unit=%ld, invert=%d, cmp_s=%d, f=%ld\n",
                        period, period_unit, invert ? 1 : 0, int16_t(cmp_s), int32_t(f_base));
    TIMERA_SetCompareValue(unit->peripheral.register_base, channel, static_cast<uint16_t>(cmp_s));

    // ensure channel compare function is enabled
    return TIMERA_CompareCmd(unit->peripheral.register_base, channel, Enable);
}

/**
 * @brief get PWM on period for a channel
 * @param unit pointer to timera unit config
 * @param channel channel to get period for
 * @param period_unit unit of period. one of TIMERA_PWM_UNIT_US, TIMERA_PWM_UNIT_MS, TIMERA_PWM_UNIT_S
 * @param invert if true, invert the PWM signal
 * @return period in period_unit, or -1 on error
 */
inline int32_t timera_pwm_get_period(timera_config_t *unit,
                                     const en_timera_channel_t channel,
                                     const uint32_t period_unit = TIMERA_PWM_UNIT_US,
                                     const bool invert = false)
{
    CORE_ASSERT(unit != nullptr, "timera_pwm_get_period: unit is nullptr", return -1);
    CORE_ASSERT(unit->state.base_init != nullptr, "timera_pwm_get_period: unit not initialized", return -1);
    CORE_ASSERT(timera_is_channel_active(unit, channel), "timera_pwm_get_period: channel not active", return -1);

    // get divider from unit config
    const uint32_t divider = timera_clk_div_to_n(unit->state.base_init->enClkDiv);

    // get base clock frequency
    const int64_t f_base = timera_get_base_clock() / divider;

    // get compare value
    uint16_t cmp_s = TIMERA_GetCompareValue(unit->peripheral.register_base, channel);

    // invert if requested
    if (invert)
    {
        const uint16_t PERAR = TIMERA_GetPeriodValue(unit->peripheral.register_base);
        cmp_s = PERAR - cmp_s;
    }

    // calculate period
    return TIMERA_PWM_CALC_PERIOD_FROM_CMP(f_base, int64_t(cmp_s), int64_t(period_unit));
}

/**
 * @brief set PWM duty cycle for a channel
 * @param unit pointer to timera unit config
 * @param channel channel to set duty for
 * @param duty duty cycle in percent (0-duty_scale)
 * @param duty_scale scale of duty cycle. default is 100 (for percent)
 * @param invert if true, invert the PWM signal
 *
 * @note this function will set the duty to the closest possible value. it may be off by a bit.
 */
inline en_result_t timera_pwm_set_duty(timera_config_t *unit,
                                       const en_timera_channel_t channel,
                                       const uint32_t duty,
                                       const uint32_t duty_scale = 100,
                                       const bool invert = false)
{
    CORE_ASSERT(unit != nullptr, "timera_pwm_set_duty: unit is nullptr", return ErrorInvalidParameter);
    CORE_ASSERT(unit->state.base_init != nullptr, "timera_pwm_set_duty: unit not initialized", return ErrorInvalidParameter);
    CORE_ASSERT(duty >= 0 && duty <= duty_scale, "timera_pwm_set_duty: duty must be between 0 and duty_scale", return ErrorInvalidParameter);

    if (!timera_is_channel_active(unit, channel))
    {
        // channel not active, initialize and enable it
        // start it later manually
        timera_pwm_channel_start(unit, channel, false);
    }

    // since we only care about the duty, we can simplify the calculation by using PERAR and taking a percentage of it
    const uint16_t PERAR = TIMERA_GetPeriodValue(unit->peripheral.register_base);
    uint16_t cmp_s = (PERAR * duty) / duty_scale;

    // invert if requested
    if (invert)
    {
        cmp_s = PERAR - cmp_s;
    }

    // constrain to 0 <= cmp_s <= PERAR
    cmp_s = TIMERA_CONSTRAIN(cmp_s, 0, PERAR);

    // set compare value
    TIMERA_DEBUG_PRINTF(unit, channel, "pwm_set_duty: duty=%ld, duty_scale=%ld, invert=%d, cmp_s=%d, PERAR=%d\n",
                        duty, duty_scale, invert ? 1 : 0, cmp_s, PERAR);
    TIMERA_SetCompareValue(unit->peripheral.register_base, channel, cmp_s);

    // ensure channel compare function is enabled
    return TIMERA_CompareCmd(unit->peripheral.register_base, channel, Enable);
}

/**
 * @brief get PWM duty cycle for a channel
 * @param unit pointer to timera unit config
 * @param channel channel to get duty for
 * @param duty_scale scale of duty cycle. default is 100 (for percent)
 * @param invert if true, invert the PWM signal
 * @return duty cycle (0-duty_scale), or -1 on error
 */
inline int32_t timera_pwm_get_duty(timera_config_t *unit,
                                   const en_timera_channel_t channel,
                                   const uint32_t duty_scale = 100,
                                   const bool invert = false)
{
    CORE_ASSERT(unit != nullptr, "timera_pwm_get_duty: unit is nullptr", return -1);
    CORE_ASSERT(unit->state.base_init != nullptr, "timera_pwm_get_duty: unit not initialized", return -1);
    CORE_ASSERT(timera_is_channel_active(unit, channel), "timera_pwm_get_duty: channel not active", return -1);

    // get current compare values (channel and PERAR)
    const uint16_t PERAR = TIMERA_GetPeriodValue(unit->peripheral.register_base);
    uint16_t cmp_s = TIMERA_GetCompareValue(unit->peripheral.register_base, channel);

    // invert if requested
    if (invert)
    {
        cmp_s = PERAR - cmp_s;
    }

    // calculate duty
    return (cmp_s * duty_scale) / PERAR;
}
