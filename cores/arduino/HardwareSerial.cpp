#include "HardwareSerial.h"
#include "drivers/gpio/gpio.h"
#include "drivers/usart/usart.h"

HardwareSerial::HardwareSerial(struct usart_dev *usart_device,
                               uint32_t tx_pin,
                               uint32_t rx_pin)
{
    this->usart_device = usart_device;
    this->tx_pin = tx_pin;
    this->rx_pin = rx_pin;
}

void HardwareSerial::begin(uint32_t baud)
{
    // default to 8 bits, no parity, 1 stop bit
    begin(baud, SERIAL_8N1);
}

void HardwareSerial::begin(uint16_t baud, uint16_t config)
{
    // set IO pin functions
    PORT_SetFuncGPIO(this->tx_pin, Disable);
    PORT_SetFuncGPIO(this->rx_pin, Disable);

    // #region setup usart:
    usart_init(this->usart_device);

    // baud
    usart_set_baud_rate(this->usart_device, baud);

    // stop bits
    switch (config & HARDSER_STOP_BIT_MASK)
    {
    default:
    case HARDSER_STOP_BIT_1:
        usart_set_stop_bits(this->usart_device, UsartOneStopBit);
        break;
    case HARDSER_STOP_BIT_2:
        usart_set_stop_bits(this->usart_device, UsartTwoStopBit);
        break;
    }

    // parity
    switch (config & HARDSER_PARITY_MASK)
    {
    default:
    case HARDSER_PARITY_NONE:
        usart_set_parity(this->usart_device, UsartParityNone);
        break;
    case HARDSER_PARITY_EVEN:
        usart_set_parity(this->usart_device, UsartParityEven);
        break;
    case HARDSER_PARITY_ODD:
        usart_set_parity(this->usart_device, UsartParityOdd);
        break;
    }

    // data bits
    switch (config & HARDSER_DATA_MASK)
    {
    default:
    case HARDSER_DATA_8:
        usart_set_data_width(this->usart_device, UsartDataBits8);
        break;
    }
    // #endregion

    // enable usart
    usart_enable(this->usart_device);
}

void HardwareSerial::end()
{
    usart_disable(this->usart_device);
}

int HardwareSerial::available(void)
{
    return usart_data_available(this->usart_device);
}

int HardwareSerial::peek(void)
{
    return usart_peek(this->usart_device);
}

int HardwareSerial::read(void)
{
    if (usart_data_available(this->usart_device) > 0)
    {
        return usart_getc(this->usart_device);
    }
    else
    {
        return -1;
    }
}

void HardwareSerial::flush(void)
{
    while (!this->usart_device->wb->isEmpty())
        ;
}

size_t HardwareSerial::write(uint8_t ch)
{
    usart_putc(this->usart_device, ch);
    return 1;
}
