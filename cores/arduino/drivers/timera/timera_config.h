#pragma once
#include <hc32_ddl.h>

#define TIMERA_UNIT1 M4_TMRA1
#define TIMERA_UNIT1_CLOCK PWC_FCG2_PERIPH_TIMA1
#define TIMERA_UNIT1_OVERFLOW_INT INT_TMRA1_OVF

/* TIMERA channel 1 Port/Pin definition */
#define TIMERA_UNIT1_CH_BL TimeraCh6

/**
 * @brief timerA peripheral config
 */
struct timera_peripheral_config_t
{
    /**
     * @brief The base address of the timer peripheral.
     */
    M4_TMRA_TypeDef *register_base;

    /**
     * @brief the clock id of the timer peripheral.
     * @note in FCG2
     */
    uint32_t clock_id;
};

/**
 * @brief timerA interrupt config
 */
typedef struct timera_interrupt_config_t
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
} timera_interrupt_config_t;

/**
 * @brief timerA unit state
 */
typedef struct timera_runtime_state_t
{
    /**
     * @brief TimerA unit base init configuration.
     * @note NULL if not initialized.
     */
    stc_timera_base_init_t *base_init;

    /**
     * @brief TimerA unit active channels
     * @note use TIMERA_STATE_ACTIVE_CHANNEL_BIT(ch) to get bit positions
     */
    uint8_t active_channels;
} timera_runtime_state_t;

/**
 * @brief get bit position of channel in active_channels bitfield
 * @param ch the channel
 * @return the bit position
 * @note ch must be 1 <= ch <= 8
*/
#define TIMERA_STATE_ACTIVE_CHANNEL_BIT(ch) (1 << (ch - 1))

/**
 * @brief timerA device config
 */
typedef struct timera_config_t
{
    /**
     * @brief timerA peripheral config
     */
    timera_peripheral_config_t peripheral;

    /**
     * @brief timerA Overflow (OVF) interrupt config
     */
    timera_interrupt_config_t overflow_interrupt;

    /**
     * @brief timerA Underflow (UDF) interrupt config
     */
    timera_interrupt_config_t underflow_interrupt;

    /**
     * @brief timerA Compare (CMP) interrupt config
     */
    timera_interrupt_config_t compare_interrupt;

    /**
     * @brief timerA unit state
     */
    timera_runtime_state_t state;
} timera_config_t;

/**
 * @brief TimerA Unit 1 configuration
 */
extern timera_config_t TIMERA1_config;

/**
 * @brief TimerA Unit 2 configuration
 */
extern timera_config_t TIMERA2_config;

/**
 * @brief TimerA Unit 3 configuration
 */
extern timera_config_t TIMERA3_config;

/**
 * @brief TimerA Unit 4 configuration
 */
extern timera_config_t TIMERA4_config;

/**
 * @brief TimerA Unit 5 configuration
 */
extern timera_config_t TIMERA5_config;

/**
 * @brief TimerA Unit 6 configuration
 */
extern timera_config_t TIMERA6_config;
