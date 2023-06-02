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
     */
    void adc_device_init(const adc_device_t *device);

    /**
     * @brief start asynchronous conversion
     * @param device ADC device configuration
     */
    void adc_start_conversion(const adc_device_t *device);

    /**
     * @brief check if conversion is complete
     * @param device ADC device configuration
     * @return true if conversion is complete
     */
    bool adc_is_conversion_completed(const adc_device_t *device);

    /**
     * @brief wait for conversion to complete
     * @param device ADC device configuration
     */
    void adc_await_conversion_completed(const adc_device_t *device);

    /**
     * @brief read asynchronous conversion result
     * @param device ADC device configuration
     * @param adc_channel ADC channel to read
     * @return conversion result
     */
    uint16_t adc_conversion_read_result(const adc_device_t *device, const uint8_t adc_channel);

    /**
     * @brief start adc conversion and wait for result synchronously
     * @param device ADC device configuration
     * @param adc_channel ADC channel to read
     * @return conversion result
     */
    inline uint16_t adc_read_sync(const adc_device_t *device, const uint8_t adc_channel)
    {
        adc_start_conversion(device);
        adc_await_conversion_completed(device);
        return adc_conversion_read_result(device, adc_channel);
    }

    /**
     * @brief initialize all ADC peripherals
     */
    inline void adc_init_all()
    {
        adc_device_init(&ADC1_device);
    }

#ifdef __cplusplus
}
#endif
