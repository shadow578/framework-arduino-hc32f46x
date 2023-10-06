#pragma once
#include <stdint.h>

// set systick to only millisecond resolution if micros are not required
#ifdef ENABLE_MICROS
#define TICKS_PER_SECOND 1000000ul
#else
#define TICKS_PER_SECOND 1000ul
#endif

void systick_init();
uint32_t systick_millis();

#ifdef ENABLE_MICROS
uint32_t systick_micros();
#endif
