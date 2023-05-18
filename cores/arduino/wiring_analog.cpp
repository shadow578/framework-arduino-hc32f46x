#include "wiring_analog.h"
#include "drivers/gpio/gpio.h" // includes drivers/adc/adc.h already

void analogReference(eAnalogReference ulMode)
{
    // stub only for compatibility with existing AVR based API
}

uint32_t analogRead(uint32_t ulPin)
{
    if (ulPin >= BOARD_NR_GPIO_PINS)
    {
        return 0;
    }

    // get adc info from pin map
    adc_dev *adc_dev = PIN_MAP[ulPin].adc_device;
    uint8_t adc_channel = PIN_MAP[ulPin].adc_channel;
    if (adc_dev == NULL || adc_channel == ADC_PIN_INVALID)
    {
        return 0;
    }

    // read from adc
    return adc_read(adc_dev, adc_channel);
}
