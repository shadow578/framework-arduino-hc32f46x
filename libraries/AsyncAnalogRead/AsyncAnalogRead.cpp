#include "AsyncAnalogRead.h"
#include <drivers/adc/adc.h>

inline bool get_adc_info(uint32_t pin, adc_device_t *&adc_device, uint8_t &adc_channel)
{
    if (pin >= BOARD_NR_GPIO_PINS)
    {
        return false;
    }

    // get adc info from pin map
    adc_device = PIN_MAP[pin].adc_device;
    adc_channel = PIN_MAP[pin].adc_channel;
    if (adc_device == NULL || adc_channel == ADC_PIN_INVALID)
    {
        return false;
    }

    return true;
}

void analogReadAsync(uint32_t ulPin)
{
    // get analog pin info
    adc_device_t *adc_device;
    uint8_t adc_channel;
    if (!get_adc_info(ulPin, adc_device, adc_channel))
    {
        return;
    }

    // start conversion
    adc_start_conversion(adc_device);
}

bool getAnalogReadComplete(uint32_t ulPin)
{
    // get analog pin info
    adc_device_t *adc_device;
    uint8_t adc_channel;
    if (!get_adc_info(ulPin, adc_device, adc_channel))
    {
        return false;
    }

    // check if conversion is complete
    return adc_is_conversion_completed(adc_device);
}

uint16_t getAnalogReadValue(uint32_t ulPin)
{
    // get analog pin info
    adc_device_t *adc_device;
    uint8_t adc_channel;
    if (!get_adc_info(ulPin, adc_device, adc_channel))
    {
        return 0;
    }

    // read conversion result
    return adc_conversion_read_result(adc_device, adc_channel);
}
