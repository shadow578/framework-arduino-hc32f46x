#include "Servo.h"
#include "drivers/timera/timera_pwm.h"

//
// attach / detach
//

Servo::Servo() {}

uint8_t Servo::attach(const gpio_pin_t gpio_pin, const int32_t min_angle, const int32_t max_angle)
{
    ASSERT_GPIO_PIN_VALID(gpio_pin, "Servo::attach", return INVALID_SERVO);

    // get assignments
    timera_config_t *timer_unit;
    en_timera_channel_t timer_channel;
    en_port_func_t port_func;
    if (!timera_get_assignment(gpio_pin, timer_unit, timer_channel, port_func))
    {
        CORE_ASSERT_FAIL("Servo::attach pin has no TimerA assignment");
        return INVALID_SERVO;
    }

    // attach
    uint8_t result = attach(timer_unit, timer_channel, min_angle, max_angle);

    // set GPIO function if attached
    if (result != INVALID_SERVO)
    {
        GPIO_SetFunc(gpio_pin, port_func, Disable);
        this->pin = gpio_pin;
    }

    return result;
}

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

    // configure and start timer for PWM
    // TODO: allow setting allow_use_incompatible_config here?
    // -> servo should not care *too much* about frequency, as long as the period is correct
    if (timera_pwm_start(timera_unit, SERVO_PWM_FREQUENCY, SERVO_TIMER_DIVIDER, false) != Ok)
    {
        CORE_ASSERT_FAIL("Servo::attach: timera_pwm_start failed");
        return INVALID_SERVO;
    }

    // initialize channel for PWM, but do not start it yet
    timera_pwm_channel_start(timera_unit, timera_channel, false);

    // set class members
    this->timer = timera_unit;
    this->channel = timera_channel;
    this->min_angle = min_angle;
    this->max_angle = max_angle;

    // set angle to 0° (this will start the output)
    write(SERVO_INITIAL_ANGLE);

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

    // stop the PWM channel
    timera_pwm_channel_stop(timer, channel);

    // stop the PWM timer if no longer in use
    timera_pwm_stop_if_not_in_use(timer);

    // reset GPIO function
    if (pin != INVALID_SERVO)
    {
        GPIO_SetFunc(pin, Func_Gpio, Enable);
    }

    // set detached
    this->timer = nullptr;
    pin = INVALID_SERVO;
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

    // set pulse width
    timera_pwm_set_period(timer, channel, pulse_width, TIMERA_PWM_UNIT_US);
}

servo_pulse_width_t Servo::readMicroseconds()
{
    if (!attached())
    {
        return 0;
    }

    // get pulse width
    return timera_pwm_get_period(timer, channel, TIMERA_PWM_UNIT_US);
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
