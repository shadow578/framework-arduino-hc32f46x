#pragma once
// is a output for panic messages defined?
#define PANIC_USART_DEFINED          \
    (defined(PANIC_USART1_TX_PIN) || \
     defined(PANIC_USART2_TX_PIN) || \
     defined(PANIC_USART3_TX_PIN) || \
     defined(PANIC_USART4_TX_PIN))

// panic and fault handlers are only enabled in core debug mode, and if a output is defined
#if defined(__CORE_DEBUG) && PANIC_USART_DEFINED
#define ENABLE_PANIC_AND_FAULT_HANDLERS
#endif

#ifdef ENABLE_PANIC_AND_FAULT_HANDLERS

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif
    //
    // sprintf helpers for panic handlers
    //
    typedef struct panic_sprintf_info_t
    {
        char *buffer;
        size_t buffer_size;
        size_t offset;
    } panic_sprintf_info_t;

    /**
     * @brief start sprintf helper for panic handler
     */
    void panic_start_sprintf(size_t buffer_size, panic_sprintf_info_t *info);

    /**
     * @brief sprintf helper for panic handlers
     * @note formatted string is appended to buffer
     */
    void panic_sprintf(panic_sprintf_info_t *info, const char *fmt, ...);

    //
    // panic handler
    //

    /**
     * @brief core panic handler
     * @param message message to print before entering infinite loop
     * @note this function never returns
     */
    __attribute__((noreturn)) void panic(const char *message);
#ifdef __cplusplus
}
#endif
#endif // ENABLE_PANIC_AND_FAULT_HANDLERS
