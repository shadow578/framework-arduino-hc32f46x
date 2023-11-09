#include "systick.h"
#include <hc32_ddl.h>

/**
 * @brief system uptime counter, incremented by systick interrupt every 1ms
 */
volatile uint32_t ticks_ms = 0;

extern "C" void SysTick_Handler(void)
{
  __sync_fetch_and_add(&ticks_ms, 1);
}

void systick_init()
{
  stc_clk_freq_t clkFreq;
  CLK_GetClockFreq(&clkFreq);
  SysTick_Config(clkFreq.sysclkFreq / 1000); // tick every 1 ms
}

uint32_t systick_millis()
{
  return ticks_ms;
}

uint32_t systick_micros()
{
  // based on implementation by STM32duino
  // https://github.com/stm32duino/Arduino_Core_STM32/blob/586319c6c2cee268747c8826d93e84b26d1549fd/libraries/SrcWrapper/src/stm32/clock.c#L29

  // read systick counter value and millis counter value
  uint32_t ms = ticks_ms;
  volatile uint32_t ticks = SysTick->VAL;

  // and again
  uint32_t ms2 = ticks_ms;
  volatile uint32_t ticks2 = SysTick->VAL;

  // if ms != ms2, then a systick occurred between the two reads
  // and we must use ms2 and ticks2
  if (ms != ms2)
  {
    ms = ms2;
    ticks = ticks2;
  }

  // get ticks per ms
  const uint32_t ticks_per_ms = SysTick->LOAD + 1;

  // calculate microseconds
  return (ms * 1000) + (((ticks_per_ms - ticks) * 1000) / ticks_per_ms);
}
