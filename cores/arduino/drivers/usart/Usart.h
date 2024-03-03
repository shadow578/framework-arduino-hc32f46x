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

#ifndef SERIAL_BUFFER_SIZE
#define SERIAL_BUFFER_SIZE 64
#endif
#ifndef SERIAL_TX_BUFFER_SIZE
#define SERIAL_TX_BUFFER_SIZE SERIAL_BUFFER_SIZE
#endif
#ifndef SERIAL_RX_BUFFER_SIZE
#define SERIAL_RX_BUFFER_SIZE SERIAL_BUFFER_SIZE
#endif

class Usart : public HardwareSerial
{
public:
  /**
   * @brief construct a new Usart object
   * @param config pointer to the usart configuration struct
   * @param tx_pin gpio pin number for tx function
   * @param rx_pin gpio pin number for rx function
   * @param rx_buffer_size size of the rx buffer
   * @param tx_buffer_size size of the tx buffer
   */
  Usart(struct usart_config_t *config, 
        gpio_pin_t tx_pin, 
        gpio_pin_t rx_pin, 
        size_t rx_buffer_size = SERIAL_RX_BUFFER_SIZE, 
        size_t tx_buffer_size = SERIAL_TX_BUFFER_SIZE);

  ~Usart();

  void begin(uint32_t baud);
  void begin(uint32_t baud, uint16_t config);
  void begin(uint32_t baud, const stc_usart_uart_init_t *config, const bool rxNoiseFilter = true);
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

  #ifdef USART_RX_ERROR_COUNTERS_ENABLE
  /**
   * @brief get the number of framing errors
   * @note usart_receive_error_t::FramingError
   */
  inline uint32_t getFramingErrorCount(void) { return this->config->state.rx_error_counters.framing_error; }

  /**
   * @brief get the number of parity errors
   * @note usart_receive_error_t::ParityError
   */
  inline uint32_t getParityErrorCount(void) { return this->config->state.rx_error_counters.parity_error; }

  /**
   * @brief get the number of overrun errors
   * @note usart_receive_error_t::OverrunError
   */
  inline uint32_t getOverrunErrorCount(void) { return this->config->state.rx_error_counters.overrun_error; }

  /**
   * @brief get how often bytes were dropped from the RX buffer
   * @note usart_receive_error_t::RxDataDropped
   */
  inline uint32_t getDroppedDataErrorCount(void) { return this->config->state.rx_error_counters.rx_data_dropped; }
  #endif // USART_RX_ERROR_COUNTERS_ENABLE

  #ifdef USART_RX_DMA_SUPPORT
public:
  /**
   * @brief enable RX DMA for this Usart
   * @param dma pointer to the DMA peripheral
   * @param channel DMA channel to use for RX
   * @note must be called before begin()
   */
  void enableRxDma(M4_DMA_TypeDef *dma, en_dma_channel_t channel);

  /**
   * @brief disable RX DMA for this Usart
   * @note must be called before begin()
   * @note if begin() was already called, this function MUST be called before end()
   */
  void disableRxDma();
private:
  void rx_dma_init();
  void rx_dma_deinit();
  #endif // USART_RX_DMA_SUPPORT

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
