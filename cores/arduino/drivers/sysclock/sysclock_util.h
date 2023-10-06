#pragma once
#include "sysclock.h"
#include <hc32_ddl.h>

//
// XTAL / MPLL util
//

/**
 * @brief init external XTAL clock source
 */
inline void sysclock_configure_xtal()
{
    // configure and enable XTAL clock source
    stc_clk_xtal_cfg_t xtalConf = {
        .enFastStartup = Enable,
        .enMode = ClkXtalModeOsc,
        .enDrv = ClkXtalLowDrv,
    };
    CLK_XtalConfig(&xtalConf);
    CLK_XtalCmd(Enable);
}

/**
 * @brief init internal high-speed RC clock source to 16 MHz
 */
inline void sysclock_configure_hrc()
{
    CLK_HrcCmd(Enable);
}

/**
 * @brief init main PLL (MPLL), with XTAL as source
 * @param pllSource PLL source (XTAL or HRC)
 * @param pllConfig PLL configuration. see notes below.
 *
 * @note
 * the HC32F460 has two PLLs, MPLL and UPLL, each with 3 "channels" (P,Q,R).
 *
 * the PLLs can be clock sources for different units:
 * - MPLL
 *   - P: sysclock (additional dividers for HCLK, PCLKn, ...)
 *   - Q: ADC (PCLK2, PCLK4), USB (UCLK), I2Sn (I2SnCLK)
 *   - R: "
 * - UPLL
 *   - P: ADC (PCLK2, PCLK4), USB (UCLK), I2Sn (I2SnCLK)
 *   - Q: "
 *   - R: "
 * (refer to HC32F460 user manual, Section 4.2.1, Figure 4-1)
 *
 * PLLs can be sourced from either XTAL or HRC (internal high-speed RC clock)
 *
 * PLL output clock is calculated as ((clock_source / M) * N) / [P|Q|R].
 * - M is pre-divider
 * - N is pre-multiplier
 * - [P|Q|R] is channel divider
 *
 * XTAL can be between 4 and 25 MHz, default is 8 MHz (if using a different XTAL, set XTAL_VALUE)
 */
inline void sysclock_configure_mpll(
    en_clk_pll_source_t pllSource,
    const stc_clk_mpll_cfg_t *pllConfig)
{
    // configure PLL
    CLK_SetPllSource(pllSource);
    CLK_MpllConfig(pllConfig);

    // enable MPLL and wait until ready
    CLK_MpllCmd(Enable);
    while (CLK_GetFlagStatus(ClkFlagMPLLRdy) != Set)
        ;
}

/**
 * @brief set dividers for HCLK, PCLKn, ...
 * @param sysClkConf clock configuration
 *
 * @note
 * different clock max frequencies:
 * - HCLK:  200 MHz
 * - EXCLK: 100 MHz
 * - PCLK0: 200 MHz
 * - PCLK1: 100 MHz
 * - PCLK2: 60 MHz
 * - PCLK3: 50 MHz
 * - PCLK4: 100 MHz
 *
 * (refer to HC32F460 user manual, Section 4.4, Table 4-1)
 */
inline void sysclock_set_clock_dividers(const stc_clk_sysclk_cfg_t *sysClkConf)
{
    CLK_SysClkConfig(sysClkConf);
}

//
// Flash / SRAM wait cycle utils
//

/**
 * @brief setup SRAM wait cycles
 * @param readWaitCycles number of wait cycles for read access. default is safe for up to 200 MHz HCLK
 * @param writeWaitCycles number of wait cycles for write access. default is safe for up to 200 MHz HCLK
 *
 * @note
 * at higher clocks, the SRAM wait cycles must be configured
 * - 0 < HCLK <= 100 MHz, 0 wait cycles are valid
 * - 0 < HCLK <= 200 MHz, min. 1 wait cycle is required
 *
 * (refer to HC32F460 user manual, Section 8.1, Table 8-1)
 *
 * @note this function also configures ECC and parity to reset on any error
 *
 * @note set SRAM wait cycles before switching sysclk
 */
inline void sysclock_configure_sram_wait_cycles(
    en_sram_rw_cycle_t readWaitCycles = SramCycle2,
    en_sram_rw_cycle_t writeWaitCycles = SramCycle2)
{
    stc_sram_config_t config = {
        // all SRAM units
        .u8SramIdx = Sram12Idx | Sram3Idx | SramHsIdx | SramRetIdx,

        // read/write wait cycles
        .enSramRC = readWaitCycles,
        .enSramWC = writeWaitCycles,

        // ECC and parity configuration
        // (use ECC and parity, reset on any error)
        .enSramEccMode = EccMode3,
        .enSramEccOp = SramReset,
        .enSramPyOp = SramReset,
    };
    SRAM_Init(&config);
}

/**
 * @brief setup flash wait cycles and cache to safe values for HCLK <= 200 MHz
 *
 * @note
 * this function sets safe values for HCLK = 200 MHz, but should be safe to use for lower clocks as well
 *
 * @note
 * refer to HC32F460 user manual, Section 7.4, Table 7-1 for values to use
 *
 * @note set flash wait cycles before switching sysclk
 */
inline void sysclock_configure_flash_wait_cycles()
{
    EFM_Unlock();

    // ultra-low-power mode only supported for HCLK <= 2 MHz
    EFM_SetReadMode(NormalRead);

    // 168 < HCLK <= 200 MHz, 5 wait cycles are required
    EFM_SetLatency(EFM_LATENCY_5);

    // enable instruction cache
    EFM_InstructionCacheCmd(Enable);
    EFM_Lock();
}

//
// Power Mode utlis
//

/**
 * @brief update power mode to highest performance allowed for new system clock
 * @param newSystemClock new system clock frequency (SYSTEM_CLOCK_FREQUENCIES.system) after switch
 * @param preClockSwitchOver true if this is called before switching the clock, false if called after
 *
 * @note this function must be called both before and after switching the clock
 */
void power_mode_update(uint32_t newSystemClock, bool preClockSwitchOver);

#define power_mode_update_pre(newSystemClock) power_mode_update(newSystemClock, true)
#define power_mode_update_post(newSystemClock) power_mode_update(newSystemClock, false)

//
// restore defaults
//

/**
 * @brief restore the default clock source and frequencies
 *
 * @note
 * default clock source is MCR (8 MHz) (refer to HC32F460 user manual, Section 4.8)
 *
 * @note
 * default clock dividers are as follows:
 * - HCLK:  1 (8 MHz)
 * - EXCLK: 1 (8 MHz)
 * - PCLK0: 1 (8 MHz)
 * - PCLK1: 1 (8 MHz)
 * - PCLK2: 1 (8 MHz)
 * - PCLK3: 1 (8 MHz)
 * - PCLK4: 1 (8 MHz)
 *
 * (refer to HC32F460 user manual, Section 4.11.21, register defaults)
 */
inline void sysclock_restore_default_clocks()
{
    // start MRC clock (should be running already, but make sure)
    CLK_MrcCmd(Enable);

    // update performance mode
    power_mode_update_pre(8000000);

    // switch to MCR (8 MHz)
    CLK_SetSysClkSource(ClkSysSrcMRC);

    // set clock dividers
    stc_clk_sysclk_cfg_t div = {
        .enHclkDiv = ClkSysclkDiv1,
        .enExclkDiv = ClkSysclkDiv1,
        .enPclk0Div = ClkSysclkDiv1,
        .enPclk1Div = ClkSysclkDiv1,
        .enPclk2Div = ClkSysclkDiv1,
        .enPclk3Div = ClkSysclkDiv1,
        .enPclk4Div = ClkSysclkDiv1,
    };
    sysclock_set_clock_dividers(&div);

    // update performance mode
    power_mode_update_post(8000000);
}
