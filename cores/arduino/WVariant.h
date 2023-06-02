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
	 * @brief variant pin map struct
	 */
	typedef struct pin_info_t
	{
		/**
		 * @brief bit position of the pin in the port
		 */
		uint8_t bit_pos;

		/**
		 * @brief IO port this pin belongs to
		 */
		en_port_t port;

		/**
		 * @brief bit mask of the pin in the port
		 */
		en_pin_t bit_mask;

		/**
		 * @brief pointer to the ADC device of this pin, if any
		 * @note NULL if not a ADC pin
		 */
		adc_device_t *adc_device;

		/**
		 * @brief adc channel number of this pin, if any
		 * @note ADC_PIN_INVALID if not a ADC pin
		 */
		uint8_t adc_channel;
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
