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
#include "HardwareSerial.h"
#include "RingBuffer.h"
#include "usart_config.h"
#include "../../core_types.h"

class Usart : public HardwareSerial
{
public:
  /**
   * @brief construct a new Usart object
   * @param config pointer to the usart configuration struct
   * @param tx_pin gpio pin number for tx function
   * @param rx_pin gpio pin number for rx function
   */
  Usart(struct usart_config_t *config, gpio_pin_t tx_pin, gpio_pin_t rx_pin);
  void begin(uint32_t baud);
  void begin(uint32_t baud, uint16_t config);
  void begin(uint32_t baud, const stc_usart_uart_init_t *config);
  void end();
  int available();
  int availableForWrite();
  int peek();
  int read();
  void flush();
  size_t write(uint8_t ch);
  using Print::write; // pull in write(str) and write(buf, size) from Print
  operator bool() { return true; }

  /**
   * @brief access the base usart config struct
   */
  const usart_config_t *c_dev(void) { return this->config; }

  /**
   * @brief get the last receive error
   * @note calling this function clears the error
   */
  const usart_receive_error_t getReceiveError(void);

private:
  // usart configuration struct
  usart_config_t *config;

  // tx / rx pin numbers
  gpio_pin_t tx_pin;
  gpio_pin_t rx_pin;

  // rx / tx buffers (unboxed from config)
  RingBuffer<uint8_t> *rxBuffer;
  RingBuffer<uint8_t> *txBuffer;

  // is initialized? (begin() called)
  bool initialized = false;
};

//
// global instances
//
#ifndef DISABLE_SERIAL_GLOBALS
extern Usart Serial1;
extern Usart Serial2;
extern Usart Serial3;

#define Serial Serial1
#endif
