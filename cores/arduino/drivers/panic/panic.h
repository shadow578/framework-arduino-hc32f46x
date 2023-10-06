#pragma once
#include <stdlib.h>

// only enable panic print if at least one output is defined
#define PANIC_PRINT_ENABLED          \
    (defined(PANIC_USART1_TX_PIN) || \
     defined(PANIC_USART2_TX_PIN) || \
     defined(PANIC_USART3_TX_PIN) || \
     defined(PANIC_USART4_TX_PIN))

/**
 * cases:
 * !__CORE_DEBUG + !PANIC_PRINT_ENABLED : all stubbed
 * !__CORE_DEBUG +  PANIC_PRINT_ENABLED : all stubbed
 *  __CORE_DEBUG + !PANIC_PRINT_ENABLED : panic_begin() and panic_printf() are stubbed, panic() calls panic_end(), msg is ignored
 *  __CORE_DEBUG +  PANIC_PRINT_ENABLED : all implemented
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __CORE_DEBUG

    /**
     * @brief finalize panic, halt or reset MCU
     * @note this function never returns
     */
    __attribute__((noreturn)) void panic_end(void);

#ifdef PANIC_PRINT_ENABLED
    // add full implementation of panic_begin(), panic_printf(), and panic()

    /**
     * @brief initialize panic output, if enabled
     */
    void panic_begin(void);

    /**
     * @brief print message to panic output, if enabled
     * @param fmt format string
     * @param ... format arguments
     * @return number of characters printed
     * @note this function is only available if panic output is enabled and panic_begin() was called
     * @note maximum number of characters printed is 256
     */
    size_t panic_printf(const char *fmt, ...);

    /**
     * @brief internal panic handler
     * @param message message to print before panicing. may be set to NULL to omit message
     */
    inline void _panic(const char *message)
    {
        if (message != NULL)
        {
            panic_begin();
            panic_printf(message);
        }

        panic_end();
    }

    // define file name macro for panic() to use
    #if defined(__CORE_DEBUG_SHORT_FILENAMES) && defined(__SOURCE_FILE_NAME__) 
    #define PANIC_FILE_NAME __SOURCE_FILE_NAME__ // only filename
    #else
    // no short filenames, or __SOURCE_FILE_NAME__ not available
    #if defined(__CORE_DEBUG_SHORT_FILENAMES)
    #warning "__CORE_DEBUG_SHORT_FILENAMES is defined, but __SOURCE_FILE_NAME__ is not available."
    #endif

    #define PANIC_FILE_NAME __FILE__
    #endif

    #define PANIC_LINE_NUMBER_STR STRINGIFY(__LINE__)

    /**
     * @brief core panic handler
     * @param message message to print before panicing. may be set to NULL to omit message
     * @note automatically adds file and line number to message
     */
    #ifdef __CORE_DEBUG_OMIT_PANIC_MESSAGE
    #define panic(msg) _panic(PANIC_FILE_NAME "l" PANIC_LINE_NUMBER_STR)
    #else
    #define panic(msg) _panic("[" PANIC_FILE_NAME " l" PANIC_LINE_NUMBER_STR "]"  msg)
    #endif

#else // !PANIC_PRINT_ENABLED
    // if panic print is not enabled, stub panic_begin() and panic_printf()
    // and redirect panic() to panic_end()

    #define panic_begin()
    #define panic_printf(fmt, ...)
    #define panic(msg) panic_end()

#endif // PANIC_PRINT_ENABLED
#else  // !__CORE_DEBUG
    // if core is not in debug mode, stub all panic functions

    #define panic_begin()
    #define panic_printf(fmt, ...)
    #define panic_end()
    #define panic(msg)
#endif // __CORE_DEBUG

#ifdef __cplusplus
}
#endif
