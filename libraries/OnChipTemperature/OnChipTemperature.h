#pragma once

// check ddl configuration
#if (DDL_OTS_ENABLE != DDL_ON)
#error "OTS library requires OTS DDL to be enabled"
#endif

#if (DDL_PWC_ENABLE != DDL_ON)
#error "OTS library requires PWC DDL to be enabled"
#endif

#if (DDL_CLK_ENABLE != DDL_ON)
#error "OTS library requires CLK DDL to be enabled"
#endif

#include "Arduino.h"
#include <hc32_ddl.h>

/**
 * @brief On-Chip Temperature Sensor (OTS) class
 */
class OnChipTemperature
{
public:
    /**
     * @brief initialize OTS with default configuration
     * @note to achive better accuracy, use .begin(const stc_ots_init_t *config) with calibrated K and M values
     * @note default configuration is XTAL clock, no auto-off, K and M taken from examples
     */
    void begin();

    /**
     * @brief initialize OTS with custom configuration
     * @param config OTS configuration
     */
    void begin(const stc_ots_init_t *config);

    /**
     * @brief read temperature from OTS
     * @param temperature temperature value
     * @return true if temperature was read successfully
     * @note if reading faster than minimum read delta, will return false
     */
    bool read(float &temperature);

    /**
     * @brief set minimum time between OTS reads
     * @param minimumReadDeltaMillis minimum time between OTS reads, in milliseconds
     * @note if this value is 0, OTS will be read every time
     * @note .read() will return false if minimum time has not passed
     */
    void setMinimumReadDeltaMillis(uint32_t minimumReadDeltaMillis)
    {
        this->minimumReadDeltaMillis = minimumReadDeltaMillis;
    }

    /**
     * @brief get minimum time between OTS reads
     * @return minimum time between OTS reads, in milliseconds
     */
    uint32_t getMinimumReadDeltaMillis()
    {
        return this->minimumReadDeltaMillis;
    }

private:
    /**
     * internal OTS read function, using OTS_Polling
     */
    bool read_internal(float &temperature);

    /**
     * minimum time between reads, in milliseconds
     */
    uint32_t minimumReadDeltaMillis = 0;

    /**
     * value of millis() at time of last real read
     */
    uint32_t lastReadMillis = 0;

    /**
     * should we read the temperature from OTS?
     * @param now value of millis() at time of check
     */
    bool shouldRead(uint32_t &now);
};

extern OnChipTemperature ChipTemperature;
