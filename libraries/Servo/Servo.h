#ifndef __SERVO_H__
#define __SERVO_H__

#include <Arduino.h>
#include "drivers/gpio/gpio.h"
#include "drivers/timera/timera_util.h"

/**
 * servo frequency in Hz
 */
#define SERVO_PWM_FREQUENCY 50

/**
 * timer divider for servo frequency
 */
#define SERVO_TIMER_DIVIDER 16

/**
 * shortest pulse sent to servo (== 0 degrees), in microseconds
 */
#define SERVO_MIN_PULSE_WIDTH 544

/**
 * longest pulse sent to servo (== 180 degrees), in microseconds
 */
#define SERVO_MAX_PULSE_WIDTH 2400

/**
 * flag indicating invalid servo index
 */
#define INVALID_SERVO 255

typedef int32_t servo_angle_t;
typedef uint32_t servo_pulse_width_t;

/**
 * @brief Servo control class using TimerA
 */
class Servo
{
public:
    Servo();

    /**
     * @brief attach the given pin to the appropriate channel and set pin mode
     * @param gpio_pin gpio pin number
     * @param min_angle minimum angle in degrees
     * @param max_angle maximum angle in degrees
     * @returns servo channel number or INVALID_SERVO if failure
     *
     * @note this will disable the pin's GPIO function
     *
     * @note ensure that the TimerA unit assigned to the pin is not already in use. if it is, this function will fail
     *
     * @note if the pin is already attached to a servo, this function will detach the pin first
     */
    inline uint8_t attach(const gpio_pin_t gpio_pin, const int32_t min_angle = 0, const int32_t max_angle = 180)
    {
        ASSERT_GPIO_PIN_VALID(gpio_pin, "Servo::attach", return INVALID_SERVO);

        // get assignments
        timera_config_t *timer_unit;
        en_timera_channel_t timera_channel;
        en_port_func_t port_func;
        if (!timera_get_assignment(gpio_pin, timer_unit, timera_channel, port_func))
        {
            CORE_ASSERT_FAIL("Servo::attach pin has no TimerA assignment");
            return INVALID_SERVO;
        }

        // attach
        uint8_t result = attach(timer_unit, timera_channel, min_angle, max_angle);

        // set GPIO function if attached
        if (result != INVALID_SERVO)
        {
            GPIO_SetFunc(gpio_pin, port_func);
        }

        return result;
    }

    /**
     * @brief attach to the given TimerA channel
     *
     * @note do not use this function unless you know what you're doing
     *
     * @note refer to "normal" attach() for notes
     */
    uint8_t attach(timera_config_t *timera_unit, const en_timera_channel_t timera_channel, const int32_t min_angle, const int32_t max_angle);

    /**
     * @brief detach the servo
     *
     * @note this will disable the TimerA channel, but will not re-enable the pin's GPIO function
     */
    void detach();

    /**
     * @brief check if the servo is currently attached
     * @returns true if attached, false otherwise
     */
    inline bool attached()
    {
        return (timer != nullptr);
    }

    /**
     * @brief set the servo pulse width
     * @param pulse_width pulse width in microseconds
     * @param force if true, ignore pulse width limits
     * @returns true if success, false otherwise
     *
     * @note this function limits the value to the range [SERVO_MIN_PULSE_WIDTH, SERVO_MAX_PULSE_WIDTH] unless force is true
     */
    void writeMicroseconds(servo_pulse_width_t pulse_width, bool force = false);

    /**
     * @brief get the servo pulse width
     * @returns pulse width in microseconds
     */
    servo_pulse_width_t readMicroseconds();

    /**
     * @brief set the servo angle
     * @param angle angle in degrees
     * @returns true if success, false otherwise
     */
    void write(servo_angle_t angle);

    /**
     * @brief get the servo angle
     * @returns angle in degrees
     */
    servo_angle_t read();

private:
    /**
     * @brief TimerA unit config reference
     * @note this is nullprt if the servo is not attached
     */
    timera_config_t *timer = nullptr;

    /**
     * @brief TimerA channel number
     */
    en_timera_channel_t channel;

    /**
     * @brief minimum angle in degrees
     */
    servo_angle_t min_angle;

    /**
     * @brief maximum angle in degrees
     */
    servo_angle_t max_angle;
};

#endif // __SERVO_H__
