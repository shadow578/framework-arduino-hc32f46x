#include "drivers/panic/panic.h"

#ifdef __CORE_DEBUG
#warning "'__CORE_DEBUG' is defined, HC32 Arduino Core Debug is Enabled"

#if !ENABLE_PANIC_HANDLER
#warning "panic handler is disabled, but __CORE_DEBUG mode is enabled! panic output will not be available"
#endif
#endif // __CORE_DEBUG

#if PANIC_ENABLE
#if defined(PANIC_USART1_TX_PIN) || defined(PANIC_USART2_TX_PIN) || defined(PANIC_USART3_TX_PIN) || defined(PANIC_USART3_TX_PIN)
#warning "PANIC_ENABLE defined alongside PANIC_USARTn_TX_PIN. please disable one of them"
#endif
#endif // PANIC_ENABLE

#if ENABLE_PANIC_HANDLER
#ifdef __PANIC_SHORT_FILENAMES
#warning "'__PANIC_SHORT_FILENAMES' is defined, short filenames are used in debug output"
#endif

#ifdef __OMIT_PANIC_MESSAGE
#warning "'__OMIT_PANIC_MESSAGE' is defined, panic message is omitted"
#endif
#endif // ENABLE_PANIC_HANDLER

#ifdef CORE_DISABLE_FAULT_HANDLER
#warning "'CORE_DISABLE_FAULT_HANDLER' is defined, core-internal fault handler is disabled"
#endif

#ifdef ENABLE_MICROS
#warning "ENABLE_MICROS is deprecated. micros() is always available"
#endif
