#ifndef TWO_WIRE_H_
#define TWO_WIRE_H_

#warning "Wire library is WIP on HC32F460. Use at your own risk."

#include "Arduino.h"

/**
 * the wire library has a end() function.
 * This macro signals to user code that it is available
 */
#define WIRE_HAS_END 1

/**
 * results of a I2C transmission
 */
enum class TwoWireTransmissionResult : uint8_t
{
  /**
   * transmission was successful
   */
  SUCCESS = 0,

  /**
   * data too long to fit in transmit buffer
   */
  DATA_TOO_LONG = 1,

  /**
   * received NACK on transmit of address
   */
  NACK_ON_ADDRESS = 2,

  /**
   * received NACK on transmit of data
   */
  NACK_ON_DATA = 3,

  /**
   * other error
   */
  OTHER_ERROR = 4,

  /**
   * timeout
   */
  TIMEOUT = 5
};

/**
 * I2C / TWI library for HC32F460
 */
class TwoWire : public Stream
{
public:
  /**
   * @brief initialize without sda/scl pins.
   * @note sda/scl pins must be set using setSDA() and setSCL() before calling begin()
   */
  TwoWire();

  /**
   * @brief initialize with sda/scl pins
   */
  TwoWire(const gpio_pin_t sda, const gpio_pin_t scl);

  /**
   * @brief initialize the Wire library and join a I2C bus as a controller
   */
  void begin();

  /**
   * @brief initialize the Wire library and join a I2C bus as a peripheral
   */
  void begin(uint8_t address);

  /**
   * @brief disable the Wire library and release I2C bus control
   */
  void end();
  void setClock(uint32_t clockFreq);

  /**
   * @brief begin a transmission to the I2C peripheral with the given address
   * @param address the 7-bit I2C address of the peripheral to transmit to
   *
   * @note no bytes are send until endTransmission() is called
   */
  void beginTransmission(uint8_t address);

  /**
   * @brief end a I2C transmission
   * @param stopBit if true, a stop bit is sent after the transmission.
   * 			    if false, the bus is left in a state where a restart can be performed
   * @return the result of the transmission. see TwoWireTransmissionResult
   */
  TwoWireTransmissionResult endTransmission(bool stopBit = true);

  /**
   * @brief request a number of bytes from a I2C peripheral
   * @param address the 7-bit I2C address of the peripheral to request from
   * @param quantity the number of bytes to request
   * @param stopBit if true, a stop bit is sent after the transmission.
   * 			    if false, the bus is left in a state where a restart can be performed
   * @return the number of bytes received
   */
  uint8_t requestFrom(uint8_t address, size_t quantity, bool stopBit = true);

  /**
   * @brief write a single byte to the I2C bus
   * @return the number of bytes written
   */
  size_t write(uint8_t data);

  /**
   * @brief write a number of bytes to the I2C bus
   * @param data the data to write
   * @param len the number of bytes to write
   * @return the number of bytes written
   */
  size_t write(const uint8_t *data, size_t len);

  using Print::write;

  /**
   * @brief the number of bytes available to read
   * @return the number of bytes available to read
   */
  virtual int available(void);

  /**
   * @brief read a single byte from the I2C bus
   * @return the byte read
   */
  virtual int read(void);

  /**
   * @brief get the next byte from the I2C bus without removing it from the buffer
   * @return the next byte from the I2C bus
   */
  virtual int peek(void);

  /**
   * @brief flush the receive buffer
   */
  virtual void flush(void);

  /**
   * @brief set the SDA pin
   * @note must be called before begin()
   */
  void setSDA(const gpio_pin_t sda)
  {
    this->sda_pin = sda;
  }

  /**
   * @brief set the SCL pin
   * @note must be called before begin()
   */
  void setSCL(const gpio_pin_t scl)
  {
    this->scl_pin = scl;
  }

private:
  /**
   * sda pin
   */
  gpio_pin_t sda_pin;

  /**
   * scl pin
   */
  gpio_pin_t scl_pin;

  /**
   * receive buffer
   */
  RingBuffer<uint8_t> *rxBuffer;

  /**
   * transmit buffer
   */
  RingBuffer<uint8_t> *txBuffer;
};

#endif /* TWO_WIRE_H_ */
