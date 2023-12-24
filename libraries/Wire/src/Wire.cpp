#include "Wire.h"
#include "core_debug.h"
#include "drivers/gpio/gpio.h"

/**
 * @brief clone of I2C_TransData, modified to return TwoWireStatus instead of en_result_t
 * @param periph I2C peripheral
 * @param data data to send
 * @param size size of data
 * @param timeout timeout in ms
 * @return result of transmission
 */
TwoWireStatus I2C_TransferDataW(M4_I2C_TypeDef *periph, const uint8_t data[], size_t size, uint32_t timeout)
{
  volatile size_t sent = 0ul;
  while ((sent < size))
  {
    // wait for TX buffer to be empty
    if (I2C_WaitStatus(periph, I2C_SR_TEMPTYF, Set, timeout) != Ok)
    {
      return TwoWireStatus::TIMEOUT;
    }

    // send one byte of data
    I2C_WriteData(periph, data[sent]);
    sent++;

    // wait for transfer to complete
    if (I2C_WaitStatus(periph, I2C_SR_TENDF, Set, timeout) != Ok)
    {
      return TwoWireStatus::TIMEOUT;
    }

    // check for NACK flag
    if (I2C_GetStatus(periph, I2C_SR_NACKF) == Set)
    {
      I2C_ClearStatus(periph, I2C_CLR_NACKFCLR);
      return TwoWireStatus::NACK_ON_DATA;
    }
  }

  return TwoWireStatus::SUCCESS;
}

/**
 * @brief clone of I2C_TransAddr, modified to return TwoWireStatus instead of en_result_t
 */
TwoWireStatus I2C_ReceiveDataW(M4_I2C_TypeDef *perih, uint8_t data[], size_t capacity, size_t &received,
                               uint32_t timeout, bool stop)
{
  bool fastAck = perih->CR3_f.FACKEN == 0;
  for (received = 0; received < capacity; received++)
  {
    // wait for RX buffer to be full
    if (I2C_WaitStatus(perih, I2C_SR_RFULLF, Set, timeout) != Ok)
    {
      return TwoWireStatus::TIMEOUT;
    }

    // ??
    if (fastAck)
    {
      if ((capacity >= 2) && (received == (capacity - 2)))
      {
        I2C_AckConfig(perih, I2c_NACK);
      }
    }
    else
    {
      if (received != (capacity - 1))
      {
        I2C_AckConfig(perih, I2c_ACK);
      }
      else
      {
        I2C_AckConfig(perih, I2c_NACK);
      }
    }

    // stop before reading last data byte
    if (received == (capacity - 1) && stop)
    {
      I2C_ClearStatus(perih, I2C_CLR_STOPFCLR);
      I2C_GenerateStop(perih, Enable);
    }

    // read one byte of data
    data[received] = I2C_ReadData(perih);

    // wait for stop flag after read
    if (received == (capacity - 1))
    {
      if (I2C_WaitStatus(perih, I2C_SR_STOPF, Set, timeout) != Ok)
      {
        return TwoWireStatus::TIMEOUT;
      }
    }
  }

  I2C_AckConfig(perih, I2c_ACK);
}

//
// TwoWire class implementation
//
TwoWire::TwoWire(i2c_device_config_t *device, const gpio_pin_t sda, const gpio_pin_t scl)
{
  this->device = device;
  this->sda_pin = sda;
  this->scl_pin = scl;
}

void TwoWire::begin()
{
  // ensure device & pins are configured
  CORE_ASSERT(this->device != NULL, "device not configured");
  ASSERT_GPIO_PIN_VALID(this->sda_pin, "TwoWire::begin");
  ASSERT_GPIO_PIN_VALID(this->scl_pin, "TwoWire::begin");

  // de-init peripheral if already initialized
  end();

  // enable peripheral clock
  PWC_Fcg1PeriphClockCmd(this->device->peripheral.clock_id, Enable);

  // configure pins
  GPIO_SetFunc(this->sda_pin, this->device->peripheral.sda_func);
  GPIO_SetFunc(this->scl_pin, this->device->peripheral.scl_func);

  // configure peripheral
  // TODO: i2c configuration is hard-coded here
  stc_i2c_init_t init = {.u32ClockDiv = I2C_CLK_DIV2, .u32Baudrate = 100000, .u32SclTime = 0};
  float32_t baud_error;
  CORE_ASSERT(I2C_Init(this->device->peripheral.register_base, &init, &baud_error) == Ok, "I2C_Init failed");
  CORE_DEBUG_PRINTF("i2c init with err=%d%%\n", (int)baud_error * 100);

  // enable bus wait
  I2C_BusWaitCmd(this->device->peripheral.register_base, Enable);
}

void TwoWire::end()
{
  I2C_DeInit(this->device->peripheral.register_base);
}

void TwoWire::beginTransmission(uint8_t address)
{
  this->tx_address = address;
  this->tx_buffer_len = 0;
}

TwoWireStatus TwoWire::endTransmission(bool stopBit)
{
  // enable peripheral
  I2C_Cmd(this->device->peripheral.register_base, Enable);

  I2C_SoftwareResetCmd(this->device->peripheral.register_base, Enable);
  I2C_SoftwareResetCmd(this->device->peripheral.register_base, Disable);

  // send start condition
  en_result_t rc = I2C_Start(this->device->peripheral.register_base, WIRE_TIMEOUT);
  if (rc == ErrorTimeout)
  {
    return TwoWireStatus::TIMEOUT;
  }

  // send address
  rc = I2C_TransAddr(this->device->peripheral.register_base, this->tx_address, I2CDirTrans, WIRE_TIMEOUT);
  if (rc = ErrorTimeout)
  {
    return TwoWireStatus::TIMEOUT;
  }
  else if (rc == Error)
  {
    return TwoWireStatus::NACK_ON_ADDRESS;
  }

  // send data
  TwoWireStatus tx_result =
      I2C_TransferDataW(this->device->peripheral.register_base, this->tx_buffer, this->tx_buffer_len, WIRE_TIMEOUT);

  // send stop condition and disable peripheral
  if (stopBit)
  {
    I2C_Stop(this->device->peripheral.register_base, WIRE_TIMEOUT);
    I2C_Cmd(this->device->peripheral.register_base, Disable);
  }

  return tx_result;
}

TwoWireStatus TwoWire::requestFromInt(uint8_t address, size_t quantity, bool stopBit)
{
  CORE_ASSERT(quantity <= WIRE_BUFFER_LENGTH, "quantity larger than RX buffer size");

  // reset rx buffer
  this->rx_buffer_index = 0;
  this->rx_buffer_len = quantity;

  // enable peripheral
  I2C_Cmd(this->device->peripheral.register_base, Enable);

  I2C_SoftwareResetCmd(this->device->peripheral.register_base, Enable);
  I2C_SoftwareResetCmd(this->device->peripheral.register_base, Disable);

  // send start condition
  en_result_t rc = I2C_Start(this->device->peripheral.register_base, WIRE_TIMEOUT);
  if (rc == ErrorTimeout)
  {
    return TwoWireStatus::TIMEOUT;
  }

  // ???
  if (quantity == 1)
  {
    I2C_AckConfig(this->device->peripheral.register_base, I2c_NACK);
  }

  // send address
  rc = I2C_TransAddr(this->device->peripheral.register_base, address, I2CDirReceive, WIRE_TIMEOUT);
  if (rc = ErrorTimeout)
  {
    return TwoWireStatus::TIMEOUT;
  }
  else if (rc == Error)
  {
    return TwoWireStatus::NACK_ON_ADDRESS;
  }

  // receive data
  TwoWireStatus rx_result = I2C_ReceiveDataW(this->device->peripheral.register_base, this->rx_buffer, quantity,
                                             this->rx_buffer_len, WIRE_TIMEOUT, stopBit);

  // send stop condition and disable peripheral
  if (stopBit)
  {
    I2C_Stop(this->device->peripheral.register_base, WIRE_TIMEOUT);
    I2C_Cmd(this->device->peripheral.register_base, Disable);
  }

  return rx_result;
}