#include "usart_config.h"
#include "../../core_hooks.h"

#define USART_COUNT 4
usart_config_t *USARTx[USART_COUNT] = {
    &USART1_config,
    &USART2_config,
    &USART3_config,
    &USART4_config,
};

//
// IRQ handler implementations
//

#ifdef USART_RX_DMA_SUPPORT
static void USART_rx_dma_btc_irq(uint8_t x)
{
    usart_config_t *usartx = USARTx[x - 1];

    M4_DMA_TypeDef *dma_unit = usartx->dma.dma_unit;
    en_dma_channel_t dma_channel = usartx->dma.dma_channel;

    // get the number of bytes transferred since the last block transfer complete interrupt
    // this *may* be more than 1 if we missed a previous interrupt, so it's calculated
    uint32_t last_dest_address = usartx->dma.rx_buffer_last_dest_address;
    uint32_t current_dest_address = DMA_GetDesAddr(dma_unit, dma_channel);

    size_t received_bytes;
    if (current_dest_address == last_dest_address)
    {
        // no data was received, ignore
        received_bytes = 0;
    }
    else if (current_dest_address > last_dest_address)
    {
        // buffer did not overflow
        //
        // the amount of data received is simply the difference between the current and last destination addresses 
        //
        // [   0   ]
        // [   1   ] <- last    |
        // [   2   ]            |
        // [   3   ]            | #1 (len = pos - old_pos)
        // [   4   ]            |
        // [   5   ]            |
        // [   6   ] <- current |
        // [   7   ]
        // [ N - 1 ]
        received_bytes = current_dest_address - last_dest_address;
    }
    else
    {
        // buffer did overflowed
        //
        // the amount of data received is the difference between the current destination address and the end of the buffer
        // plus the difference between the last destination address and the start of the buffer
        //
        // [   0   ]            |
        // [   1   ]            | #2 (len = current)
        // [   2   ]            |
        // [   3   ] <- current
        // [   4   ] <- last    |
        // [   5   ]            |
        // [   6   ]            | #1 (len = N - last)
        // [   7   ]            |
        // [ N - 1 ]            |
        size_t buffer_len = usartx->state.rx_buffer->capacity();
        received_bytes = (buffer_len - last_dest_address) + current_dest_address;
    }

    // then, update the write pointer in the rx buffer and the last destination address
    bool rxOverrun = usartx->state.rx_buffer->_update_write_index(received_bytes);
    usartx->dma.rx_buffer_last_dest_address = current_dest_address;

    // if the buffer was overrun, set the overrun error flag
    if (rxOverrun)
    {
        usartx->state.rx_error = usart_receive_error_t::RxDataDropped;
        
        #ifdef USART_RX_ERROR_COUNTERS_ENABLE
        usartx->state.rx_error_counters.rx_data_dropped++;
        #endif
    }

    // finally, clear the DMA block transfer complete flag
    DMA_ClearIrqFlag(dma_unit, dma_channel, BlkTrnCpltIrq);
}
#endif // USART_RX_DMA_SUPPORT

static void USART_rx_data_available_irq(uint8_t x)
{
    usart_config_t *usartx = USARTx[x - 1];

    // get the received byte and push it to the rx buffer
    uint8_t ch = USART_RecData(usartx->peripheral.register_base);
    core_hook_usart_rx_irq(ch, x);
    
    bool rxOverrun;
    usartx->state.rx_buffer->push(ch, /*force*/true, rxOverrun);

    // if the buffer was overrun, set the overrun error flag
    if (rxOverrun)
    {
        usartx->state.rx_error = usart_receive_error_t::RxDataDropped;
        
        #ifdef USART_RX_ERROR_COUNTERS_ENABLE
        usartx->state.rx_error_counters.rx_data_dropped++;
        #endif
    }
}

static void USART_rx_error_irq(uint8_t x)
{
    usart_config_t *usartx = USARTx[x - 1];

    // check and clear error flags
    if (USART_GetStatus(usartx->peripheral.register_base, UsartFrameErr) == Set)
    {
        USART_ClearStatus(usartx->peripheral.register_base, UsartFrameErr);
        usartx->state.rx_error = usart_receive_error_t::FramingError;

        #ifdef USART_RX_ERROR_COUNTERS_ENABLE
        usartx->state.rx_error_counters.framing_error++;
        #endif
    }

    if (USART_GetStatus(usartx->peripheral.register_base, UsartParityErr) == Set)
    {
        USART_ClearStatus(usartx->peripheral.register_base, UsartParityErr);
        usartx->state.rx_error = usart_receive_error_t::ParityError;

        #ifdef USART_RX_ERROR_COUNTERS_ENABLE
        usartx->state.rx_error_counters.parity_error++;
        #endif
    }

    if (USART_GetStatus(usartx->peripheral.register_base, UsartOverrunErr) == Set)
    {
        USART_ClearStatus(usartx->peripheral.register_base, UsartOverrunErr);
        usartx->state.rx_error = usart_receive_error_t::OverrunError;

        #ifdef USART_RX_ERROR_COUNTERS_ENABLE
        usartx->state.rx_error_counters.overrun_error++;
        #endif
    }
}

static void USART_tx_buffer_empty_irq(uint8_t x)
{
    usart_config_t *usartx = USARTx[x - 1];

    // get the next byte from the tx buffer
    uint8_t ch;
    if (usartx->state.tx_buffer->pop(ch))
    {
        // call hook, then send the byte
        core_hook_usart_tx_irq(ch, x);
        USART_SendData(usartx->peripheral.register_base, ch);
    }
    else
    {
        // disable TX empty interrupt, and enable TX complete interrupt
        // (tx complete interrupt will disable TX when it fires)
        USART_FuncCmd(usartx->peripheral.register_base, UsartTxEmptyInt, Disable);
        USART_FuncCmd(usartx->peripheral.register_base, UsartTxCmpltInt, Enable);
    }
}

static void USART_tx_complete_irq(uint8_t x)
{
    usart_config_t *usartx = USARTx[x - 1];

    // disable TX and TX complete interrupts
    USART_FuncCmd(usartx->peripheral.register_base, UsartTxCmpltInt, Disable);
    USART_FuncCmd(usartx->peripheral.register_base, UsartTx, Disable);
}

//
// IRQ handler templates
//

#define IS_VALID_USARTx(x) ((x) >= 1 && (x) <= USART_COUNT)
#define ASSERT_VALID_USARTx(x) static_assert(IS_VALID_USARTx(x), "USART number must be between 1 and USART_COUNT")

#if USART_RX_DMA_SUPPORT
template <uint8_t x>
static void USARTx_rx_da_dma_btc_irq(void)
{
    ASSERT_VALID_USARTx(x);
    USART_rx_dma_btc_irq(x);
}
#endif // USART_RX_DMA_SUPPORT

template <uint8_t x>
static void USARTx_rx_data_available_irq(void)
{
    ASSERT_VALID_USARTx(x);
    USART_rx_data_available_irq(x);
}

template <uint8_t x>
static void USARTx_rx_error_irq(void)
{
    ASSERT_VALID_USARTx(x);
    USART_rx_error_irq(x);
}

template <uint8_t x>
static void USARTx_tx_buffer_empty_irq(void)
{
    ASSERT_VALID_USARTx(x);
    USART_tx_buffer_empty_irq(x);
}

template <uint8_t x>
static void USARTx_tx_complete_irq(void)
{
    ASSERT_VALID_USARTx(x);
    USART_tx_complete_irq(x);
}
