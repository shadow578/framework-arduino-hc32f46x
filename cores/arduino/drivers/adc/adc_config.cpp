#include "adc_config.h"

// configurable ADC resolution
#ifndef CORE_ADC_RESOLUTION
#define CORE_ADC_RESOLUTION 10 // fallback to 10 bit
#endif

#if CORE_ADC_RESOLUTION == 8
#define ADC_RESOLUTION AdcResolution_8Bit
#elif CORE_ADC_RESOLUTION == 10
#define ADC_RESOLUTION AdcResolution_10Bit
#elif CORE_ADC_RESOLUTION == 12
#define ADC_RESOLUTION AdcResolution_12Bit
#else
#error "Invalid ADC resolution. only 8, 10, 12 bit are supported"
#endif

//
// ADC devices
//
adc_device_t ADC1_device = {
    .adc = {
        .register_base = M4_ADC1,
        .clock_id = PWC_FCG3_PERIPH_ADC1,
        .sequence = ADC_SEQ_A,
        .channel_count = ADC1_CH_COUNT,
    },
    .init_params = {
        .resolution = ADC_RESOLUTION,
        .data_alignment = AdcDataAlign_Right,
        .scan_mode = AdcMode_SAOnce, // only sequence A
    },
    .dma = {
        .register_base = M4_DMA1,
        .clock_id = PWC_FCG0_PERIPH_DMA1,
        .channel = DmaCh1,
        .event_source = EVT_ADC1_EOCA,
    },
    .state = {
        .conversion_results = new uint16_t[ADC1_CH_COUNT],
    },
};
