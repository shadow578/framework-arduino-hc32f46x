#include "sysclock.h"
#include <hc32_ddl.h>

system_clock_frequencies_t SYSTEM_CLOCK_FREQUENCIES = {0};

//__attribute__((weak))
void sysclock_init(void)
{
    // setup divisors for the different bus clocks
    stc_clk_sysclk_cfg_t sysClkConf = {
        .enHclkDiv = ClkSysclkDiv1,
        .enExclkDiv = ClkSysclkDiv2,
        .enPclk0Div = ClkSysclkDiv1,
        .enPclk1Div = ClkSysclkDiv2,
        .enPclk2Div = ClkSysclkDiv4,
        .enPclk3Div = ClkSysclkDiv4,
        .enPclk4Div = ClkSysclkDiv2,
    };
    CLK_SysClkConfig(&sysClkConf);

    // configure and enable XTAL clock source
    stc_clk_xtal_cfg_t xtalConf = {
        .enFastStartup = Enable,
        .enMode = ClkXtalModeOsc,
        .enDrv = ClkXtalLowDrv,
    };
    CLK_XtalConfig(&xtalConf);
    CLK_XtalCmd(Enable);

    // configure PLL using XTAL clock as source
    stc_clk_mpll_cfg_t pllConf = {
        .PllpDiv = 4u,
        .PllqDiv = 4u,
        .PllrDiv = 4u,
        .plln = 50u,
        .pllmDiv = 1u,
    };
    CLK_SetPllSource(ClkPllSrcXTAL);
    CLK_MpllConfig(&pllConf);

    // enable MPLL and wait until ready
    CLK_MpllCmd(Enable);
    while (CLK_GetFlagStatus(ClkFlagMPLLRdy) != Set)
        ;

    // switch the system clock to MPLL
    CLK_SetSysClkSource(CLKSysSrcMPLL);
}

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
}
