#include <hc32_ddl.h>
#include <addon_usart.h>
#include "Usart.h"
#include "core_hooks.h"
#include "core_debug.h"
#include "yield.h"
#include "../gpio/gpio.h"
#include "../irqn/irqn.h"
#include "../sysclock/sysclock.h"

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
    NVIC_SetPriority(irqConf.enIRQn, DDL_IRQ_PRIORITY_03);
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
// automatic clock divider + oversampling calculation
//
#ifdef USART_AUTO_CLKDIV_OS_CONFIG
#define CLKDIV_OS_DEBUG_ENABLE 0

#if CLKDIV_OS_DEBUG_ENABLE == 1
#define CLKDIV_OS_DEBUG_PRINTF(fmt, ...) \
    CORE_DEBUG_PRINTF("[CLKDIV_OS] " fmt, ##__VA_ARGS__)
#else
#define CLKDIV_OS_DEBUG_PRINTF(fmt, ...)
#endif

#include "usart_util.h"

/**
 * @brief calculate the real baudrate that will be achieved with the given parameters
 * @param usartClkDiv the clock divider used for the USART peripheral
 * @param over8 the oversampling mode (0: 16-bit, 1: 8-bit)
 * @param targetBaudrate the target baudrate
 * @return the real baudrate that will be achieved with the given parameters, or -1.0f if the parameters are invalid 
 */
float calculateRealBaudrate(uint32_t usartClkDiv, uint8_t over8, uint32_t targetBaudrate) 
{
    // the usart base clock is PCLK1 / usartClkDiv
    update_system_clock_frequencies();
    uint32_t usartBaseClock = SYSTEM_CLOCK_FREQUENCIES.pclk1 / usartClkDiv;
    CLKDIV_OS_DEBUG_PRINTF("usartBaseClock: %lu\n", usartBaseClock);
    
    // calculate dividers (integer + fractional) as in SetUartBaudrate (hc32f460_usart.c line 1405 ff.)
    float DIV = ((float)usartBaseClock / ((float)targetBaudrate * 8.0f * (2.0f - (float)over8))) - 1.0f;
    uint32_t DIV_integer = (uint32_t)DIV;

    uint64_t fractTmp = (uint64_t)(((uint64_t)2ul - (uint64_t)over8) * ((uint64_t)DIV_integer + 1ul) * (uint64_t)targetBaudrate);
    uint32_t DIV_fraction = (uint32_t)(2048ul * fractTmp / usartBaseClock - 128ul);

    // only use fractional divider if the fractional part is not too small
    bool useFractionalDivider = (DIV - (float)DIV_integer) > 0.00001f;

    // check if dividers are valid
    if (DIV < 0.0f || DIV_integer > 0xFFul) {
        // DIV must be between 0 and 0xFF
        return -1.0f;
    }

    // if fractional divider is used, check if the fractional part is valid
    if (useFractionalDivider && (DIV_fraction > 0x1FFul)) {
        // DIV_fraction must be between 0 and 0x1FF
        return -1.0f;
    }

    return calculateBaudrate(usartBaseClock, DIV_integer, DIV_fraction, over8, useFractionalDivider);
}

/**
 * @brief find the best clock divider and oversampling mode for the given target baudrate
 * @param targetBaudrate the target baudrate
 * @param bestClkDiv the best clock divider found (OUTPUT)
 * @param bestOver8 the best oversampling mode found (OUTPUT; 0: 16-bit, 1: 8-bit)
 * @param bestError the error of the best configuration found (OUTPUT)
 * @return true if a valid configuration was found, false if no valid configuration was found
 */
bool findBestClockDivAndOversamplingMode(uint32_t targetBaudrate, uint32_t &bestClkDiv, uint8_t &bestOver8, float &bestError) 
{
    // list of valid clock dividers
    static const uint16_t clkDividers[] = {1, 4, 16, 64};
    #define CLK_DIVIDER_COUNT (sizeof(clkDividers) / sizeof(clkDividers[0]))

    // iterate all configurations and find the best one
    bestClkDiv = 0;
    bestOver8 = 0;
    bestError = targetBaudrate;

    for (uint8_t over8 = 0; over8 <= 1; over8++)
    {
        for (size_t clkDivIndex = 0; clkDivIndex < CLK_DIVIDER_COUNT; clkDivIndex++)
        {
            uint32_t clkDiv = clkDividers[clkDivIndex];
            float realBaudrate = calculateRealBaudrate(clkDiv, over8, targetBaudrate);
            if (realBaudrate < 0.0f) {
                // invalid configuration
                CLKDIV_OS_DEBUG_PRINTF("invalid configuration: clkDiv=%u, over8=%u @ targetBaud=%u\n", clkDiv, over8, targetBaudrate);
                continue;
            }

            float error = (float)targetBaudrate - realBaudrate;
            CLKDIV_OS_DEBUG_PRINTF("@targetBaud=%u; realBaud=%.2f (%.2f), clkDiv=%u, over8=%u\n", targetBaudrate, realBaudrate, error, clkDiv, over8);
            if (error < 0.0f) {
                // error shall be an absolute value
                error = -error;
            }

            if (error < bestError) {
                // found a new best configuration
                bestError = error;
                bestClkDiv = clkDiv;
                bestOver8 = over8;
            }
        }
    }

    // if a valid configuration was found, bestClkDiv cannot be 0
    return bestClkDiv != 0;
}

#define CLK_DIVIDER_INT_TO_ENUM(div) \
    div == 1  ? UsartClkDiv_1  \
    : div == 4  ? UsartClkDiv_4  \
    : div == 16 ? UsartClkDiv_16 \
    : div == 64 ? UsartClkDiv_64 \
    : UsartClkDiv_1

#define OVERSAMPLING_INT_TO_ENUM(over8) \
    over8 == 0 ? UsartSampleBit16  \
    : over8 == 1 ? UsartSampleBit8  \
    : UsartSampleBit8

/**
 * @brief set the calculated clock divider and oversampling mode for the given target baudrate
 * @param config the USART configuration to set the clock divider and oversampling mode in
 * @param targetBaudrate the target baudrate 
 */
inline void setCalculatedClockDivAndOversampling(stc_usart_uart_init_t* config, uint32_t targetBaudrate)
{
    uint32_t bestClkDiv;
    uint8_t bestOver8;
    float bestError;
    if (findBestClockDivAndOversamplingMode(targetBaudrate, bestClkDiv, bestOver8, bestError))
    {
        config->enClkDiv = CLK_DIVIDER_INT_TO_ENUM(bestClkDiv);
        config->enSampleMode = OVERSAMPLING_INT_TO_ENUM(bestOver8);
        CLKDIV_OS_DEBUG_PRINTF("final @targetBaud=%u: clkDiv=%u, over8=%u, error=%.2f\n", targetBaudrate, bestClkDiv, bestOver8, bestError);
    }
    else
    {
        panic("could not find valid clock divider and oversampling mode for target baudrate");
    }
}
#endif // USART_AUTO_CLKDIV_OS_CONFIG

//
// Usart RX DMA
//
// [USART] -> [AOS] -> [DMA]
// 
// 1. USART triggers RI event
// 2. AOS forwards the event to the DMA Channel and starts the transfer
// 3.1. DMA transfers the data from the USART RX data register to the RX buffer
// 3.2. DMA increments the destination address 
//      and loops back to the start if it copied more than the buffer capacity
// 4. DMA trigger the block transfer complete (BTC) interrupt
// 5. the BTC handler calculates the amount of data transferred since the last BTC interrupt 
//    and updates the ring buffer accordingly
//
#ifdef USART_RX_DMA_SUPPORT
inline en_int_src_t dma_unit_and_channel_to_btc_int_src(M4_DMA_TypeDef *unit, en_dma_channel_t channel)
{
    if (unit == M4_DMA1)
    {
        switch(channel)
        {
            case DmaCh0: return INT_DMA1_BTC0;
            case DmaCh1: return INT_DMA1_BTC1;
            case DmaCh2: return INT_DMA1_BTC2;
            case DmaCh3: return INT_DMA1_BTC3;
            default: break;
        }
    }
    else if (unit == M4_DMA2)
    {
        switch(channel)
        {
            case DmaCh0: return INT_DMA2_BTC0;
            case DmaCh1: return INT_DMA2_BTC1;
            case DmaCh2: return INT_DMA2_BTC2;
            case DmaCh3: return INT_DMA2_BTC3;
            default: break;
        }
    }

    panic("invalid DMA unit or channel");
    return INT_DMA1_BTC0;
}

void Usart::rx_dma_init()
{
    auto dma_unit = this->config->dma.dma_unit;
    auto dma_channel = this->config->dma.dma_channel;

    // enable clock of DMA and AOS
    if (dma_unit == M4_DMA1)
    {
        PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_DMA1, Enable);
    }
    else if (dma_unit == M4_DMA2)
    {
        PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_DMA2, Enable);
    }
    else
    {
        panic("invalid DMA unit");
    }

    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_AOS, Enable);

    // prepare DMA configuration
    // transfer from USART RX data register to the RX buffer
    // the source is a fixed address, the destination is incremented and loops back to the start
    uint32_t dmaSrcAddr = (uint32_t)&this->config->peripheral.register_base->DR + 2; // the RX data register is on bits 16-24 in DR
    uint32_t dmaDesAddr = this->config->dma.rx_buffer_start_address;
    size_t rxBufferCapacity = this->config->state.rx_buffer->capacity();
    if (rxBufferCapacity > UINT16_MAX)
    {
        panic("RX buffer capacity too large for DMA");
    }

    stc_dma_config_t dmaConfig = {
        .u16BlockSize = 1,                              // transfer 1 block (= byte) at a time
        .u16TransferCnt = 0,                            // do not limit transfer count
        .u32SrcAddr = dmaSrcAddr,                       // copy from USART RX data register
        .u32DesAddr = dmaDesAddr,                       // to the RX ring buffer
        .u16SrcRptSize = 0,                             // RX data register is not repeated
        .u16DesRptSize = (uint16_t) rxBufferCapacity,   // ring buffer is repeated after it is filled
        .stcDmaChCfg = {
            .enSrcInc = AddressFix,                     // source address remains fixed
            .enDesInc = AddressIncrease,                // destination address is incremented
            .enSrcRptEn = Disable,                      // source does not repeat / loop
            .enDesRptEn = Enable,                       // destination will loop back to the start after the buffer is filled
            .enTrnWidth = Dma8Bit,                      // transfer in 8-bit blocks
            .enIntEn = Enable,                          // enable the BTC interrupt
        }
    };

    // enable DMA and apply config
    DMA_Cmd(dma_unit, Enable);
    DMA_InitChannel(dma_unit, dma_channel, &dmaConfig);

    // get irqn for BTC interrupt
    IRQn_Type btcIrqn;
    irqn_aa_get(btcIrqn, "usart rx dma btc");
    this->config->dma.rx_data_available_dma_btc.interrupt_number = btcIrqn;

    // setup the DMA BTC interrupt
    stc_irq_regi_conf_t btcIrqConf = {
        .enIntSrc = dma_unit_and_channel_to_btc_int_src(dma_unit, dma_channel),
        .enIRQn = this->config->dma.rx_data_available_dma_btc.interrupt_number,
        .pfnCallback = this->config->dma.rx_data_available_dma_btc.interrupt_handler,
    };
    enIrqRegistration(&btcIrqConf);
    // note: irq priority should not matter much, as the btc interrupt handler can handle missed interrupts
    NVIC_SetPriority(btcIrqConf.enIRQn, DDL_IRQ_PRIORITY_03);
    NVIC_ClearPendingIRQ(btcIrqConf.enIRQn);
    NVIC_EnableIRQ(btcIrqConf.enIRQn);

    // enable the DMA channel
    DMA_ChannelCmd(dma_unit, dma_channel, Enable);

    // clear DMA transfer complete flags
    DMA_ClearIrqFlag(dma_unit, dma_channel, TrnCpltIrq);
    DMA_ClearIrqFlag(dma_unit, dma_channel, BlkTrnCpltIrq);

    // set the DMA to trigger from the usart data available event using AOS
    DMA_SetTriggerSrc(dma_unit, dma_channel, this->config->dma.rx_data_available_event);

    USART_DEBUG_PRINTF("rx_dma_init completed w/ rxBufferCapacity=%u\n", rxBufferCapacity);
}

void Usart::rx_dma_deinit()
{
    USART_DEBUG_PRINTF("rx_dma_deinit\n");

    auto dma_unit = this->config->dma.dma_unit;
    auto dma_channel = this->config->dma.dma_channel;

    // disable the DMA channel
    DMA_ChannelCmd(dma_unit, dma_channel, Disable);

    // clear DMA transfer complete flags
    DMA_ClearIrqFlag(dma_unit, dma_channel, TrnCpltIrq);
    DMA_ClearIrqFlag(dma_unit, dma_channel, BlkTrnCpltIrq);

    // disable and resign the DMA BTC interrupt
    auto btcIrqn = this->config->dma.rx_data_available_dma_btc.interrupt_number;
    NVIC_DisableIRQ(btcIrqn);
    NVIC_ClearPendingIRQ(btcIrqn);
    enIrqResign(btcIrqn);

    irqn_aa_resign(btcIrqn, "usart rx dma btc");

    // reset the dma channel to default values
    DMA_DeInit(dma_unit, dma_channel);

    // note: other systems may still use the DMA unit and AOS, so we do not disable their clock
    // or deinit the DMA unit
}

void Usart::enableRxDma(M4_DMA_TypeDef *dma, en_dma_channel_t channel)
{
    // set dma unit and channel
    this->config->dma.dma_unit = dma;
    this->config->dma.dma_channel = channel;

    // set the base address of the RX buffer
    volatile uint8_t *rxRawBuffer = this->config->state.rx_buffer->getBuffer();
    uint32_t rxStartAddr = (uint32_t)rxRawBuffer;

    this->config->dma.rx_buffer_start_address = rxStartAddr;
    this->config->dma.rx_buffer_last_dest_address = rxStartAddr;
}

void Usart::disableRxDma()
{
    // reset dma unit to disable
    this->config->dma.dma_unit = nullptr;
}
#endif // USART_RX_DMA_SUPPORT

//
// Usart class implementation
//
Usart::Usart(struct usart_config_t *config, gpio_pin_t tx_pin, gpio_pin_t rx_pin, size_t rx_buffer_size, size_t tx_buffer_size)
{
    CORE_ASSERT(config != NULL, "USART() config cannot be NULL");
    ASSERT_GPIO_PIN_VALID(tx_pin, "USART() tx_pin");
    ASSERT_GPIO_PIN_VALID(rx_pin, "USART() rx_pin");

    this->config = config;
    this->tx_pin = tx_pin;
    this->rx_pin = rx_pin;
    
    // initialize and assign rx and tx buffers
    this->rxBuffer = new RingBuffer<uint8_t>(rx_buffer_size);
    this->txBuffer = new RingBuffer<uint8_t>(tx_buffer_size);

    this->config->state.rx_buffer = this->rxBuffer;
    this->config->state.tx_buffer = this->txBuffer;
}

Usart::~Usart()
{
    // free rx and tx buffers
    delete this->rxBuffer;
    delete this->txBuffer;

    // unassign rx and tx buffers
    this->rxBuffer = nullptr;
    this->txBuffer = nullptr;

    this->config->state.rx_buffer = nullptr;
    this->config->state.tx_buffer = nullptr;
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

    #ifdef USART_AUTO_CLKDIV_OS_CONFIG
    // auto-calculate best clock divider and oversampling mode for the given baudrate
    setCalculatedClockDivAndOversampling(&usartConfig, baud);
    #endif

    // call begin with full config
    begin(baud, &usartConfig);
}

void Usart::begin(uint32_t baud, const stc_usart_uart_init_t *config, const bool rxNoiseFilter)
{
    // clear rx and tx buffers
    this->rxBuffer->clear();
    this->txBuffer->clear();

    // set IO pin functions
    GPIO_SetFunc(this->tx_pin, this->config->peripheral.tx_pin_function);
    GPIO_SetFunc(this->rx_pin, this->config->peripheral.rx_pin_function);

    // enable peripheral clock
    PWC_Fcg1PeriphClockCmd(this->config->peripheral.clock_id, Enable);

    // initialize usart peripheral and set baud rate
    USART_UART_Init(this->config->peripheral.register_base, config);
    SetUartBaudrate(this->config->peripheral.register_base, baud);

    // set noise filtering on RX line
    USART_FuncCmd(this->config->peripheral.register_base, UsartNoiseFilter, rxNoiseFilter ? Enable : Disable);

    // setup usart interrupts
    usart_irq_register(this->config->interrupts.rx_error, "usart rx error");
    usart_irq_register(this->config->interrupts.tx_buffer_empty, "usart tx buffer empty");
    usart_irq_register(this->config->interrupts.tx_complete, "usart tx complete");

    #ifdef USART_RX_DMA_SUPPORT
    if (this->config->dma.is_dma_enabled())
    {
        // setup RX dma
        rx_dma_init();
    } 
    else
    #endif // USART_RX_DMA_SUPPORT
    {
        // setup RX interrupt
        usart_irq_register(this->config->interrupts.rx_data_available, "usart rx data available");
    }

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

    // clear initialized flag early so write() ignores calls 
    // and doesn't try to wait for tx buffer to empty
    this->initialized = false;

    // disable uart peripheral
    USART_FuncCmd(this->config->peripheral.register_base, UsartTx, Disable);
    USART_FuncCmd(this->config->peripheral.register_base, UsartRx, Disable);

    // resign usart interrupts
    usart_irq_resign(this->config->interrupts.rx_error, "usart rx error");
    usart_irq_resign(this->config->interrupts.tx_buffer_empty, "usart tx buffer empty");
    usart_irq_resign(this->config->interrupts.tx_complete, "usart tx complete");

    #ifdef USART_RX_DMA_SUPPORT
    if (this->config->dma.is_dma_enabled())
    {
        // de-init RX dma
        rx_dma_deinit();
    }
    else
    #endif // USART_RX_DMA_SUPPORT
    {
        // resign RX interrupt
        usart_irq_resign(this->config->interrupts.rx_data_available, "usart rx data available");
    }

    // deinit uart
    USART_DeInit(this->config->peripheral.register_base);

    // disable peripheral clock
    PWC_Fcg1PeriphClockCmd(this->config->peripheral.clock_id, Disable);

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
    {
        yield();
    }
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
