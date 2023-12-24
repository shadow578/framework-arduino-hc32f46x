#ifndef TWO_WIRE_H_
#define TWO_WIRE_H_

#warning "Wire library is WIP on HC32F460. Use at your own risk."

#if DDL_I2C_ENABLE != DDL_ON
  #error "Wire library requires I2C to be enabled"
#endif

#include "Arduino.h"
#include "hc32_ddl.h"
#include "i2c_config.h"

/**
 * the wire library has a end() function.
 * This macro signals to user code that it is available
 */
#define WIRE_HAS_END 1

#ifndef WIRE_BUFFER_LENGTH
  #define WIRE_BUFFER_LENGTH 32
#endif

#define WIRE_TIMEOUT 1000

/**
 * results of a I2C transmission
 */
enum class TwoWireStatus : uint8_t
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
   * @param device the I2C device config
   * @note sda/scl pins must be set using setSDA() and setSCL() before calling begin()
   */
  TwoWire(i2c_device_config_t *device)
  {
    TwoWire(device, GPIO_PIN_INVALID, GPIO_PIN_INVALID);
  }

  /**
   * @brief initialize with sda/scl pins
   * @param device the I2C device config
   * @param sda the sda pin
   * @param scl the scl pin
   */
  TwoWire(i2c_device_config_t *device, const gpio_pin_t sda, const gpio_pin_t scl);

  /**
   * @brief initialize the Wire library and join a I2C bus as a controller
   */
  void begin();

  /**
   * @brief initialize the Wire library and join a I2C bus as a peripheral
   */
  // TODO: void begin(uint8_t address);

  /**
   * @brief disable the Wire library and release I2C bus control
   * @note SDA and SCL pins are NOT reverted to GPIO function
   */
  void end();

  /**
   * @brief set the clock frequency for the I2C bus
   */
  // TODO: void setClock(uint32_t clockFreq);

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
  TwoWireStatus endTransmission(bool stopBit = true);

  /**
   * @brief request a number of bytes from a I2C peripheral
   * @param address the 7-bit I2C address of the peripheral to request from
   * @param quantity the number of bytes to request
   * @param stopBit if true, a stop bit is sent after the transmission.
   * 			    if false, the bus is left in a state where a restart can be performed
   * @return the result of the transmission. see TwoWireTransmissionResult
   *
   * @note the number of bytes received can be obtained using available()
   */
  TwoWireStatus requestFromInt(uint8_t address, size_t quantity, bool stopBit = true);

  /**
   * @brief request a number of bytes from a I2C peripheral
   * @param address the 7-bit I2C address of the peripheral to request from
   * @param quantity the number of bytes to request
   * @param stopBit if true, a stop bit is sent after the transmission.
   * 			    if false, the bus is left in a state where a restart can be performed
   * @return the number of bytes received
   */
  uint8_t requestFrom(uint8_t address, size_t quantity, bool stopBit = true)
  {
    requestFromInt(address, quantity, stopBit);
    return available();
  }

  /**
   * @brief write a single byte to the I2C bus
   * @return the number of bytes written
   */
  size_t write(uint8_t data)
  {
    // is there space in the tx buffer?
    if (this->tx_buffer_len >= WIRE_BUFFER_LENGTH)
    {
      return 0;
    }

    // add data to tx buffer
    this->tx_buffer[this->tx_buffer_len++] = data;
    return 1;
  }

  /**
   * @brief write a number of bytes to the I2C bus
   * @param data the data to write
   * @param len the number of bytes to write
   * @return the number of bytes written
   */
  size_t write(const uint8_t *data, size_t len)
  {
    size_t written = 0;
    for (size_t i = 0; i < len; i++)
    {
      if (this->write(data[i]) == 0)
      {
        return written;
      }

      written++;
    }

    return written;
  }

  using Print::write;

  /**
   * @brief the number of bytes available to read
   * @return the number of bytes available to read
   */
  virtual int available(void)
  {
    return this->rx_buffer_len;
  }

  /**
   * @brief read a single byte from the I2C bus
   * @return the byte read
   */
  virtual int read(void)
  {
    int res = this->peek();
    if (res != -1)
    {
      this->rx_buffer_index++;
    }

    return res;
  }

  /**
   * @brief get the next byte from the I2C bus without removing it from the buffer
   * @return the next byte from the I2C bus
   */
  virtual int peek(void)
  {
    if (this->rx_buffer_index >= this->rx_buffer_len)
    {
      return -1;
    }

    return this->rx_buffer[this->rx_buffer_index];
  }

  /**
   * @brief flush the receive buffer
   */
  virtual void flush(void)
  {
    this->rx_buffer_len = 0;
    this->rx_buffer_index = 0;
  }

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
   * i2c device
   */
  i2c_device_config_t *device = NULL;

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
  uint8_t rx_buffer[WIRE_BUFFER_LENGTH];

  /**
   * amount of data in the receive buffer
   */
  size_t rx_buffer_len = 0;

  /**
   * index of the next byte to read from the receive buffer
   */
  size_t rx_buffer_index = 0;

  /**
   * transmit buffer
   */
  uint8_t tx_buffer[WIRE_BUFFER_LENGTH];

  /**
   * amount of data in the transmit buffer
   */
  size_t tx_buffer_len = 0;

  /**
   * the address of the peripheral to transmit to
   */
  uint8_t tx_address;
};

#endif /* TWO_WIRE_H_ */
