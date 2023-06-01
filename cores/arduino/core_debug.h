#ifndef _CORE_DEBUG_H
#define _CORE_DEBUG_H

#ifdef __CORE_DEBUG

#include <stdio.h>

// allow user to re-define the debug macros with custom ones
// user macros are only active if __CORE_DEBUG is defined
#ifndef CORE_DEBUG_INIT
#define CORE_DEBUG_INIT()
#endif

#ifndef CORE_DEBUG_PRINTF
#define CORE_DEBUG_PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)
#endif

#else // !__CORE_DEBUG

// no debug, dummy macros and user-macros are undefined
#undef CORE_DEBUG_INIT
#undef CORE_DEBUG_PRINTF
#define CORE_DEBUG_PRINTF(fmt, ...)
#define CORE_DEBUG_INIT()
#endif // __CORE_DEBUG

#endif // _CORE_DEBUG_H