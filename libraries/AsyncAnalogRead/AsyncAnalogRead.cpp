#include "AsyncAnalogRead.h"
#include <drivers/adc/adc.h>
#include "core_debug.h"

inline bool get_adc_info(gpio_pin_t pin, adc_device_t *&adc_device, uint8_t &adc_channel)
{
    ASSERT_GPIO_PIN_VALID(pin, "get_adc_info");
    if (pin >= BOARD_NR_GPIO_PINS)
    {
        return false;
    }

    // get adc info from pin map
    adc_device = PIN_MAP[pin].adc_info.get_device();
    adc_channel = PIN_MAP[pin].adc_info.channel;
    if (adc_device == NULL || adc_channel == ADC_PIN_INVALID)
    {
        return false;
    }

    return true;
}

void analogReadAsync(gpio_pin_t ulPin)
{
    ASSERT_GPIO_PIN_VALID(ulPin, "analogReadAsync");

    // get analog pin info
    adc_device_t *adc_device;
    uint8_t adc_channel;
    if (!get_adc_info(ulPin, adc_device, adc_channel))
    {
        CORE_ASSERT_FAIL("analogReadAsync: not an analog pin");
        return;
    }

    // start conversion
    adc_start_conversion(adc_device);
}

bool getAnalogReadComplete(gpio_pin_t ulPin)
{
    ASSERT_GPIO_PIN_VALID(ulPin, "getAnalogReadComplete");

    // get analog pin info
    adc_device_t *adc_device;
    uint8_t adc_channel;
    if (!get_adc_info(ulPin, adc_device, adc_channel))
    {
        CORE_ASSERT_FAIL("getAnalogReadComplete: not an analog pin");
        return false;
    }

    // check if conversion is complete
    return adc_is_conversion_completed(adc_device);
}

uint16_t getAnalogReadValue(gpio_pin_t ulPin)
{
    ASSERT_GPIO_PIN_VALID(ulPin, "getAnalogReadValue");

    // get analog pin info
    adc_device_t *adc_device;
    uint8_t adc_channel;
    if (!get_adc_info(ulPin, adc_device, adc_channel))
    {
        CORE_ASSERT_FAIL("getAnalogReadValue: not an analog pin");
        return 0;
    }

    // read conversion result
    return adc_conversion_read_result(adc_device, adc_channel);
}
