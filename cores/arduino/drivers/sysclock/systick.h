#pragma once
#include <stdint.h>

// cannot have REQUIRE_MICROS and STUB_MICROS at the same time
#if defined(REQUIRE_MICROS) && defined(STUB_MICROS)
#error "Cannot have REQUIRE_MICROS and STUB_MICROS at the same time"
#endif

// set systick to only millisecond resolution if micros are not required
#ifdef REQUIRE_MICROS
#define TICKS_PER_SECOND 1000000ul
#else
#define TICKS_PER_SECOND 1000ul
#endif

void systick_init();
uint32_t millis();


#ifdef REQUIRE_MICROS
uint32_t micros();
#endif

// allow stubbing the micros() function
#ifdef STUB_MICROS
#include <hc32_ddl.h>
inline uint32_t micros()
{
    DDL_ASSERT(0);
    return 0;
}
#endif
