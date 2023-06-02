#pragma once
#include <hc32_ddl.h>

/**
 * @brief timer0 peripheral config
 */
typedef struct timer0_peripheral_config_t
{
    /**
     * @brief The base address of the timer peripheral.
     */
    M4_TMR0_TypeDef *register_base;

    /**
     * @brief The clock id of the timer peripheral.
     * @note in FCG2
     */
    uint32_t clock_id;
} timer0_peripheral_config_t;

/**
 * @brief timer0 interrupt config
 */
typedef struct timer0_interrupt_config_t
{
    /**
     * @brief IRQn assigned to this interrupt handler
     * @note auto-assigned in Usart implementation
     */
    IRQn_Type interrupt_number;

    /**
     * @brief interrupt source
     */
    en_int_src_t interrupt_source;

    /**
     * @brief static interrupt handler pointer
     */
    func_ptr_t interrupt_handler;

    /**
     * @brief user callback function pointer
     * @note may be NULL
     */
    func_ptr_t user_callback;
} timer0_interrupt_config_t;

/**
 * @brief timer0 device config
 */
typedef struct timer0_config_t
{
    /**
     * @brief The peripheral config of the timer.
     */
    timer0_peripheral_config_t peripheral;

    /**
     * @brief timer 0 channel a interrupt config
     */
    timer0_interrupt_config_t channel_a_interrupt;

    /**
     * @brief timer 0 channel b interrupt config
     */
    timer0_interrupt_config_t channel_b_interrupt;
} timer0_config_t;

/**
 * @brief timer0, unit 1 configuration
 */
extern timer0_config_t TIMER01_config;

/**
 * @brief timer0, unit 2 configuration
 */
extern timer0_config_t TIMER02_config;
