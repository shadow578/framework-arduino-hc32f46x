#include "IWatchdog.h"

//
// global WDT object
//
IWatchdog WDT;

//
// IWatchdog class implementation
//
void IWatchdog::begin(const stc_wdt_init_t *config)
{
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
