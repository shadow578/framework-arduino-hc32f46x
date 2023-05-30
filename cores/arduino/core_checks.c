#include "drivers/panic/panic.h"

#ifdef __CORE_DEBUG
#warning "'__CORE_DEBUG' is defined, HC32 Arduino Core Debug is Enabled"
#endif

#if !defined(__CORE_DEBUG) && PANIC_USART_DEFINED
#warning "Panic output is set, but core debug mode is disabled. Panic messages will not be printed, and faults not be handled."
#endif
