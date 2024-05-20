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


#ifdef __cplusplus
/**
 * @brief en_clk_sysclk_div_factor to integer division factor
 * @param div_factor The clock divider factor
 * @return The integer division factor
 */
constexpr int div_factor_to_n(const en_clk_sysclk_div_factor div_factor)
{
  return 1 << static_cast<int>(div_factor);
}

/**
 * @brief Check if the system clock configuration is valid
 * @param sysclock The input system clock frequency
 * @param hclk_div The HCLK divider value
 * @param pclk0_div The PCLK0 divider value
 * @param pclk1_div The PCLK1 divider value
 * @param pclk2_div The PCLK2 divider value
 * @param pclk3_div The PCLK3 divider value
 * @param pclk4_div The PCLK4 divider value
 * @param exclk_div The EXCLK divider value
 * 
 * @note
 * Rules as per Section 4.4 "Working Clock Specifications" of the HC32F460 Reference Manual.
 * 
 * Maximum clock frequencies as per Table 4-1:
 * HCLK: 200MHz
 * PCLK0: 200MHz
 * PCLK1: 100MHz
 * PCLK2: 60MHz
 * PCLK3: 50MHz
 * PCLK4: 100MHz
 * EXCLK: 100MHz
 *
 * System Clock rules as per Note 1:
 * 1. HCLK >= PCLK1
 * 2. HCLK >= PCLK3
 * 3. HCLK >= PCLK4 
 * 4. HCLK / EXCLK in { 2/1, 4/1, 8/1, 16/1, 32/1 }
 * 5. PCLK0 >= PCLK1
 * 6. PCLK0 >= PCLK3
 * 7. HCLK / PCLK0 in { N/1, 1/N } (TODO: what does this even mean?)
 * 8. PCLK2 / PCLK4 in { 1/4, 1/2, 1/1, 2/1, 4/1, 8/1 }
 */
template <
  uint32_t sysclock, 
  en_clk_sysclk_div_factor_t hclk_div,
  en_clk_sysclk_div_factor_t pclk0_div,
  en_clk_sysclk_div_factor_t pclk1_div,
  en_clk_sysclk_div_factor_t pclk2_div,
  en_clk_sysclk_div_factor_t pclk3_div,
  en_clk_sysclk_div_factor_t pclk4_div,
  en_clk_sysclk_div_factor_t exclk_div
>
constexpr void assert_system_clocks_valid()
{
  // calculate clock frequencies
  const uint32_t hclk = sysclock / div_factor_to_n(hclk_div);
  const uint32_t pclk0 = sysclock / div_factor_to_n(pclk0_div);
  const uint32_t pclk1 = sysclock / div_factor_to_n(pclk1_div);
  const uint32_t pclk2 = sysclock / div_factor_to_n(pclk2_div);
  const uint32_t pclk3 = sysclock / div_factor_to_n(pclk3_div);
  const uint32_t pclk4 = sysclock / div_factor_to_n(pclk4_div);
  const uint32_t exclk = sysclock / div_factor_to_n(exclk_div);

  // validate clock frequencies are within limits
  static_assert(hclk <= 200000000, "HCLK exceeds maximum 200MHz. Consider increasing HCLK divider.");
  static_assert(pclk0 <= 200000000, "PCLK0 exceeds maximum 200MHz. Consider increasing PCLK0 divider.");
  static_assert(pclk1 <= 100000000, "PCLK1 exceeds maximum 100MHz. Consider increasing PCLK1 divider.");
  static_assert(pclk2 <= 60000000, "PCLK2 exceeds maximum 60MHz. Consider increasing PCLK2 divider.");
  static_assert(pclk3 <= 50000000, "PCLK3 exceeds maximum 50MHz. Consider increasing PCLK3 divider.");
  static_assert(pclk4 <= 100000000, "PCLK4 exceeds maximum 100MHz. Consider increasing PCLK4 divider.");
  static_assert(exclk <= 100000000, "EXCLK exceeds maximum 100MHz. Consider increasing EXCLK divider.");

  // validate system clock rules
  static_assert(hclk >= pclk1, "HCLK must be greater than or equal to PCLK1 (1).");
  static_assert(hclk >= pclk3, "HCLK must be greater than or equal to PCLK3 (2).");
  static_assert(hclk >= pclk4, "HCLK must be greater than or equal to PCLK4 (3).");
  static_assert(hclk / exclk == 2 || hclk / exclk == 4 || hclk / exclk == 8 || hclk / exclk == 16 || hclk / exclk == 32, 
    "HCLK / EXCLK must be in { 2/1, 4/1, 8/1, 16/1, 32/1 } (4).");
  static_assert(pclk0 >= pclk1, "PCLK0 must be greater than or equal to PCLK1 (5).");
  static_assert(pclk0 >= pclk3, "PCLK0 must be greater than or equal to PCLK3 (6).");
  static_assert(pclk2 / pclk4 == 1 / 4 || pclk2 / pclk4 == 1 / 2 || pclk2 / pclk4 == 1 / 1 || pclk2 / pclk4 == 2 / 1 || pclk2 / pclk4 == 4 / 1 || pclk2 / pclk4 == 8 / 1, 
    "PCLK2 / PCLK4 must be in { 1/4, 1/2, 1/1, 2/1, 4/1, 8/1 } (8).");
}

/**
 * @brief Calculate the MPLL output clock frequency
 * @param input_clock The input clock frequency
 * @param M The input clock divider
 * @param N The VCO multiplier
 * @param P The output clock divider. Can be either P, Q, or R.
 * @return The MPLL output clock frequency
 */
constexpr uint32_t get_mpll_output_clock(uint32_t input_clock, uint32_t M, uint32_t N, uint32_t P)
{
  return (input_clock / M) * N / P;
}

/**
 * @brief Check if the MPLL configuration is valid
 * 
 * 
 * @note Rules as per Section 4.11.15 "CMU MPLL Configuration Register" of the HC32F460 Reference Manual.
 * 
 * MPLL block diagram, with intermediary clocks (1) = VCO_in, (2) = VCO_out:
 * 
 *  INPUT -> [/ M] -(1)-> [* N] -(2)-|-> [/ P] -> MPLL-P
 *                                   |-> [/ Q] -> MPLL-Q
 *                                   |-> [/ R] -> MPLL-R
 * 
 * 
 * Bounds for M, N, P, Q, R:
 * - M (input clock divider)      :  1 <= M <= 24
 * - N (VCO multiplier)           : 20 <= N <= 480
 * - P (output "P" clock divider) :  2 <= P <= 16
 * - Q (output "Q" clock divider) :  2 <= Q <= 16
 * - R (output "R" clock divider) :  2 <= R <= 16
 * 
 * 
 * VCO frequency bounds:
 * -   1 MHz <= VCO_in <= 25 MHz
 * - 240 MHz <= VCO_out <= 480 MHz
 */
template <
  uint32_t input_clock,
  uint32_t M,
  uint32_t N,
  uint32_t P,
  uint32_t Q,
  uint32_t R
>
constexpr void assert_mpll_config_valid()
{
  // check bounds for M, N, P, Q, R
  static_assert(M >= 1 && M <= 24, "MPLL input clock divider M must be in range [1, 24].");
  static_assert(N >= 20 && N <= 480, "MPLL VCO multiplier N must be in range [20, 480].");
  static_assert(P >= 2 && P <= 16, "MPLL output clock divider P must be in range [2, 16].");
  static_assert(Q >= 2 && Q <= 16, "MPLL output clock divider Q must be in range [2, 16].");
  static_assert(R >= 2 && R <= 16, "MPLL output clock divider R must be in range [2, 16].");

  // calculate VCO frequencies
  const uint32_t vco_in = input_clock / M;
  const uint32_t vco_out = vco_in * N;

  // check bounds for VCO frequencies
  static_assert(vco_in >= 1000000 && vco_in <= 25000000, "MPLL VCO input frequency must be in range [1MHz, 25MHz].");
  static_assert(vco_out >= 240000000 && vco_out <= 480000000, "MPLL VCO output frequency must be in range [240MHz, 480MHz].");
}

#endif // __cplusplus
