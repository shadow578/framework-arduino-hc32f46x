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
   * \return assigned interrupt number, or -1 if the interrupt couldn't be assigned
   *
   * \note
   * the external interrupt priority is set to DDL_IRQ_PRIORITY_DEFAULT by default.
   * use setInterruptPriority() to change it.
   *
   * \note 
   * you must call checkIRQFlag() in your ISR to clear the interrupt flag.
   * Otherwise, your ISR may be called continuously or never again.
   *
   * \note 
   * if a interrupt is already attached to the given pin, this function will detach it first.
   *
   * \note
   * any pin may be used for external interrupts, with the following limitations:
   * - the pin should not be assigned to another function
   * - at most 16 external interrupts can be used at the same time
   * - not any combination of pins can be used. Each pin on the HC32F46x connects to a EXTI line,
   *   and only one pin per EXTI line can be used for external interrupts at a time.
   *   The EXTI line number is equal to the bit position of the pin in the port register (~= the 1 in PA1).
   *   Thus, PA0, PB0, PC0, ... share a EXTI line, and only one of them can be used for external interrupts at a time.
   * (this info may be incorrect. the datasheet is chinese and i'm not 100% sure i understood it correctly)
   */
  int attachInterrupt(gpio_pin_t pin, voidFuncPtr callback, uint32_t mode);

  /*
   * \brief Turns off the given interrupt.
   *
   * \note if no interrupt is attached to the given pin, this function does nothing.
   */
  void detachInterrupt(gpio_pin_t pin);

  /*
   * \brief Checks if the interrupt flag is set for the given pin
   *
   * \param pin The pin to check
   * \param clear If true, clear the flag if it is set
   * \return True if the flag is set, false otherwise
   *
   * \note
   * under the hood, this will only check the interrupt flag for the pin's EXTI line.
   * this does not necessarily mean that the interrupt was caused by the pin.
   * (e.g. if the interrupt was caused by PB0, calling this function with PA0 will return true too)
   */
  #ifdef __cplusplus
  bool checkIRQFlag(gpio_pin_t pin, bool clear = true);
  #else
  bool checkIRQFlag(gpio_pin_t pin, bool clear);
  #endif

  /*
   * \brief set the priority of the given external interrupt
   *
   * \param pin The pin to set the priority for
   * \param priority The priority to set. one of DDL_IRQ_PRIORITY_xx
   */
  void setInterruptPriority(gpio_pin_t pin, uint32_t priority);

#ifdef __cplusplus
}
#endif

#endif
