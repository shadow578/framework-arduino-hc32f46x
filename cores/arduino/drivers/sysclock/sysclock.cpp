#include "sysclock.h"
#include <hc32_ddl.h>

system_clock_frequencies_t SYSTEM_CLOCK_FREQUENCIES = {0};

void update_system_clock_frequencies()
{
    stc_clk_freq_t clkFreq;
    CLK_GetClockFreq(&clkFreq);
    SYSTEM_CLOCK_FREQUENCIES.system = clkFreq.sysclkFreq;
    SYSTEM_CLOCK_FREQUENCIES.hclk = clkFreq.hclkFreq;
    SYSTEM_CLOCK_FREQUENCIES.pclk0 = clkFreq.pclk0Freq;
    SYSTEM_CLOCK_FREQUENCIES.pclk1 = clkFreq.pclk1Freq;
    SYSTEM_CLOCK_FREQUENCIES.pclk2 = clkFreq.pclk2Freq;
    SYSTEM_CLOCK_FREQUENCIES.pclk3 = clkFreq.pclk3Freq;
    SYSTEM_CLOCK_FREQUENCIES.pclk4 = clkFreq.pclk4Freq;
    SYSTEM_CLOCK_FREQUENCIES.exclk = clkFreq.exckFreq;

    // update SystemCoreClock of DDL
    SystemCoreClock = SYSTEM_CLOCK_FREQUENCIES.system;
}
