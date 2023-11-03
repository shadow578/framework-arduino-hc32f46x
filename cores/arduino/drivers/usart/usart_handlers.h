#include "usart_config.h"
#include "../../core_hooks.h"

#define USART_COUNT 3
usart_config_t *USARTx[USART_COUNT] = {
    &USART1_config,
    &USART2_config,
    &USART3_config,
};

//
// IRQ handler implementations
//

static void USART_rx_data_available_irq(uint8_t x)
{
    usart_config_t *usartx = USARTx[x - 1];

    // get the received byte and push it to the rx buffer
    uint8_t ch = USART_RecData(usartx->peripheral.register_base);
    core_hook_usart_rx_irq(ch, x);
    usartx->state.rx_buffer->push(ch);
}

static void USART_rx_error_irq(uint8_t x)
{
    usart_config_t *usartx = USARTx[x - 1];

    // check and clear error flags
    if (USART_GetStatus(usartx->peripheral.register_base, UsartFrameErr) == Set)
    {
        USART_ClearStatus(usartx->peripheral.register_base, UsartFrameErr);
        usartx->state.rx_error = usart_receive_error_t::FramingError;
    }

    if (USART_GetStatus(usartx->peripheral.register_base, UsartParityErr) == Set)
    {
        USART_ClearStatus(usartx->peripheral.register_base, UsartParityErr);
        usartx->state.rx_error = usart_receive_error_t::ParityError;
    }

    if (USART_GetStatus(usartx->peripheral.register_base, UsartOverrunErr) == Set)
    {
        USART_ClearStatus(usartx->peripheral.register_base, UsartOverrunErr);
        usartx->state.rx_error = usart_receive_error_t::OverrunError;
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
