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

#ifndef _WIRING_INTERRUPTS_
#define _WIRING_INTERRUPTS_

#include "core_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

//      LOW 0
//      HIGH 1
#define CHANGE 2
#define FALLING 3
#define RISING 4

#define DEFAULT 1
#define EXTERNAL 0


  /*
   * \brief Specifies a named Interrupt Service Routine (ISR) to call when an interrupt occurs.
   *        Detaches any previously attached interrupt on the same pin.
   *
   * \param pin The pin number to attach the interrupt to
   * \param callback The function to call when the interrupt occurs
   * \param mode Defines when the interrupt should be triggered.
   * \return assigned interrupt number, or -1 if there are no available interrupts
   *
   * \note you must call checkIRQFlag() in your ISR to clear the interrupt flag.
   *      Otherwise, your ISR may be called continuously or never again.
   */
  int attachInterrupt(gpio_pin_t pin, voidFuncPtr callback, uint32_t mode);

  /*
   * \brief Turns off the given interrupt.
   */
  void detachInterrupt(gpio_pin_t pin);

  /*
   * \brief Checks if the interrupt flag is set for the given pin
   *
   * \param pin The pin to check
   * \param clear If true, clear the flag if it is set
   * \return True if the flag is set, false otherwise
   */
  bool checkIRQFlag(gpio_pin_t pin, bool clear = true);

#ifdef __cplusplus
}
#endif

#endif
