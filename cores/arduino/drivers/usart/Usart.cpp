#include <hc32_ddl.h>
#include "Usart.h"
#include "core_hooks.h"
#include "core_debug.h"
#include "yield.h"
#include "../gpio/gpio.h"
#include "../irqn/irqn.h"

//
// global instances
//
#ifndef DISABLE_SERIAL_GLOBALS
Usart Serial1(&USART1_config, VARIANT_USART1_TX_PIN, VARIANT_USART1_RX_PIN);
Usart Serial2(&USART2_config, VARIANT_USART2_TX_PIN, VARIANT_USART2_RX_PIN);
Usart Serial3(&USART3_config, VARIANT_USART3_TX_PIN, VARIANT_USART3_RX_PIN);
#endif

//
// IRQ register / unregister helper
//
inline void usart_irq_register(usart_interrupt_config_t &irq, const char *name)
{
    // get auto-assigned irqn and set in irq struct
    IRQn_Type irqn;
    irqn_aa_get(irqn, name);
    irq.interrupt_number = irqn;

    // create irq registration struct
    stc_irq_regi_conf_t irqConf = {
        .enIntSrc = irq.interrupt_source,
        .enIRQn = irq.interrupt_number,
        .pfnCallback = irq.interrupt_handler,
    };

    // register and enable irq
    enIrqRegistration(&irqConf);
    NVIC_SetPriority(irqConf.enIRQn, irq.interrupt_priority);
    NVIC_ClearPendingIRQ(irqConf.enIRQn);
    NVIC_EnableIRQ(irqConf.enIRQn);
}

inline void usart_irq_resign(usart_interrupt_config_t &irq, const char *name)
{
    // disable interrupt and clear pending
    NVIC_DisableIRQ(irq.interrupt_number);
    NVIC_ClearPendingIRQ(irq.interrupt_number);
    enIrqResign(irq.interrupt_number);

    // resign auto-assigned irqn
    irqn_aa_resign(irq.interrupt_number, name);
}

//
// debug print helpers
//
#define USART_REG_TO_X(reg) \
    reg == M4_USART1   ? 1  \
    : reg == M4_USART2 ? 2  \
    : reg == M4_USART3 ? 3  \
    : reg == M4_USART4 ? 4  \
                       : 0
#define USART_DEBUG_PRINTF(fmt, ...) \
    CORE_DEBUG_PRINTF("[USART%d] " fmt, USART_REG_TO_X(this->config->peripheral.register_base), ##__VA_ARGS__)

//
// Usart class implementation
//
Usart::Usart(struct usart_config_t *config, gpio_pin_t tx_pin, gpio_pin_t rx_pin)
{
    ASSERT_GPIO_PIN_VALID(tx_pin, "USART() tx_pin");
    ASSERT_GPIO_PIN_VALID(rx_pin, "USART() rx_pin");

    this->config = config;
    this->tx_pin = tx_pin;
    this->rx_pin = rx_pin;

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
        CORE_ASSERT_FAIL("USART: invalid stop bit configuration");
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
        CORE_ASSERT_FAIL("USART: invalid parity configuration");
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
        CORE_ASSERT_FAIL("USART: invalid data bits configuration");
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
    // this->txBuffer->clear();

    // set IO pin functions
    GPIO_SetFunc(this->tx_pin, this->config->peripheral.tx_pin_function);
    GPIO_SetFunc(this->rx_pin, this->config->peripheral.rx_pin_function);

    // enable peripheral clock
    PWC_Fcg1PeriphClockCmd(this->config->peripheral.clock_id, Enable);

    // initialize usart peripheral and set baud rate
    USART_UART_Init(this->config->peripheral.register_base, config);
    USART_SetBaudrate(this->config->peripheral.register_base, baud);

    // setup usart interrupts
    usart_irq_register(this->config->interrupts.rx_data_available, "usart rx data available");
    usart_irq_register(this->config->interrupts.rx_error, "usart rx error");
    usart_irq_register(this->config->interrupts.tx_buffer_empty, "usart tx buffer empty");
    usart_irq_register(this->config->interrupts.tx_complete, "usart tx complete");

    // enable usart RX + interrupts
    // (tx is enabled on-demand when data is available to send)
    USART_FuncCmd(this->config->peripheral.register_base, UsartRx, Enable);
    USART_FuncCmd(this->config->peripheral.register_base, UsartRxInt, Enable);

    // write debug message AFTER init (this UART may be used for the debug message)
    USART_DEBUG_PRINTF("begin completed\n");
    this->initialized = true;
}

void Usart::end()
{
    // write debug message BEFORE deinit (this UART may be used for the debug message)
    USART_DEBUG_PRINTF("end()\n");

    // wait for tx buffer to empty
    flush();

    // disable uart peripheral
    USART_FuncCmd(this->config->peripheral.register_base, UsartTx, Disable);
    USART_FuncCmd(this->config->peripheral.register_base, UsartRx, Disable);

    // resign usart interrupts
    usart_irq_resign(this->config->interrupts.rx_data_available, "usart rx data available");
    usart_irq_resign(this->config->interrupts.rx_error, "usart rx error");
    usart_irq_resign(this->config->interrupts.tx_buffer_empty, "usart tx buffer empty");
    usart_irq_resign(this->config->interrupts.tx_complete, "usart tx complete");

    // deinit uart
    USART_DeInit(this->config->peripheral.register_base);

    // clear rx and tx buffers
    this->rxBuffer->clear();
    this->txBuffer->clear();

    this->initialized = false;
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
    // ignore if not initialized
    if (!this->initialized)
    {
        return;
    }

    // wait for tx buffer to empty
    while (!this->txBuffer->isEmpty())
        ;
}

size_t Usart::write(uint8_t ch)
{
    // if uninitialized, ignore write
    if (!this->initialized)
    {
        return 1;
    }

    // wait until tx buffer is no longer full
    while (this->txBuffer->isFull())
    {
        yield();
    }

    // add to tx buffer
    while (!this->txBuffer->push(ch))
    {
        yield();
    }

    // enable tx + empty interrupt
    USART_FuncCmd(this->config->peripheral.register_base, UsartTxAndTxEmptyInt, Enable);

    // wrote one byte
    return 1;
}

const usart_receive_error_t Usart::getReceiveError()
{
    auto rxError = this->config->state.rx_error;
    this->config->state.rx_error = usart_receive_error_t::None;
    return rxError;
}
