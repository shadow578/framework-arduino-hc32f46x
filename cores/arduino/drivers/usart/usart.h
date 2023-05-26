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
#include "drivers/usart/usart.h"
#include "HardwareSerial.h"
#include "RingBuffer.h"
#include "usart_config.h"

class Usart : public HardwareSerial
{
public:
  Usart(struct usart_config_t *config);
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

  // rx / tx buffers (unboxed from config)
  RingBuffer *rxBuffer;
  RingBuffer *txBuffer;
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
