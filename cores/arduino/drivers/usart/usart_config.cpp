#include "usart_config.h"
#include "usart_handlers.h"
#include "../../WVariant.h"
#include "../../core_hooks.h"

#ifndef SERIAL_BUFFER_SIZE
#define SERIAL_BUFFER_SIZE 64
#endif
#ifndef SERIAL_TX_BUFFER_SIZE
#define SERIAL_TX_BUFFER_SIZE SERIAL_BUFFER_SIZE
#endif
#ifndef SERIAL_RX_BUFFER_SIZE
#define SERIAL_RX_BUFFER_SIZE SERIAL_BUFFER_SIZE
#endif

//
// USART configurations
//
usart_config_t USART1_config = {
    .peripheral = {
        .register_base = M4_USART1,
        .clock_id = PWC_FCG1_PERIPH_USART1,
        .tx_pin_function = Func_Usart1_Tx,
        .rx_pin_function = Func_Usart1_Rx,
    },
    .interrupts = {
        .rx_data_available = {
            .interrupt_priority = DDL_IRQ_PRIORITY_03,
            .interrupt_source = INT_USART1_RI,
            .interrupt_handler = USARTx_rx_data_available_irq<1>,
        },
        .rx_error = {
            .interrupt_priority = DDL_IRQ_PRIORITY_03,
            .interrupt_source = INT_USART1_EI,
            .interrupt_handler = USARTx_rx_error_irq<1>,
        },
        .tx_buffer_empty = {
            .interrupt_priority = DDL_IRQ_PRIORITY_03,
            .interrupt_source = INT_USART1_TI,
            .interrupt_handler = USARTx_tx_buffer_empty_irq<1>,
        },
        .tx_complete = {
            .interrupt_priority = DDL_IRQ_PRIORITY_03,
            .interrupt_source = INT_USART1_TCI,
            .interrupt_handler = USARTx_tx_complete_irq<1>,
        },
    },
    .state = {
        .rx_buffer = new RingBuffer<uint8_t>(SERIAL_RX_BUFFER_SIZE),
        .tx_buffer = new RingBuffer<uint8_t>(SERIAL_TX_BUFFER_SIZE),
        .rx_error = usart_receive_error_t::None,
    },
};

usart_config_t USART2_config = {
    .peripheral = {
        .register_base = M4_USART2,
        .clock_id = PWC_FCG1_PERIPH_USART2,
        .tx_pin_function = Func_Usart2_Tx,
        .rx_pin_function = Func_Usart2_Rx,
    },
    .interrupts = {
        .rx_data_available = {
            .interrupt_priority = DDL_IRQ_PRIORITY_03,
            .interrupt_source = INT_USART2_RI,
            .interrupt_handler = USARTx_rx_data_available_irq<2>,
        },
        .rx_error = {
            .interrupt_priority = DDL_IRQ_PRIORITY_03,
            .interrupt_source = INT_USART2_EI,
            .interrupt_handler = USARTx_rx_error_irq<2>,
        },
        .tx_buffer_empty = {
            .interrupt_priority = DDL_IRQ_PRIORITY_03,
            .interrupt_source = INT_USART2_TI,
            .interrupt_handler = USARTx_tx_buffer_empty_irq<2>,
        },
        .tx_complete = {
            .interrupt_priority = DDL_IRQ_PRIORITY_03,
            .interrupt_source = INT_USART2_TCI,
            .interrupt_handler = USARTx_tx_complete_irq<2>,
        },
    },
    .state = {
        .rx_buffer = new RingBuffer<uint8_t>(SERIAL_RX_BUFFER_SIZE),
        .tx_buffer = new RingBuffer<uint8_t>(SERIAL_TX_BUFFER_SIZE),
        .rx_error = usart_receive_error_t::None,
    },
};

usart_config_t USART3_config = {
    .peripheral = {
        .register_base = M4_USART3,
        .clock_id = PWC_FCG1_PERIPH_USART3,
        .tx_pin_function = Func_Usart3_Tx,
        .rx_pin_function = Func_Usart3_Rx,
    },
    .interrupts = {
        .rx_data_available = {
            .interrupt_priority = DDL_IRQ_PRIORITY_03,
            .interrupt_source = INT_USART3_RI,
            .interrupt_handler = USARTx_rx_data_available_irq<3>,
        },
        .rx_error = {
            .interrupt_priority = DDL_IRQ_PRIORITY_03,
            .interrupt_source = INT_USART3_EI,
            .interrupt_handler = USARTx_rx_error_irq<3>,
        },
        .tx_buffer_empty = {
            .interrupt_priority = DDL_IRQ_PRIORITY_03,
            .interrupt_source = INT_USART3_TI,
            .interrupt_handler = USARTx_tx_buffer_empty_irq<3>,
        },
        .tx_complete = {
            .interrupt_priority = DDL_IRQ_PRIORITY_03,
            .interrupt_source = INT_USART3_TCI,
            .interrupt_handler = USARTx_tx_complete_irq<3>,
        },
    },
    .state = {
        .rx_buffer = new RingBuffer<uint8_t>(SERIAL_RX_BUFFER_SIZE),
        .tx_buffer = new RingBuffer<uint8_t>(SERIAL_TX_BUFFER_SIZE),
        .rx_error = usart_receive_error_t::None,
    },
};
