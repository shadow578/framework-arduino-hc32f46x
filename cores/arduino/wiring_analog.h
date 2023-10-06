/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#pragma once

#include <stdint.h>
#include "core_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

  typedef enum _eAnalogReference
  {
    AR_DEFAULT,
    AR_INTERNAL,
  } eAnalogReference;

  /*
   * \brief Configures the reference voltage used for analog input (i.e. the value used as the top of the input range).
   * This function is kept only for compatibility with existing AVR based API.
   *
   * \param ulMmode Should be set to AR_DEFAULT.
   * \note when called, this function will do nothing and return immediately.
   */
  extern void analogReference(eAnalogReference ulMode);

  /*
   * \brief Writes an analog value (PWM wave) to a pin.
   *
   * \param ulPin
   * \param ulValue
   *
   * \note the pin must be configured as OUTPUT_PWM beforehand.
   */
  extern void analogWrite(gpio_pin_t ulPin, uint32_t ulValue);

  /**
   * \brief Writes an analog value (PWM wave) to a pin, with the given scale
   *
   * \param ulPin
   * \param ulValue
   * \param ulScale
   *
   * \note the pin must be configured as OUTPUT_PWM beforehand.
   */
  extern void analogWriteScaled(gpio_pin_t ulPin, uint32_t ulValue, uint32_t ulScale);

  /*
   * \brief Reads the value from the specified analog pin.
   *
   * \param ulPin
   *
   * \return Read value from selected pin, if no error.
   * \note the pin must be configured as INPUT_ANALOG beforehand.
   * \note not all pins on the chip can be used for analog input. see the datasheet for details.
   */
  extern uint32_t analogRead(gpio_pin_t ulPin);

  /*
   * \brief Set the resolution of analogRead return values. Default is 10 bits (range from 0 to 1023).
   *
   * \param res adc resolution. one of 8, 10, 12 bits
   * \note
   * resolution must be set before setting a pin to INPUT_ANALOG mode using pinMode().
   * changing the resolution afterwards will not have any effect.
   */
  extern void analogReadResolution(int res);

  /*
   * \brief Set the resolution of analogWrite parameters. Default is 8 bits (range from 0 to 255).
   *
   * \param res resolution in bits (4-16)
   */
  extern void analogWriteResolution(uint8_t res);

  /**
   * \brief check if a pin can be used for analogWrite (PWM output)
   *
   * \param ulPin
   *
   * \return true if pin can be used for analogWrite, false otherwise
   */
  extern bool isAnalogWritePin(gpio_pin_t ulPin);

#ifdef __cplusplus
}
#endif
