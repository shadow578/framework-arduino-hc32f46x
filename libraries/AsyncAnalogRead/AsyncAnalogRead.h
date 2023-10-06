#pragma once
#include "Arduino.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief start asynchronous analog read from the specified analog pin.
     *
     * @param ulPin analog pin to read from
     * @note the pin must be configured as INPUT_ANALOG beforehand.
     * @note not all pins on the chip can be used for analog input. see the datasheet for details.
     */
    void analogReadAsync(gpio_pin_t ulPin);

    /**
     * @brief check if the last asynchronous analog read is complete
     * @param ulPin analog pin to read from
     * @return true if conversion is complete
     */
    bool getAnalogReadComplete(gpio_pin_t ulPin);

    /**
     * @brief get the last asynchronous analog read value
     * @param ulPin analog pin to read from
     * @return the last read value
     */
    uint16_t getAnalogReadValue(gpio_pin_t ulPin);

#ifdef __cplusplus
}
#endif
