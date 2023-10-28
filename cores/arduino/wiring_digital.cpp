#include "wiring_digital.h"
#include "drivers/gpio/gpio.h"
#include "drivers/adc/adc.h"
#include "drivers/timera/timera_pwm.h"
#include "wiring_constants.h"
#include "core_debug.h"

/**
 * @brief set a pin to PWM mode
 */
static void pin_set_pwm_mode(gpio_pin_t dwPin)
{
    // get timer assignment for pin
    timera_config_t *unit;
    en_timera_channel_t channel;
    en_port_func_t port_function;
    if (!timera_get_assignment(dwPin, unit, channel, port_function))
    {
        CORE_ASSERT_FAIL("analogWrite: pin is not a PWM pin");
        return;
    }

    // initialize timer unit, allow incompatible config
    switch (timera_pwm_start(unit, 1000 /* Hz */, 16, true))
    {
    case Ok:
        // all good
        break;
    case ErrorOperationInProgress:
        // already initialized with incompatible config
        CORE_ASSERT_FAIL("timera_pwm_start failed: ErrorOperationInProgress");
        return;
    case ErrorInvalidParameter:
    default:
        // invalid parameter or other error
        CORE_ASSERT_FAIL("timera_pwm_start failed");
        return;
    }

    // initialize channel, start now
    timera_pwm_channel_start(unit, channel, true);

    // set pin function to TimerA output, no GPIO
    GPIO_SetFunc(dwPin, port_function, Disable);
}

void pinMode(gpio_pin_t dwPin, uint32_t dwMode)
{
    ASSERT_GPIO_PIN_VALID(dwPin, "pinMode", return);

    // if pin has ADC channel, configure ADC according to pin mode
    pin_adc_info_t adc_info = PIN_MAP[dwPin].adc_info;
    if (adc_info.has_adc())
    {
        adc_device_t *adc_device = adc_info.get_device();
        uint8_t adc_channel = adc_info.channel;

        if (dwMode == INPUT_ANALOG)
        {
            // initialize adc device (if already initialized, this will do nothing)
            adc_device_init(adc_device);

            // enable ADC channel
            adc_enable_channel(adc_device, adc_channel);
        }
        else
        {
            // disable ADC channel
            adc_disable_channel(adc_device, adc_channel);
        }
    }

    // build pin configuration
    stc_port_init_t pinConf;
    switch (dwMode)
    {
    case INPUT:
        pinConf.enPinMode = Pin_Mode_In;
        break;
    case INPUT_PULLUP:
        pinConf.enPinMode = Pin_Mode_In;
        pinConf.enPullUp = Enable;
        break;
    case INPUT_ANALOG:
        pinConf.enPinMode = Pin_Mode_Ana;
        break;
    case OUTPUT_PWM:
        pin_set_pwm_mode(dwPin);

        // return immediately, as pin_set_pwm_mode sets the pin function
        return;
    case OUTPUT:
        pinConf.enPinMode = Pin_Mode_Out;
        break;
    default:
        CORE_ASSERT_FAIL("pinMode: invalid pin mode. Must be INPUT, INPUT_PULLUP, INPUT_ANALOG, OUTPUT or OUTPUT_PWM");
        return;
    }

    // set pin function and config
    GPIO_SetFunc(dwPin, Func_Gpio, Enable);
    GPIO_Init(dwPin, &pinConf);
}

uint32_t getPinMode(gpio_pin_t dwPin)
{
    ASSERT_GPIO_PIN_VALID(dwPin, "getPinMode");
    if (dwPin >= BOARD_NR_GPIO_PINS)
    {
        return INPUT_FLOATING;
    }

    // read pin configuration
    stc_port_init_t pinConf;
    CORE_ASSERT(GPIO_GetConfig(dwPin, &pinConf) == Ok, "getPinMode: GPIO_GetConfig failed");

    // read pin function
    en_port_func_t pinFunction;
    en_functional_state_t pinSubFunction;
    CORE_ASSERT(GPIO_GetFunc(dwPin, &pinFunction, &pinSubFunction) == Ok, "getPinMode: GPIO_GetFunc failed");

    // determine mode from function and configuration
    switch (pinFunction)
    {
    case Func_Gpio:
        // GPIO function, determine mode from configuration
        switch (pinConf.enPinMode)
        {
        case Pin_Mode_Out:
            return OUTPUT;
        case Pin_Mode_In:
            return (pinConf.enPullUp == Enable) ? INPUT_PULLUP : INPUT;
        case Pin_Mode_Ana:
            return INPUT_ANALOG;
        default:
            CORE_ASSERT_FAIL("getPinMode: invalid configuration for Func_Gpio");
            return INPUT_FLOATING;
        }

    case Func_Tima0:
    case Func_Tima1:
    case Func_Tima2:
        // TimerA output function, must be PWM
        // in that case, subFunction is always disabled
        CORE_ASSERT(pinSubFunction == Disable, "getPinMode: pinSubFunctin is Enabled for Func_TimaX");
        return OUTPUT_PWM;

    default:
        CORE_ASSERT_FAIL("getPinMode: invalid pin function");
        return INPUT_FLOATING;
    }
}

void digitalWrite(gpio_pin_t dwPin, uint32_t dwVal)
{
    ASSERT_GPIO_PIN_VALID(dwPin, "digitalWrite");
    if (dwPin >= BOARD_NR_GPIO_PINS)
    {
        return;
    }

    if (dwVal == HIGH)
    {
        GPIO_SetBits(dwPin);
    }
    else
    {
        GPIO_ResetBits(dwPin);
    }
}

int digitalRead(gpio_pin_t ulPin)
{
    ASSERT_GPIO_PIN_VALID(ulPin, "digitalRead");
    if (ulPin >= BOARD_NR_GPIO_PINS)
    {
        return LOW;
    }

    return GPIO_GetBit(ulPin) ? HIGH : LOW;
}
