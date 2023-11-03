#include "wiring_analog.h"
#include "drivers/gpio/gpio.h" // includes drivers/adc/adc.h already
#include "drivers/timera/timera_pwm.h"
#include "core_debug.h"

//
// analogRead
//

void analogReference(eAnalogReference ulMode)
{
    // stub only for compatibility with existing AVR based API
}

uint32_t analogRead(gpio_pin_t ulPin)
{
    ASSERT_GPIO_PIN_VALID(ulPin, "analogRead");

    if (ulPin >= BOARD_NR_GPIO_PINS)
    {
        return 0;
    }

    // get adc info from pin map
    pin_adc_info_t adc_info = PIN_MAP[ulPin].adc_info;
    adc_device_t *adc_device = adc_info.get_device();
    uint8_t adc_channel = adc_info.channel;
    if (adc_device == NULL || adc_channel == ADC_PIN_INVALID)
    {
        CORE_ASSERT_FAIL("analogRead: pin is not an ADC pin")
        return 0;
    }

    // read from adc channel synchronously
    return adc_read_sync(adc_device, adc_channel);
}

void analogReadResolution(int res)
{
    switch (res)
    {
    case 8:
        ADC1_device.init_params.resolution = AdcResolution_8Bit;
        break;
    case 10:
        ADC1_device.init_params.resolution = AdcResolution_10Bit;
        break;
    case 12:
        ADC1_device.init_params.resolution = AdcResolution_12Bit;
        break;
    default:
        CORE_ASSERT_FAIL("analogReadResolution: resolution must be 8, 10 or 12")

        // fallback to 10 bit
        ADC1_device.init_params.resolution = AdcResolution_10Bit;
        break;
    }
}

//
// analogWrite
//
uint32_t analogWriteScale = (1 << 8);

void analogWriteResolution(uint8_t res)
{
    // NOTE: 4-16 bits is not a technical limitation, but a practical one.
    // for 4-16 bits resolution, the PWM calculations should work ok...
    CORE_ASSERT(res >= 4 && res <= 16, "analogWriteResolution: resolution must be between 4 and 16");
    analogWriteScale = 1 << res;
}

void analogWrite(gpio_pin_t ulPin, uint32_t ulValue)
{
    analogWriteScaled(ulPin, ulValue, analogWriteScale);
}

void analogWriteScaled(gpio_pin_t ulPin, uint32_t ulValue, uint32_t ulScale)
{
    // get timer assignment for pin
    timera_config_t *unit;
    en_timera_channel_t channel;
    en_port_func_t port_function;
    if (!timera_get_assignment(ulPin, unit, channel, port_function))
    {
        CORE_ASSERT_FAIL("analogWrite: pin is not a PWM pin");
        return;
    }

    // ensure timer and channel are started
    CORE_ASSERT(timera_is_unit_initialized(unit), "analogWrite: timer unit is not initialized", return);
    CORE_ASSERT(timera_is_channel_active(unit, channel), "analogWrite: timer channel is not active", return);

    // set duty with requested scale
    timera_pwm_set_duty(unit, channel, ulValue, ulScale);
}

bool isAnalogWritePin(gpio_pin_t ulPin)
{
    // if the pin has a timerA assignment, it is a PWM pin
    timera_config_t *unit;
    en_timera_channel_t channel;
    en_port_func_t port_function;
    return timera_get_assignment(ulPin, unit, channel, port_function);
}