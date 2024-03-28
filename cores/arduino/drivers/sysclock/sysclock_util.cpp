/**
 * HC32F460 power mode switching logic.
 * refer to HC32F460 user manual, Section 5.4.1 (esp. Table 5-6) for more information
 */

#include "sysclock_util.h"
#include "../core_debug.h"

typedef enum power_mode_t
{
    /**
     * highest performance mode
     * 0 < sysclock <= 200 MHz
     */
    PowerMode_HighPerformance = 1,

    /**
     * high speed mode
     * 0 < sysclock <= 168 MHz
     */
    PowerMode_HighSpeed = 2,

    /**
     * low speed mode
     * 0 < sysclock <= 8 MHz
     */
    PowerMode_LowSpeed = 4,
} power_mode_t;

#define FREQ_ULTRA_HIGH_SPEED 200000000
#define FREQ_HIGH_SPEED 168000000
#define FREQ_ULTRA_LOW_SPEED 8000000

/**
 * system clock for calculations.
 * DDL examples use SystemCoreClock
 */
#define SYSTEM_CLOCK (SYSTEM_CLOCK_FREQUENCIES.system)
#define SYSTEM_CLOCK_UPDATE() update_system_clock_frequencies()

/**
 * @brief get power mode for a given frequency
 */
inline power_mode_t get_power_mode_for_freq(uint32_t freq)
{
    if (freq <= FREQ_ULTRA_LOW_SPEED)
    {
        // 0 < freq <= 8 MHz
        return PowerMode_LowSpeed;
    }
    else if (freq <= FREQ_HIGH_SPEED)
    {
        // 8 < freq <= 168 MHz
        return PowerMode_HighSpeed;
    }
    else
    {
        // freq > 168 MHz
        return PowerMode_HighPerformance;
    }
}

void power_mode_update(uint32_t newSystemClock, bool preClockSwitchOver)
{
    // get current and future power mode
    SYSTEM_CLOCK_UPDATE();
    power_mode_t currentPowerMode = get_power_mode_for_freq(SYSTEM_CLOCK);
    power_mode_t futurePowerMode = get_power_mode_for_freq(newSystemClock);

    // do nothing if power mode is not changing
    if (currentPowerMode == futurePowerMode)
    {
        return;
    }

    // resolve switch function to call
    en_result_t (*sw_func)() = nullptr;
#define PWR_MODE_CHANGE(current, future) ((current << 4) | future)
    switch (PWR_MODE_CHANGE(currentPowerMode, futurePowerMode))
    {
    case PWR_MODE_CHANGE(PowerMode_LowSpeed, PowerMode_HighSpeed):
        // Low Speed -> High Speed, available before clock switch
        // refer to Section 5.4.1-2
        if (preClockSwitchOver)
        {
            sw_func = &PWC_LS2HS;
        }
        break;
    case PWR_MODE_CHANGE(PowerMode_HighSpeed, PowerMode_HighPerformance):
        // High Speed -> High Performance, available before clock switch
        // refer to Section 5.4.1-3
        if (preClockSwitchOver)
        {
            sw_func = &PWC_HS2HP;
        }
        break;
    case PWR_MODE_CHANGE(PowerMode_LowSpeed, PowerMode_HighPerformance):
        // Low Speed -> High Performance, available before clock switch
        // refer to Section 5.4.1-5
        if (preClockSwitchOver)
        {
            sw_func = &PWC_LS2HP;
        }
        break;

    case PWR_MODE_CHANGE(PowerMode_HighSpeed, PowerMode_LowSpeed):
        // High Speed -> Low Speed, available after clock switch
        // refer to Section 5.4.1-1
        if (!preClockSwitchOver)
        {
            sw_func = &PWC_HS2LS;
        }
        break;
    case PWR_MODE_CHANGE(PowerMode_HighPerformance, PowerMode_HighSpeed):
        // High Performance -> High Speed, available after clock switch
        // refer to Section 5.4.1-4
        if (!preClockSwitchOver)
        {
            sw_func = &PWC_HP2HS;
        }
        break;
    case PWR_MODE_CHANGE(PowerMode_HighPerformance, PowerMode_LowSpeed):
        // High Performance -> Low Speed, available after clock switch
        // refer to Section 5.4.1-6
        if (!preClockSwitchOver)
        {
            sw_func = &PWC_HP2LS;
        }
        break;
    default:
        // unsupported combination
        CORE_DEBUG_PRINTF("unsupported power mode change: %d -> %d\n", currentPowerMode, futurePowerMode);
        return;
    }

    // call switch function (null if no switch is needed)
    if (sw_func != nullptr)
    {
        if (Ok != sw_func())
        {
            // switch failed, panic
            panic("power mode switch failed");
        }
    }

    if (!preClockSwitchOver)
    {
        // update GPIO wait cycles
        // see reference manual Section 9.4.8 "Public Control Register (PCCR)"
        uint8_t newRDWT;
        if (newSystemClock <= 42000000)
        {
            // <= 42 MHz
            newRDWT = 0;
        }
        else if (newSystemClock < 84000000)
        {
            // 42 - 84 MHz
            newRDWT = 1;
        }
        else if (newSystemClock < 126000000)
        {
            // 84 - 126 MHz
            newRDWT = 2;
        }
        else
        {
            // freq >= 126 MHz
            newRDWT = 3;
        }

        // update register if different
        if (M4_PORT->PCCR_f.RDWT != newRDWT)
        {
            PORT_Unlock();
            M4_PORT->PCCR_f.RDWT = newRDWT;
            PORT_Lock();
        }
    }
}
