#include "wiring_analog.h"
#include "drivers/gpio/gpio.h" // includes drivers/adc/adc.h already
#include "core_debug.h"

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
    adc_device_t *adc_device = adc_info.device;
    uint8_t adc_channel = adc_info.channel;
    if (adc_device == NULL || adc_channel == ADC_PIN_INVALID)
    {
        CORE_ASSERT_FAIL("analogRead: pin is not an ADC pin")
        return 0;
    }

    // read from adc channel synchronously
    return adc_read_sync(adc_device, adc_channel);
}
