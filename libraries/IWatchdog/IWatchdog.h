#ifndef __IWATCHDOG_H__
#define __IWATCHDOG_H__
#include "Arduino.h"
#include <hc32_ddl.h>

#if (DDL_WDT_ENABLE != DDL_ON)
#error "IWatchdog library requires WDT DDL to be enabled"
#endif

class IWatchdog
{
public:
    /**
     * @brief initialize internal watchdog
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
};

extern IWatchdog WDT;
#endif
