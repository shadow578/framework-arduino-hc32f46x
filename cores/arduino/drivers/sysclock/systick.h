#pragma once
#include <stdint.h>

void systick_init();
uint32_t systick_millis();
uint32_t systick_micros();
