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
     * @brief timer peripheral channel.
     */
    en_tim0_channel_t channel;

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
     * @note auto-assigned in lib implementation
     */
    IRQn_Type interrupt_number;

    /**
     * @brief interrupt source
     */
    en_int_src_t interrupt_source;
} timer0_interrupt_config_t;

/**
 * @brief timer0 device config
 */
typedef struct timer0_channel_config_t
{
    /**
     * @brief The peripheral config of the timer.
     */
    timer0_peripheral_config_t peripheral;

    /**
     * @brief timer 0 channel interrupt config
     */
    timer0_interrupt_config_t interrupt;
} timer0_channel_config_t;

/**
 * @brief timer0, unit 1, channel A configuration
 * @note Unit1, Channel A only supports async mode. use Unit2 when possible
 */
extern timer0_channel_config_t TIMER01A_config;

/**
 * @brief timer0, unit 1, channel B configuration
 */
extern timer0_channel_config_t TIMER01B_config;

/**
 * @brief timer0, unit 2, channel A configuration
 */
extern timer0_channel_config_t TIMER02A_config;

/**
 * @brief timer0, unit 2, channel B configuration
 */
extern timer0_channel_config_t TIMER02B_config;
