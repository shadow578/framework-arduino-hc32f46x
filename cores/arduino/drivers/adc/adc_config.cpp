#include "adc_config.h"

//
// ADC devices
//
adc_device_t ADC1_device = {
    .adc = {
        .register_base = M4_ADC1,
        .clock_id = PWC_FCG3_PERIPH_ADC1,
    },
    //TODO: ideally, init_params would be configurable, as u32Channel effectively selects which pins are enabled for ADC
    .init_params = {
        .channel_config = {
            .u32Channel = (ADC1_CH14 | ADC1_CH15),
            .u8Sequence = ADC_SEQ_A,
            .pu8SampTime = (uint8_t[]){50, 50},
        },
        //TODO: adc resolution should be configurable at compile time
        .resolution = AdcResolution_10Bit,
    },
    .dma = {
        .register_base = M4_DMA1,
        .clock_id = PWC_FCG0_PERIPH_DMA1,
        .channel = DmaCh1,
        .event_source = EVT_ADC1_EOCA,
    },
    .state = {
        .conversion_results = new uint16_t[ADC1_CH_COUNT],
        .conversion_results_size = ADC1_CH_COUNT,
    },
};
