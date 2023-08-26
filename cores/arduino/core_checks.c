#include "drivers/panic/panic.h"

#ifdef __CORE_DEBUG
#warning "'__CORE_DEBUG' is defined, HC32 Arduino Core Debug is Enabled"

#ifdef __CORE_DEBUG_SHORT_FILENAMES
#warning "'__CORE_DEBUG_SHORT_FILENAMES' is defined, short filenames are used in debug output"
#endif

#ifdef __CORE_DEBUG_OMIT_PANIC_MESSAGE
#warning "'__CORE_DEBUG_OMIT_PANIC_MESSAGE' is defined, panic message is omitted"
#endif
#endif

#if !defined(__CORE_DEBUG) && PANIC_PRINT_ENABLED
#warning "panic output is enabled, but __CORE_DEBUG mode is disabled! panic output will not be available"
#endif
