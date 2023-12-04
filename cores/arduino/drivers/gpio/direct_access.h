#pragma once

/**
 * direct port manipulation helpers for HC32F460.
 * see examples for usage and more info
 *
 * general information on the GPIO registers can be found in the HC32F460 user manual, chapter 9.4
 */
#include <hc32_ddl.h>
#include "../../WVariant.h"

/**
 * @brief convert arduino pin number to port number
 * @param pin arduino pin number, e.g. PB0
 * @return GPIO port number
 *
 * @note GPIO port numbers match the enum values of en_port_t
 */
#define digitalPinToPort(pin) \
    ((uint8_t)PIN_MAP[pin].port) // (uint8_t)en_port_t

/**
 * @brief convert arduino pin number to bit mask for that pin in the port registers
 * @param pin arduino pin number, e.g. PB0
 * @return bit mask for that pin in the port registers
 */
#define digitalPinToBitMask(pin) \
    ((uint16_t)(1 << PIN_MAP[pin].bit_pos))

/**
 * @brief offset a GPIO register by the port number. assumes 16 byte offset between port registers
 * @param base_register base address of GPIO register. e.g. M4_PORT->POERA
 * @param port port number to offset by. both uint8_t and en_port_t are valid inputs
 * @return pointer to the GPIO register for the given port
 *
 * @note (&base_register + (0x10 * port)), result is cast to uint16_t*
 *
 * @note internally used by portModeRegister, portOutputRegister and portInputRegister
 */
#define __PORT_OFFSET_REGISTER(base_register, port) \
    ((uint16_t *)((uint32_t)(&base_register) + (0x10 * (uint32_t)(port))))

/**
 * @brief get the mode register for a given port
 * @param port port number to offset by. both the output of digitalPinToPort and en_port_t are valid inputs
 * @return pointer to the mode register for the given port
 *
 * @note the register controls the output function of the pin. 0 = input, 1 = output
 *
 * @note
 * to ensure proper function, the pin must *not* have been set to any other function than GPIO.
 * if not sure, use pinMode(pin, INPUT) or pinMode(pin, OUTPUT) to set the pin to GPIO mode first.
 * (default mode after reset is GPIO)
 *
 * @note
 * special modes, like
 * - open drain
 * - pullups / pulldowns
 * - analog input
 * - peripheral functions (e.g. UART TX)
 * - ...
 * are not affected by this register.
 * using them in combination with this register may cause unexpected results or even damage the chip.
 *
 * @note
 * really, just use pinMode(pin, INPUT) or pinMode(pin, OUTPUT) to set the pin to GPIO mode first.
 */
#define portModeRegister(port) \
    __PORT_OFFSET_REGISTER(M4_PORT->POERA, port)

/**
 * @brief get the output register for a given port
 * @param port port number to offset by. both the output of digitalPinToPort and en_port_t are valid inputs
 * @return pointer to the output register for the given port
 *
 * @note the register controls the output value of the pin. 0 = low, 1 = high
 * @note usage is only valid if the pin is set to output mode first
 */
#define portOutputRegister(port) \
    __PORT_OFFSET_REGISTER(M4_PORT->PODRA, port)

/**
 * @brief get the input register for a given port
 * @param port port number to offset by. both the output of digitalPinToPort and en_port_t are valid inputs
 * @return pointer to the input register for the given port
 *
 * @note the register allows reading the input value of the pin. 0 = low, 1 = high
 * @note usage is valid for both input and output mode
 */
#define portInputRegister(port) \
    __PORT_OFFSET_REGISTER(M4_PORT->PIDRA, port)
