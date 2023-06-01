#include "drivers/panic/panic.h"

#ifdef __CORE_DEBUG
#warning "'__CORE_DEBUG' is defined, HC32 Arduino Core Debug is Enabled"
#endif

#if !defined(__CORE_DEBUG) && PANIC_PRINT_ENABLED
#warning "panic output is enabled, but __CORE_DEBUG mode is disabled! panic output will not be available"
#endif
