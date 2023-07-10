#ifndef __IWATCHDOG_H__
#define __IWATCHDOG_H__

// check ddl configuration
#if (DDL_WDT_ENABLE != DDL_ON)
#error "IWatchdog library requires WDT DDL to be enabled"
#endif

#include "Arduino.h"
#include <hc32_ddl.h>

class IWatchdog
{
public:
    /**
     * @brief initialize internal watchdog
     *
     * @param timeout_ms watchdog timeout in milliseconds
     * @param callback watchdog timeout callback. if not provided, MCU will be reset on timeout
     * @return actual timeout in milliseconds
     *
     * @note begin must not be called more than once
     */
    uint32_t begin(const uint32_t timeout_ms = 1000, const voidFuncPtr callback = NULL);

    /**
     * @brief initialize internal watchdog
     *
     * @param divider watchdog clock divider. one of {4, 64, 128, 256, 512, 1024, 2048, 8192}
     * @param count_cycle watchdog count cycle. one of {256, 4096, 16384, 65536}
     * @param callback watchdog timeout callback. if not provided, MCU will be reset on timeout
     *
     * @note begin must not be called more than once
     */
    void begin(const uint32_t divider, const uint32_t count_cycle, const voidFuncPtr callback = NULL);

    /**
     * @brief initialize internal watchdog
     *
     * @param config watchdog configuration
     *
     * @note begin must not be called more than once
     */
    void begin(const stc_wdt_init_t *config);

    /**
     * @brief refresh internal watchdog
     */
    void reload(void);

    /**
     * @brief get internal watchdog counter value
     */
    uint16_t getCounter(void);

private:
    bool initialized = false;
};

extern IWatchdog WDT;
#endif
