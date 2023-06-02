#include "adc.h"
#include "../../yield.h"
#include "../../core_debug.h"

//
// ADC init
//

/**
 * @brief ADC peripheral init
 */
inline void adc_adc_init(const adc_device_t *device)
{
    // enable ADC peripheral clock
    PWC_Fcg3PeriphClockCmd(device->adc.clock_id, Enable);

    // initialize ADC peripheral
    stc_adc_init_t init_device = {
        .enResolution = device->init_params.resolution,
        .enDataAlign = AdcDataAlign_Right,
        .enAutoClear = AdcClren_Enable,
        .enScanMode = AdcMode_SAOnce, //TODO: this depends on the channel in the channel_config...
        .enRschsel = AdcRschsel_Restart,
    };
    ADC_Init(device->adc.register_base, &init_device);

    // setup adc channel
    ADC_AddAdcChannel(device->adc.register_base, &device->init_params.channel_config);

    // ADC will always trigger conversion by software
    ADC_TriggerSrcCmd(device->adc.register_base, device->init_params.channel_config.u8Sequence, Disable);
}

/**
 * @brief ADC DMA transfer init
 */
inline void adc_dma_init(const adc_device_t *device)
{
    // prepare DMA transfer deviceuration to
    // transfer ADCx->DR0-DRn to state.conversion_results
    stc_dma_config_t dma_device = {
        .u16BlockSize = device->state.conversion_results_size,
        .u16TransferCnt = 0,
        .u32SrcAddr = (uint32_t)(&device->adc.register_base->DR0),  // source address is ADCx->DR0
        .u32DesAddr = (uint32_t)(device->state.conversion_results), // destination address is state.conversion_results
        .u16SrcRptSize = device->state.conversion_results_size,
        .u16DesRptSize = device->state.conversion_results_size,
        .stcDmaChCfg = {
            .enSrcInc = AddressIncrease,
            .enDesInc = AddressIncrease,
            .enSrcRptEn = Enable,
            .enDesRptEn = Enable,
            .enSrcNseqEn = Disable,
            .enDesNseqEn = Disable,
            .enTrnWidth = Dma16Bit,
            .enLlpEn = Disable,
            .enIntEn = Disable,
        },
    };

    // enable DMA peripheral clock
    PWC_Fcg0PeriphClockCmd(device->dma.clock_id, Enable);

    // initialize DMA channel and enable
    DMA_InitChannel(device->dma.register_base, device->dma.channel, &dma_device);
    DMA_Cmd(device->dma.register_base, Enable);
    DMA_ChannelCmd(device->dma.register_base, device->dma.channel, Enable);

    // clear DMA transfer complete flag
    DMA_ClearIrqFlag(device->dma.register_base, device->dma.channel, TrnCpltIrq);
    DMA_ClearIrqFlag(device->dma.register_base, device->dma.channel, BlkTrnCpltIrq);

    // AOS is required to trigger DMA transfer, enable AOS peripheral clock
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_AOS, Enable);

    // automatically start DMA transfer when ADC conversion is complete
    DMA_SetTriggerSrc(device->dma.register_base, device->dma.channel, device->dma.event_source);
}

void adc_device_init(const adc_device_t *device)
{
    // adc is set up to trigger conversion by software
    // once conversion is completed, DMA transfer is triggered via AOS
    // adc_wait_for_conversion() waits until the DMA transfer is complete
    adc_adc_init(device);
    adc_dma_init(device);
}

//
// ADC conversion API
//

void adc_start_conversion(const adc_device_t *device)
{
    // clear DMA transfer complete flag
    DMA_ClearIrqFlag(device->dma.register_base, device->dma.channel, BlkTrnCpltIrq);

    // start ADC conversion
    ADC_StartConvert(device->adc.register_base);
}

bool adc_is_conversion_completed(const adc_device_t *device)
{
    // check if DMA transfer complete flag is set
    return DMA_GetIrqFlag(device->dma.register_base, device->dma.channel, BlkTrnCpltIrq) == Set;
}

void adc_await_conversion_completed(const adc_device_t *device)
{
    while (!adc_is_conversion_completed(device))
    {
        yield();
    }
}

uint16_t adc_conversion_read_result(const adc_device_t *device, const uint8_t adc_channel)
{
    // ensure that adc_channel is valid
    CORE_ASSERT(adc_channel >= 0 && adc_channel < device->state.conversion_results_size, "adc_channel is invalid for provided adc device");

    // clear DMA transfer complete flag
    DMA_ClearIrqFlag(device->dma.register_base, device->dma.channel, BlkTrnCpltIrq);

    // read conversion result
    return device->state.conversion_results[adc_channel];
}
