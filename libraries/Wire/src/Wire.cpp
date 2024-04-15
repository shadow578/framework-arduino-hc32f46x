#include "Wire.h"
#include "drivers/gpio/gpio.h"

/**
 * i2c unit to number 1-3
 */
#define I2C_UNIT_TO_N(unit) ((unit) == M4_I2C1 ? 1    \
      : (unit) == M4_I2C2 ? 2                         \
      : (unit) == M4_I2C3 ? 3                         \
      : -1)

/**
 * debug print for TwoWire class
 */
#define TWO_WIRE_DEBUG_PRINTF(fmt, ...)                                              \
  CORE_DEBUG_PRINTF("[TWI%d] " fmt, I2C_UNIT_TO_N(this->peripheral), ##__VA_ARGS__)

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
      return I2C_TIMEOUT;
    }

    // send one byte of data
    I2C_WriteData(periph, data[sent]);
    sent++;

    // wait for transfer to complete
    if (I2C_WaitStatus(periph, I2C_SR_TENDF, Set, timeout) != Ok)
    {
      return I2C_TIMEOUT;
    }

    // check for NACK flag
    if (I2C_GetStatus(periph, I2C_SR_NACKF) == Set)
    {
      I2C_ClearStatus(periph, I2C_CLR_NACKFCLR);
      return I2C_NACK_ON_DATA;
    }
  }

  return I2C_SUCCESS;
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
      return I2C_TIMEOUT;
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
        return I2C_TIMEOUT;
      }
    }
  }

  I2C_AckConfig(perih, I2c_ACK);
  return I2C_SUCCESS;
}

//
// TwoWire class implementation
//
TwoWire::TwoWire(M4_I2C_TypeDef *peripheral, const gpio_pin_t sda, const gpio_pin_t scl)
{
  this->peripheral = peripheral;
  this->sda_pin = sda;
  this->scl_pin = scl;
}

void TwoWire::begin()
{
  // ensure device & pins are configured
  CORE_ASSERT(this->peripheral != nullptr, "device not set");
  ASSERT_GPIO_PIN_VALID(this->sda_pin, "TwoWire::begin");
  ASSERT_GPIO_PIN_VALID(this->scl_pin, "TwoWire::begin");

  // de-init peripheral if already initialized
  end();

  // get clock and pin functions
  uint32_t peripheral_clock;
  en_port_func_t sda_func, scl_func;
  switch (I2C_UNIT_TO_N(this->peripheral))
  {
    case 1:
      // I2C1
      peripheral_clock = PWC_FCG1_PERIPH_I2C1;
      sda_func = Func_I2c1_Sda;
      scl_func = Func_I2c1_Scl;
      break;
    case 2:
      // I2C2
      peripheral_clock = PWC_FCG1_PERIPH_I2C2;
      sda_func = Func_I2c2_Sda;
      scl_func = Func_I2c2_Scl;
      break;
    case 3:
      // I2C3
      peripheral_clock = PWC_FCG1_PERIPH_I2C3;
      sda_func = Func_I2c3_Sda;
      scl_func = Func_I2c3_Scl;
      break;
    default:
      CORE_ASSERT_FAIL("invalid I2C peripheral");
      return;
  }

  // enable peripheral clock
  PWC_Fcg1PeriphClockCmd(peripheral_clock, Enable);

  // configure pins
  GPIO_SetFunc(this->sda_pin, sda_func);
  GPIO_SetFunc(this->scl_pin, scl_func);

  // configure peripheral
  // TODO: i2c configuration is hard-coded here
  stc_i2c_init_t init = {.u32ClockDiv = I2C_CLK_DIV1, .u32Baudrate = 100000, .u32SclTime = 0};
  float32_t baud_error;
  CORE_ASSERT(I2C_Init(this->peripheral, &init, &baud_error) != Ok, "I2C_Init failed");
  TWO_WIRE_DEBUG_PRINTF("init with err=%d%%\n", (int)(baud_error * 100));

  // enable bus wait
  I2C_BusWaitCmd(this->peripheral, Enable);
}

void TwoWire::end()
{
  I2C_DeInit(this->peripheral);
  TWO_WIRE_DEBUG_PRINTF("deinit\n");
}

void TwoWire::beginTransmission(uint8_t address)
{
  this->tx_address = address;
  this->tx_buffer_len = 0;
}

TwoWireStatus TwoWire::endTransmission(bool stopBit)
{
  TWO_WIRE_DEBUG_PRINTF("sending %d bytes to 0x%02X (stop=%d)\n", this->tx_buffer_len, this->tx_address, stopBit);

  // enable peripheral
  I2C_Cmd(this->peripheral, Enable);

  I2C_SoftwareResetCmd(this->peripheral, Enable);
  I2C_SoftwareResetCmd(this->peripheral, Disable);

  // send start condition
  en_result_t rc = I2C_Start(this->peripheral, WIRE_TIMEOUT);
  if (rc == ErrorTimeout)
  {
    TWO_WIRE_DEBUG_PRINTF("timeout on I2C_Start\n");
    return I2C_TIMEOUT;
  }

  // send address
  rc = I2C_TransAddr(this->peripheral, this->tx_address, I2CDirTrans, WIRE_TIMEOUT);
  if (rc == ErrorTimeout)
  {
    TWO_WIRE_DEBUG_PRINTF("timeout on I2C_TransAddr\n");
    return I2C_TIMEOUT;
  }
  else if (rc == Error)
  {
    return I2C_NACK_ON_ADDRESS;
  }

  // send data
  TwoWireStatus tx_result =
      I2C_TransferDataW(this->peripheral, this->tx_buffer, this->tx_buffer_len, WIRE_TIMEOUT);

  // send stop condition and disable peripheral
  if (stopBit)
  {
    I2C_Stop(this->peripheral, WIRE_TIMEOUT);
    I2C_Cmd(this->peripheral, Disable);
  }

  return tx_result;
}

TwoWireStatus TwoWire::_requestFrom(uint8_t address, size_t quantity, bool stopBit)
{
  CORE_ASSERT(quantity <= WIRE_BUFFER_LENGTH, "quantity larger than RX buffer size", return I2C_OTHER_ERROR);

  TWO_WIRE_DEBUG_PRINTF("requesting %d bytes from 0x%02X (stop=%d)\n", quantity, address, stopBit);

  // reset rx buffer
  this->rx_buffer_index = 0;
  this->rx_buffer_len = 0;

  // enable peripheral
  I2C_Cmd(this->peripheral, Enable);

  I2C_SoftwareResetCmd(this->peripheral, Enable);
  I2C_SoftwareResetCmd(this->peripheral, Disable);

  // send start condition
  en_result_t rc = I2C_Start(this->peripheral, WIRE_TIMEOUT);
  if (rc == ErrorTimeout)
  {
    TWO_WIRE_DEBUG_PRINTF("timeout on I2C_Start\n");
    return I2C_TIMEOUT;
  }

  // ???
  if (quantity == 1)
  {
    I2C_AckConfig(this->peripheral, I2c_NACK);
  }

  // send address
  rc = I2C_TransAddr(this->peripheral, address, I2CDirReceive, WIRE_TIMEOUT);
  if (rc == ErrorTimeout)
  {
    TWO_WIRE_DEBUG_PRINTF("timeout on I2C_TransAddr\n");
    return I2C_TIMEOUT;
  }
  else if (rc == Error)
  {
    return I2C_NACK_ON_ADDRESS;
  }

  // receive data
  TwoWireStatus rx_result = I2C_ReceiveDataW(this->peripheral, this->rx_buffer, quantity,
                                             this->rx_buffer_len, WIRE_TIMEOUT, stopBit);

  // send stop condition and disable peripheral
  if (stopBit)
  {
    I2C_Stop(this->peripheral, WIRE_TIMEOUT);
    I2C_Cmd(this->peripheral, Disable);
  }

  return rx_result;
}