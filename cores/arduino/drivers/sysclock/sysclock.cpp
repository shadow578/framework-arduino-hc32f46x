#include "sysclock.h"
#include <hc32_ddl.h>

__attribute__((weak)) void sysclock_init(void)
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

//
// clock source getters
//
#define DEF_CLOCK_SOURCE_GETTER(name, src) \
    uint32_t get_##name##_clock()          \
    {                                      \
        stc_clk_freq_t clkInfo;            \
        CLK_GetClockFreq(&clkInfo);        \
        return clkInfo.src##Freq;          \
    }

DEF_CLOCK_SOURCE_GETTER(sysclk, sysclk)
DEF_CLOCK_SOURCE_GETTER(hclk, hclk)
DEF_CLOCK_SOURCE_GETTER(pclk0, pclk0)
DEF_CLOCK_SOURCE_GETTER(pclk1, pclk1)
DEF_CLOCK_SOURCE_GETTER(pclk2, pclk2)
DEF_CLOCK_SOURCE_GETTER(pclk3, pclk3)
DEF_CLOCK_SOURCE_GETTER(pclk4, pclk4)
DEF_CLOCK_SOURCE_GETTER(exclk, exck)
