#include "adc_config.h"

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

        //TODO: adc resolution should be configurable at compile time
        .resolution = AdcResolution_10Bit,
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
