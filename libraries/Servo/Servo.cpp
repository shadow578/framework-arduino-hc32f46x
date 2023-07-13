#include "Servo.h"

//
// attach / detach
//

uint8_t Servo::attach(timera_config_t *timera_unit,
                      const en_timera_channel_t timera_channel,
                      const int32_t min_angle,
                      const int32_t max_angle)
{
    CORE_ASSERT(timera_unit != nullptr, "Servo::attach: timera_unit is nullptr", return INVALID_SERVO);

    // detach if already attached
    if (attached())
    {
        detach();
    }

    // ensure channel not already in use
    CORE_ASSERT(timera_is_channel_active(timera_unit, timera_channel),
                "Servo::attach: channel is already active",
                return INVALID_SERVO);

    // calculate period value for timer
    uint32_t base_clock = timera_get_base_clock();
    uint32_t period_val = (base_clock / 16) / (SERVO_PWM_FREQUENCY /* *2 */); // TODO: ref does *2, check why *2 is needed...
    CORE_ASSERT(period_val > 0 && period_val <= 0xFFFF,
                "Servo::attach: period value calculated is invalid",
                return INVALID_SERVO);

    // prepare unit config
    // (when initializing, a pointer to this is stored in the unit's state. so we need to allocate it on the heap)
    stc_timera_base_init_t *unit_config = new stc_timera_base_init_t;
    unit_config->enClkDiv = TimeraPclkDiv16;                       // PCLK1 / 16
    unit_config->enCntMode = TimeraCountModeTriangularWave;        // triangular wave mode
    unit_config->enCntDir = TimeraCountDirUp;                      // count up
    unit_config->enSyncStartupEn = Disable;                        // no sync startup
    unit_config->u16PeriodVal = static_cast<uint16_t>(period_val); // count up to period_val, then count down to 0 (triangle wave)

    // check if unit is already initialized
    if (timera_is_unit_initialized(timera_unit))
    {
        // already initialized, compare config
        stc_timera_base_init_t *current_config = timera_unit->state.base_init;
#define COMPARE_CONFIG_FIELD(field)                                                               \
    CORE_ASSERT(current_config->field == unit_config->field,                                      \
                "Servo::attach: unit initialized with mismatch config (at " STRINGIFY(field) ")", \
                return INVALID_SERVO);

        COMPARE_CONFIG_FIELD(enClkDiv);
        COMPARE_CONFIG_FIELD(enCntMode);
        COMPARE_CONFIG_FIELD(enCntDir);
        COMPARE_CONFIG_FIELD(enSyncStartupEn);
        COMPARE_CONFIG_FIELD(u16PeriodVal);

        // if we got here, config matches and init can be skipped
        delete[] unit_config;
        unit_config = current_config;
    }
    else
    {
        // not initialized, initialize and start
        TIMERA_BaseInit(timera_unit->peripheral.register_base, unit_config);
        TIMERA_Cmd(timera_unit->peripheral.register_base, Enable);
        timera_unit->state.base_init = unit_config;
    }

    // configure channel and enable
    // TODO: check these settings, eg. why is Cache enabled?
    stc_timera_compare_init_t cmp_config = {
        .u16CompareVal = 0,
        .enStartCountOutput = TimeraCountStartOutputLow,     // when not active, output LOW
        .enStopCountOutput = TimeraCountStopOutputLow,       // "
        .enCompareMatchOutput = TimeraCompareMatchOutputLow, // when cnt == cmp, output LOW
        .enPeriodMatchOutput = TimeraPeriodMatchOutputHigh,  // when cnt == period, output HIGH
        .enSpecifyOutput = TimeraSpecifyOutputInvalid,       // ?
        .enCacheEn = Enable,                                 // ?
        .enTriangularTroughTransEn = Enable,                 // ?
        .enTriangularCrestTransEn = Disable,                 // ?
        .u16CompareCacheVal = unit_config->u16PeriodVal,     // ?
    };
    TIMERA_CompareInit(timera_unit->peripheral.register_base, timera_channel, &cmp_config);
    TIMERA_CompareCmd(timera_unit->peripheral.register_base, timera_channel, Enable);
    timera_set_channel_active_flag(timera_unit, timera_channel, true);

    // TODO: ref enables overflow interrupt, but only to reset the flag. is this necessary?

    // set class members
    this->timer = timera_unit;
    this->channel = timera_channel;
    this->min_angle = min_angle;
    this->max_angle = max_angle;

    // done, return (fake) channel number
    return 1;
}

void Servo::detach()
{
    // ensure attached
    if (!attached())
    {
        return;
    }

    // disable channel
    TIMERA_CompareCmd(timer->peripheral.register_base, channel, Disable);
    timera_set_channel_active_flag(timer, channel, false);

    // set detached
    this->timer = nullptr;
}

//
// period time API
//
void Servo::writeMicroseconds(servo_pulse_width_t pulse_width, bool force)
{
    if (!attached())
    {
        return;
    }

    // clamp value to range, unless force write
    if (!force)
    {
        pulse_width = constrain(pulse_width, SERVO_MIN_PULSE_WIDTH, SERVO_MAX_PULSE_WIDTH);
    }

    // convert to compare value
    uint32_t cmp_value = (pulse_width * SERVO_PWM_FREQUENCY) / 1000000;
    CORE_ASSERT(cmp_value > 0 && cmp_value <= 0xFFFF,
                "Servo::writeMicroseconds: compare value calculated is invalid",
                return);

    // write compare value
    TIMERA_SetCompareValue(timer->peripheral.register_base, channel, cmp_value);
}

servo_pulse_width_t Servo::readMicroseconds()
{
    if (!attached())
    {
        return 0;
    }

    // read current compare and period value
    uint32_t base_clock = TIMERA_GetClockFreq(timer->peripheral.register_base);
    uint32_t period_value = TIMERA_GetPeriodValue(timer->peripheral.register_base);
    uint32_t cmp_value = TIMERA_GetCompareValue(timer->peripheral.register_base, channel);

    // convert to microseconds
}

//
// servo angle API
//
void Servo::write(servo_angle_t angle)
{
    // clamp angle to min/max
    angle = constrain(angle, min_angle, max_angle);

    // convert to microseconds
    // SERVO_MIN_PULSE_WIDTH is 0°, SERVO_MAX_PULSE_WIDTH is 180°, everything in-between is linear
    servo_pulse_width_t us = map(angle, 0, 180, SERVO_MIN_PULSE_WIDTH, SERVO_MAX_PULSE_WIDTH);

    // write microseconds
    writeMicroseconds(us);
}

servo_angle_t Servo::read()
{
    // get microseconds value
    servo_pulse_width_t us = readMicroseconds();

    // convert to angle
    // SERVO_MIN_PULSE_WIDTH is 0°, SERVO_MAX_PULSE_WIDTH is 180°, everything in-between is linear
    return map(us, SERVO_MIN_PULSE_WIDTH, SERVO_MAX_PULSE_WIDTH, 0, 180);
}
