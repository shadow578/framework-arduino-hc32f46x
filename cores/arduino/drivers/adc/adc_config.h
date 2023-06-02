#pragma once
#include <hc32_ddl.h>

/**
 * @brief ADC peripheral configuration
 */
typedef struct adc_peripheral_config_t
{
    /**
     * @brief ADC peripheral register base address
     */
    M4_ADC_TypeDef *register_base;

    /**
     * @brief ADC peripheral clock id
     * @note in FCG3
     */
    uint32_t clock_id;

    /**
     * @brief adc sequence selection
     * @note eg. ADC_SEQ_A
     */
    uint8_t sequence;

    /**
     * @brief ADC channel count
     */
    uint16_t channel_count;
} adc_peripheral_config_t;

/**
 * @brief ADC init parameters
 */
typedef struct adc_init_params_t
{
    /**
     * @brief ADC conversion resolution
     */
    en_adc_resolution_t resolution;

    /**
     * @brief ADC sample data alignment
     */
    en_adc_data_align_t data_alignment;

    /**
     * @brief ADC scan mode
     * @note must include sequence set in peripheral config
     */
    en_adc_scan_mode_t scan_mode;

} adc_init_params_t;

/**
 * @brief ADC DMA configuration
 */
typedef struct adc_dma_config_t
{
    /**
     * @brief DMA peripheral register base address
     */
    M4_DMA_TypeDef *register_base;

    /**
     * @brief DMA peripheral clock id
     * @note in FCG0
     */
    uint32_t clock_id;

    /**
     * @brief DMA channel
     */
    en_dma_channel_t channel;

    /**
     * @brief DMA start event source
     * @note should be set to EVT_ADCx_EOCA (ADC end of conversion)
     */
    en_event_src_t event_source;
} adc_dma_config_t;

/**
 * @brief ADC runtime state
 */
typedef struct adc_runtime_state_t
{
    /**
     * @brief was the adc already initialized?
     */
    bool initialized;

    /**
     * @brief adc conversion results array
     * @note index == adc channel number
     * @note lenght == adc channel count (see ADCx_device.adc.channel_count)
     */
    uint16_t *conversion_results;
} adc_runtime_state_t;

/**
 * @brief ADC device configuration
 */
typedef struct adc_device_t
{
    /**
     * @brief ADC peripheral configuration
     */
    adc_peripheral_config_t adc;

    /**
     * @brief ADC init parameters
     */
    adc_init_params_t init_params;

    /**
     * @brief ADC DMA configuration
     */
    adc_dma_config_t dma;

    /**
     * @brief ADC runtime state
     */
    adc_runtime_state_t state;
} adc_device_t;

//
// ADC devices
//
extern adc_device_t ADC1_device;
