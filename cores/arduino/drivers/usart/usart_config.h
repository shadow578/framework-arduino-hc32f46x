#pragma once
#include <hc32_ddl.h>
#include "../../RingBuffer.h"

/**
 * @brief USART peripheral configuration
 */
struct usart_peripheral_config_t
{
    /**
     * @brief USART peripheral register base address
     */
    M4_USART_TypeDef *register_base;

    /**
     * @brief USART peripheral clock id
     * @note in FCG1
     */
    uint32_t clock_id;

    /**
     * @brief pin function for usart tx pin
     */
    en_port_func_t tx_pin_function;

    /**
     * @brief pin function for usart rx pin
     */
    en_port_func_t rx_pin_function;
};

/**
 * @brief USART single interrupt and handler configuration
 */
struct usart_interrupt_config_t
{
    /**
     * @brief IRQn assigned to this interrupt handler
     * @note auto-assigned in Usart implementation
     */
    IRQn_Type interrupt_number;

    /**
     * @brief Interrupt source to set for this interrupt handler
     */
    en_int_src_t interrupt_source;

    /**
     * @brief Interrupt handler function pointer
     */
    func_ptr_t interrupt_handler;
};

/**
 * @brief USART interrupts and handlers configuration
 */
struct usart_interrupts_config_t
{
    /**
     * @brief USART receive data available interrupt configuration
     */
    usart_interrupt_config_t rx_data_available;

    /**
     * @brief USART receive error interrupt configuration
     */
    usart_interrupt_config_t rx_error;

    /**
     * @brief USART transmit buffer empty interrupt configuration
     */
    usart_interrupt_config_t tx_buffer_empty;

    /**
     * @brief USART transmit complete interrupt configuration
     */
    usart_interrupt_config_t tx_complete;
};

/**
 * @brief USART receive error codes
 */
enum class usart_receive_error_t
{
    /**
     * @brief no error
     */
    None,

    /**
     * @brief usart peripheral framing error
     * @note occurs, for example, when the stop bit of the received data is not detected
     */
    FramingError,

    /**
     * @brief usart peripheral parity error
     * @note occurs when the parity of the received data does not match the expected parity
     */
    ParityError,

    /**
     * @brief usart peripheral RX register overrun error
     * @note occurs when the RX data register is not read before the next data is received
     */
    OverrunError,

    /**
     * @brief RX ring buffer was full and a new byte was received
     * @note occurs when the RX ring buffer is full and a new byte is received
     * @note this error indicates that the oldest data in the RX buffer was lost
     */
    RxDataDropped,
};

#ifdef USART_RX_ERROR_COUNTERS_ENABLE
/**
 * @brief USART receive error counters
 */
struct usart_rx_error_counters_t
{
    /**
     * @brief counter for usart_receive_error_t::FramingError
     */
    uint32_t framing_error;

    /**
     * @brief counter for usart_receive_error_t::ParityError
     */
    uint32_t parity_error;

    /**
     * @brief counter for usart_receive_error_t::OverrunError
     */
    uint32_t overrun_error;

    /**
     * @brief counter for usart_receive_error_t::RxDataDropped
     */
    uint32_t rx_data_dropped;
};
#endif // USART_RX_ERROR_COUNTERS_ENABLE

/**
 * @brief USART runtime states
 */
struct usart_runtime_state_t
{
    /**
     * @brief USART receive buffer
     * @note allocated and freed in Usart class (constructor and destructor methods)
     */
    RingBuffer<uint8_t> *rx_buffer;

    /**
     * @brief USART transmit buffer
     * @note allocated and freed in Usart class (constructor and destructor methods)
     */
    RingBuffer<uint8_t> *tx_buffer;

    /**
     * @brief last error in RX error interrupt handler
     */
    usart_receive_error_t rx_error;

    #ifdef USART_RX_ERROR_COUNTERS_ENABLE
    /**
     * @brief USART receive error counters
     */
    usart_rx_error_counters_t rx_error_counters;
    #endif
};

#ifdef USART_RX_DMA_SUPPORT
/**
 * @brief USART DMA block transfer complete interrupt configuration
 * @note cannot use usart_interrupt_config_t since DMA peripheral and channel are variable
 */
struct usart_dma_btc_interrupt_config_t
{
    /**
     * @brief IRQn assigned to this interrupt handler
     * @note auto-assigned in Usart implementation
     */
    IRQn_Type interrupt_number;

    /**
     * @brief Interrupt handler function pointer
     */
    func_ptr_t interrupt_handler;
};

/**
 * @brief USART DMA configuration
 */
struct usart_dma_config_t
{
    /**
     * @brief DMA unit to use for USART RX
     * @note assigned in Usart class
     * @note dma is disabled if this is not a valid DMA unit
     */
    M4_DMA_TypeDef *dma_unit;

    /**
     * @brief DMA channel to use for USART RX
     * @note assigned in Usart class
     */
    en_dma_channel_t dma_channel;

    /**
     * @brief base address of the DMA target buffer
     * @note assigned in Usart class
     */
    uint32_t rx_buffer_start_address;

    /**
     * @brief destination address of the last DMA transfer as observed in the block transfer complete interrupt
     * @note initialized to rx_buffer_start_address in Usart class, 
     *       updated in the block transfer complete interrupt
     */
    uint32_t rx_buffer_last_dest_address;

    /**
     * @brief AOS event source for USART rx data available
     */
    en_event_src_t rx_data_available_event;

    /**
     * @brief USART receive DMA block transfer complete interrupt configuration
     */
    usart_dma_btc_interrupt_config_t rx_data_available_dma_btc;
    
    #ifdef __cplusplus
    /**
     * @brief is dma_unit a valid DMA unit?
     */
    bool is_dma_enabled() const
    {
      return dma_unit == M4_DMA1 || dma_unit == M4_DMA2;
    }
    #endif
};
#endif // USART_RX_DMA_SUPPORT

/**
 * @brief USART device configuration
 */
struct usart_config_t
{
    /**
     * @brief USART peripheral configuration
     */
    usart_peripheral_config_t peripheral;

    /**
     * @brief USART interrupts and handlers configuration
     */
    usart_interrupts_config_t interrupts;

    /**
     * @brief USART runtime states
     */
    usart_runtime_state_t state;

    #ifdef USART_RX_DMA_SUPPORT
    /**
     * @brief USART DMA configuration
     */
    usart_dma_config_t dma;
    #endif
};

//
// USART 1-3 configurations
//
extern usart_config_t USART1_config;
extern usart_config_t USART2_config;
extern usart_config_t USART3_config;
extern usart_config_t USART4_config;
