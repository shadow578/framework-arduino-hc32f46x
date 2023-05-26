#include <hc32_ddl.h>
#include "Usart.h"
#include "core_hooks.h"
#include "yield.h"
#include "../gpio/gpio.h"

Usart::Usart(struct usart_config_t *config)
{
    this->config = config;

    // unpack rx and tx buffers from usart config
    this->rxBuffer = config->state.rx_buffer;
    this->txBuffer = config->state.tx_buffer;
}

void Usart::begin(uint32_t baud)
{
    // default to 8 bits, no parity, 1 stop bit
    begin(baud, SERIAL_8N1);
}

void Usart::begin(uint32_t baud, uint16_t config)
{
    // create full configuration from input config
    stc_usart_uart_init_t usartConfig = {
        .enClkMode = UsartIntClkCkNoOutput,
        .enClkDiv = UsartClkDiv_1,
        .enDataLength = UsartDataBits8,
        .enDirection = UsartDataLsbFirst,
        .enStopBit = UsartOneStopBit,
        .enParity = UsartParityNone,
        .enSampleMode = UsartSampleBit8,
        .enDetectMode = UsartStartBitFallEdge,
        .enHwFlow = UsartRtsEnable,
    };

    // stop bits
    switch (config & HARDSER_STOP_BIT_MASK)
    {
    default:
    case HARDSER_STOP_BIT_1:
        usartConfig.enStopBit = UsartOneStopBit;
        break;
    case HARDSER_STOP_BIT_2:
        usartConfig.enStopBit = UsartTwoStopBit;
        break;
    }

    // parity
    switch (config & HARDSER_PARITY_MASK)
    {
    default:
    case HARDSER_PARITY_NONE:
        usartConfig.enParity = UsartParityNone;
        break;
    case HARDSER_PARITY_EVEN:
        usartConfig.enParity = UsartParityEven;
        break;
    case HARDSER_PARITY_ODD:
        usartConfig.enParity = UsartParityOdd;
        break;
    }

    // data bits
    switch (config & HARDSER_DATA_MASK)
    {
    default:
    case HARDSER_DATA_8:
        usartConfig.enDataLength = UsartDataBits8;
        break;
    }

    // call begin with full config
    begin(baud, &usartConfig);
}

void Usart::begin(uint32_t baud, const stc_usart_uart_init_t *config)
{
    // clear rx and tx buffers
    this->rxBuffer->clear();
    this->txBuffer->clear();

    // set IO pin functions
    PORT_SetFuncGPIO(this->config->pins.rx_pin, Disable);
    PORT_SetFuncGPIO(this->config->pins.tx_pin, Disable);

    // enable peripheral clock
    PWC_Fcg1PeriphClockCmd(this->config->peripheral.clock_id, Enable);

    // initialize usart peripheral and set baud rate
    USART_UART_Init(this->config->peripheral.register_base, config);
    USART_SetBaudrate(this->config->peripheral.register_base, baud);

    // setup usart interrupts:
#define EN_IRQ(irq)                                               \
    {                                                             \
        stc_irq_regi_conf_t irqConf = {                           \
            .enIntSrc = irq.interrupt_source,                     \
            .enIRQn = irq.interrupt_number,                       \
            .pfnCallback = irq.interrupt_handler,                 \
        };                                                        \
        enIrqRegistration(&irqConf);                              \
        NVIC_SetPriority(irqConf.enIRQn, irq.interrupt_priority); \
        NVIC_ClearPendingIRQ(irqConf.enIRQn);                     \
        NVIC_EnableIRQ(irqConf.enIRQn);                           \
    }

    // RX data available
    EN_IRQ(this->config->interrupts.rx_data_available);

    // RX error
    EN_IRQ(this->config->interrupts.rx_error);

    // TX buffer empty
    EN_IRQ(this->config->interrupts.tx_buffer_empty);

    // TX Complete
    EN_IRQ(this->config->interrupts.tx_complete);

    // enable usart RX + interrupts
    // (tx is enabled on-demand when data is available to send)
    USART_FuncCmd(this->config->peripheral.register_base, UsartRx, Enable);
    USART_FuncCmd(this->config->peripheral.register_base, UsartRxInt, Enable);
}

void Usart::end()
{
    // wait for tx buffer to empty
    flush();

    // deinit uart
    USART_DeInit(this->config->peripheral.register_base);

    // clear rx and tx buffers
    this->rxBuffer->clear();
    this->txBuffer->clear();
}

int Usart::available(void)
{
    return this->rxBuffer->count();
}

int Usart::availableForWrite(void)
{
    return this->txBuffer->capacity() - this->txBuffer->count();
}

int Usart::peek(void)
{
    return this->rxBuffer->peek();
}

int Usart::read(void)
{
    uint8_t ch;
    if (this->rxBuffer->pop(ch))
    {
        return ch;
    }
    else
    {
        return -1;
    }
}

void Usart::flush(void)
{
    while (!this->txBuffer->isEmpty())
        ;
}

size_t Usart::write(uint8_t ch)
{
    uint32_t errors = 0;

    // wait until tx buffer is empty
    while (!this->txBuffer->isEmpty())
    {
        if (++errors > 500)
            return 1;
        yield();
    }

    // add to tx buffer until it succeeds
    while (!this->txBuffer->push(ch))
    {
        if (++errors > 500)
            return 1;
        yield();
    }

    // enable tx + empty interrupt
    USART_FuncCmd(this->config->peripheral.register_base, UsartTxAndTxEmptyInt, Enable);

    // wrote one byte
    return 1;
}
