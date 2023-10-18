#include "adc.h"
#include "../../yield.h"
#include "../../core_debug.h"

/**
 * @brief assert that channel id is valid
 * @param device ADC device configuration
 * @param channel ADC channel id to check
 */
#define ASSERT_CHANNEL_ID(device, channel) \
    CORE_ASSERT(channel >= 0 && channel < device->adc.channel_count, "invalid channel id")

/**
 * @brief assert that adc device is initialized
 */
#define ASSERT_INITIALIZED(device, function_name) \
    CORE_ASSERT(device->state.initialized, "ADC device not initialized (calling " function_name ")")

/**
 * @brief adc register to debug name
 */
#define ADC_REG_TO_NAME(reg)  \
    reg == M4_ADC1   ? "ADC1" \
    : reg == M4_ADC2 ? "ADC2" \
                     : "N/A"

/**
 * @brief debug printf for ADC
 */
#define ADC_DEBUG_PRINTF(device, fmt, ...) \
    CORE_DEBUG_PRINTF("[%s] " fmt, ADC_REG_TO_NAME(device->adc.register_base), ##__VA_ARGS__)

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
        .enDataAlign = device->init_params.data_alignment,
        .enAutoClear = AdcClren_Enable,
        .enScanMode = device->init_params.scan_mode,
        .enRschsel = AdcRschsel_Restart,
    };
    ADC_Init(device->adc.register_base, &init_device);

    // ADC will always trigger conversion by software
    ADC_TriggerSrcCmd(device->adc.register_base, device->adc.sequence, Disable);
}

void adc_device_init(adc_device_t *device)
{
    // do nothing if ADC is already initialized
    if (device->state.initialized)
    {
        return;
    }

    // adc is set up to trigger conversion by software
    // adc_wait_for_conversion() waits until the ADC conversion is complete
    adc_adc_init(device);

    // set initialized flag
    device->state.initialized = true;
    ADC_DEBUG_PRINTF(device, "initialized device\n");
}

//
// ADC Channel API
//

inline uint32_t adc_channel_to_mask(const adc_device_t *device, const uint8_t channel)
{
    ASSERT_CHANNEL_ID(device, channel);
    return 1 << channel;
}

void adc_enable_channel(const adc_device_t *device, const uint8_t adc_channel, uint8_t sample_time)
{
    ASSERT_INITIALIZED(device, STRINGIFY(adc_enable_channel));
    ASSERT_CHANNEL_ID(device, adc_channel);
    CORE_ASSERT(sample_time > 0, "adc channel sample_time must be > 0")

    ADC_DEBUG_PRINTF(device, "enable channel %d, sample_time=%d\n", adc_channel, sample_time);
    stc_adc_ch_cfg_t channel_config = {
        .u32Channel = adc_channel_to_mask(device, adc_channel),
        .u8Sequence = device->adc.sequence,
        .pu8SampTime = &sample_time,
    };
    ADC_AddAdcChannel(device->adc.register_base, &channel_config);
}

void adc_disable_channel(const adc_device_t *device, const uint8_t adc_channel)
{
    if (!device->state.initialized)
    {
        // if adc is not initialized, it's safe to assume no channels have been enabled yet
        return;
    }

    ASSERT_CHANNEL_ID(device, adc_channel);

    ADC_DEBUG_PRINTF(device, "disable channel %d\n", adc_channel);
    ADC_DelAdcChannel(device->adc.register_base, adc_channel_to_mask(device, adc_channel));
}

//
// ADC conversion API
//

void adc_start_conversion(const adc_device_t *device)
{
    ASSERT_INITIALIZED(device, STRINGIFY(adc_start_conversion));

    // clear ADC conversion complete flag
    ADC_ClrEocFlag(device->adc.register_base, device->adc.sequence);

    // start ADC conversion
    ADC_StartConvert(device->adc.register_base);
}

bool adc_is_conversion_completed(const adc_device_t *device)
{
    ASSERT_INITIALIZED(device, STRINGIFY(adc_is_conversion_completed));

    // check if ADC conversion complete flag is set
    return ADC_GetEocFlag(device->adc.register_base, device->adc.sequence) == Set;
}

void adc_await_conversion_completed(const adc_device_t *device)
{
    ASSERT_INITIALIZED(device, STRINGIFY(adc_await_conversion_completed));
    while (!adc_is_conversion_completed(device))
    {
        yield();
    }
}

uint16_t adc_conversion_read_result(const adc_device_t *device, const uint8_t adc_channel)
{
    ASSERT_INITIALIZED(device, STRINGIFY(adc_conversion_read_result));
    ASSERT_CHANNEL_ID(device, adc_channel);

    // clear ADC conversion complete flag
    // ADC_ClrEocFlag(device->adc.register_base, device->adc.sequence);

    // read conversion result directly from DRx register
    uint16_t *conversion_results = (uint16_t *)(&device->adc.register_base->DR0);
    return conversion_results[adc_channel];
}
