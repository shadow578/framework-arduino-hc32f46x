#include "systick.h"
#include <hc32_ddl.h>

volatile uint32_t uptime = 0;

extern "C" void SysTick_IrqHandler(void)
{
    uptime++;
}

void systick_init()
{
    stc_clk_freq_t clkFreq;
    CLK_GetClockFreq(&clkFreq);
    SysTick_Config(clkFreq.sysclkFreq / TICKS_PER_SECOND);
}

uint32_t systick_millis()
{
#ifdef ENABLE_MICROS
    return uptime / 1000;
#else
    return uptime;
#endif
}

#ifdef ENABLE_MICROS
uint32_t systick_micros()
{
    return uptime;
}
#endif
