#pragma once
#include <hc32_ddl.h>
#include "adc_config.h"

#ifdef __cplusplus
extern "C"
{
#endif
    /**
     * @brief ADC peripheral init
     * @param device ADC device configuration
     * @note if the device is initialized, this function will do nothing
     */
    void adc_device_init(adc_device_t *device);

#ifdef __cplusplus
    // TODO: find out what unit sample_time is in...
    /**
     * @brief enable adc conversion channel
     * @param device ADC device configuration
     * @param adc_channel ADC channel to enable
     * @param sample_time ADC sampling time
     * @note requires adc_device_init() to be called first
     */
    void adc_enable_channel(const adc_device_t *device, const uint8_t adc_channel, uint8_t sample_time = 50);
#else
    void adc_enable_channel(const adc_device_t *device, const uint8_t adc_channel, uint8_t sample_time);
#endif

    /**
     * @brief disable adc conversion channel
     * @param device ADC device configuration
     * @param adc_channel ADC channel to disable
     * @note if adc_device_init() was not called before, this function will do nothing
     */
    void adc_disable_channel(const adc_device_t *device, const uint8_t adc_channel);

    /**
     * @brief start asynchronous conversion
     * @param device ADC device configuration
     * @note requires adc_device_init() to be called first
     */
    void adc_start_conversion(const adc_device_t *device);

    /**
     * @brief check if conversion is complete
     * @param device ADC device configuration
     * @return true if conversion is complete
     * @note requires adc_device_init() to be called first
     */
    bool adc_is_conversion_completed(const adc_device_t *device);

    /**
     * @brief wait for conversion to complete
     * @param device ADC device configuration
     * @note requires adc_device_init() to be called first
     */
    void adc_await_conversion_completed(const adc_device_t *device);

    /**
     * @brief read asynchronous conversion result
     * @param device ADC device configuration
     * @param adc_channel ADC channel to read
     * @return conversion result
     * @note requires adc_device_init() to be called first
     */
    uint16_t adc_conversion_read_result(const adc_device_t *device, const uint8_t adc_channel);

    /**
     * @brief start adc conversion and wait for result synchronously
     * @param device ADC device configuration
     * @param adc_channel ADC channel to read
     * @return conversion result
     * @note requires adc_device_init() to be called first
     */
    inline uint16_t adc_read_sync(const adc_device_t *device, const uint8_t adc_channel)
    {
        adc_start_conversion(device);
        adc_await_conversion_completed(device);
        return adc_conversion_read_result(device, adc_channel);
    }

#ifdef __cplusplus
}
#endif
