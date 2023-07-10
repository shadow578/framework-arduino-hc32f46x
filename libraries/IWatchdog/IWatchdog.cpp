#include "IWatchdog.h"
#include "core_debug.h"

//
// global WDT object
//
IWatchdog WDT;

//
// IWatchdog class implementation
//
void IWatchdog::begin(const stc_wdt_init_t *config)
{
    CORE_ASSERT(config != NULL, "IWatchdog::begin: config is NULL")
    WDT_Init(config);

    // reload watchdog after init
    reload();
}

void IWatchdog::reload(void)
{
    WDT_RefreshCounter();
}

uint16_t IWatchdog::getCounter(void)
{
    return WDT_GetCountValue();
}
