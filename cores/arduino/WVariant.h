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

#ifndef WVARIANT_H_
#define WVARIANT_H_

#include <stdint.h>
#include <stdbool.h>
#include <hc32_ddl.h>
#include <addon_gpio.h>
#include "drivers/adc/adc.h"

// Include board variant
#include <variant.h>

#ifdef __cplusplus
extern "C"
{
#endif
	/**
	 * @brief adc info for a pin
	 */
	typedef struct pin_adc_info_t
	{
		/**
		 * @brief pointer to the ADC device of this pin, if any
		 * @note
		 * - 0 if not set / not a ADC pin
		 * - 1 = ADC1
		 * - 2 = ADC2
		 */
		const uint8_t device : 2;

		/**
		 * @brief adc channel number of this pin, if any
		 * @note only valid if device != 0
		 */
		const uint8_t channel : 6;

#ifdef __cplusplus
		/**
		 * @brief get the pointer to the ADC device of this pin
		 * @return pointer to the ADC device of this pin, or NULL if no adc device is assigned
		 */
		adc_device_t *get_device() const
		{
			switch (device)
			{
			case 1:
				return &ADC1_device;
			case 2:
				// TODO: ADC2 is not yet supported
				// return &ADC2_device;
			default:
				return NULL;
			}
		}
#endif
	} pin_adc_info_t;

	/**
	 * @brief TimerA info for a pin
	 */
	typedef struct pin_timera_info_t
	{
		/**
		 * @brief TimerA unit assigned to this pin for PWM or Servo output
		 *
		 * @note
		 * - 0: no timer assigned
		 * - 1: M4_TMRA1 / TIMERA1_config
		 * - 2: M4_TMRA2 / TIMERA2_config
		 * - 3: M4_TMRA3 / TIMERA3_config
		 * - 4: M4_TMRA4 / TIMERA4_config
		 * - 5: M4_TMRA5 / TIMERA5_config
		 * - 6: M4_TMRA6 / TIMERA6_config
		 */
		const uint8_t unit : 3;

		/**
		 * @brief TimerA channel assigned to this pin for PWM or Servo output
		 *
		 * @note
		 * - 0: TimeraCh1
		 * - 1: TimeraCh2
		 * - 2: TimeraCh3
		 * - 3: TimeraCh4
		 * - 4: TimeraCh5
		 * - 5: TimeraCh6
		 * - 6: TimeraCh7
		 * - 7: TimeraCh8
		 *
		 * @note only valid if unit != 0
		 */
		const uint8_t channel : 3;

		/**
		 * @brief PORT primary function to enable TimerA output for this pin
		 *
		 * @note
		 * - 0: Func_Tima0
		 * - 1: Func_Tima1
		 * - 2: Func_Tima2
		 *
		 * @note only valid if unit != 0
		 */
		const uint8_t function : 2;
	} pin_timera_info_t;

	/**
	 * @brief variant pin map struct
	 */
	typedef struct pin_info_t
	{
		/**
		 * @brief bit position of the pin in the port
		 */
		const uint8_t bit_pos : 5;

		/**
		 * @brief IO port this pin belongs to
		 */
		const en_port_t port : 3; 

#ifdef __cplusplus
		/**
		 * @brief bit mask of the pin in the port
		 */
		en_pin_t bit_mask() const
		{
			return (en_pin_t)(1 << bit_pos);
		}
#endif

		/**
		 * @brief adc configuration for this pin
		 */
		const pin_adc_info_t adc_info;

		/**
		 * @brief TimerA configuration for this pin
		 */
		const pin_timera_info_t timera_info;

	} pin_info_t;

	/**
	 * @brief GPIO pin map
	 */
	extern const pin_info_t PIN_MAP[BOARD_NR_GPIO_PINS];

/**
 * @brief test if a gpio pin number is valid
 */
#define IS_GPIO_PIN(pin) (pin >= 0 && pin < BOARD_NR_GPIO_PINS)

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* WVARIANT_H_ */
