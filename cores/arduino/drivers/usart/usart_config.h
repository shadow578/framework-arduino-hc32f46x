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
     * @brief interrupt priority
     */
    uint32_t interrupt_priority;

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
    None,
    FramingError,
    ParityError,
    OverrunError
};

/**
 * @brief USART runtime states
 */
struct usart_runtime_state_t
{
    /**
     * @brief USART receive buffer
     */
    RingBuffer<uint8_t> *rx_buffer;

    /**
     * @brief USART transmit buffer
     */
    RingBuffer<uint8_t> *tx_buffer;

    /**
     * @brief last error in RX error interrupt handler
     */
    usart_receive_error_t rx_error;
};

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
};

//
// USART 1-3 configurations
//
extern usart_config_t USART1_config;
extern usart_config_t USART2_config;
extern usart_config_t USART3_config;
