#include "usart_config.h"
#include "usart_handlers.h"
#include "../gpio/gpio_pindefs.h"
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
    },
    .pins = {
        .tx_pin = PC0,
        .rx_pin = PC1,
    },
    .interrupts = {
        .rx_data_available = {
            .interrupt_number = Int002_IRQn,
            .interrupt_priority = DDL_IRQ_PRIORITY_03,
            .interrupt_source = INT_USART1_RI,
            .interrupt_handler = USARTx_rx_data_available_irq<1>,
        },
        .rx_error = {
            .interrupt_number = Int020_IRQn,
            .interrupt_priority = DDL_IRQ_PRIORITY_03,
            .interrupt_source = INT_USART1_EI,
            .interrupt_handler = USARTx_rx_error_irq<1>,
        },
        .tx_buffer_empty = {
            .interrupt_number = Int003_IRQn,
            .interrupt_priority = DDL_IRQ_PRIORITY_03,
            .interrupt_source = INT_USART1_TI,
            .interrupt_handler = USARTx_tx_buffer_empty_irq<1>,
        },
        .tx_complete = {
            .interrupt_number = Int021_IRQn,
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
    },
    .pins = {
        .tx_pin = PA9,
        .rx_pin = PA15,
    },
    .interrupts = {
        .rx_data_available = {
            .interrupt_number = Int004_IRQn,
            .interrupt_priority = DDL_IRQ_PRIORITY_03,
            .interrupt_source = INT_USART2_RI,
            .interrupt_handler = USARTx_rx_data_available_irq<2>,
        },
        .rx_error = {
            .interrupt_number = Int022_IRQn,
            .interrupt_priority = DDL_IRQ_PRIORITY_03,
            .interrupt_source = INT_USART2_EI,
            .interrupt_handler = USARTx_rx_error_irq<2>,
        },
        .tx_buffer_empty = {
            .interrupt_number = Int005_IRQn,
            .interrupt_priority = DDL_IRQ_PRIORITY_03,
            .interrupt_source = INT_USART2_TI,
            .interrupt_handler = USARTx_tx_buffer_empty_irq<2>,
        },
        .tx_complete = {
            .interrupt_number = Int023_IRQn,
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
    },
    .pins = {
        .tx_pin = PE5,
        .rx_pin = PE4,
    },
    .interrupts = {
        .rx_data_available = {
            .interrupt_number = Int008_IRQn,
            .interrupt_priority = DDL_IRQ_PRIORITY_03,
            .interrupt_source = INT_USART3_RI,
            .interrupt_handler = USARTx_rx_data_available_irq<3>,
        },
        .rx_error = {
            .interrupt_number = Int024_IRQn,
            .interrupt_priority = DDL_IRQ_PRIORITY_03,
            .interrupt_source = INT_USART3_EI,
            .interrupt_handler = USARTx_rx_error_irq<3>,
        },
        .tx_buffer_empty = {
            .interrupt_number = Int009_IRQn,
            .interrupt_priority = DDL_IRQ_PRIORITY_03,
            .interrupt_source = INT_USART3_TI,
            .interrupt_handler = USARTx_tx_buffer_empty_irq<3>,
        },
        .tx_complete = {
            .interrupt_number = Int025_IRQn,
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
