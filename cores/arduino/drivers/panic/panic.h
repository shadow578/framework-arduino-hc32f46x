#pragma once
// only enable panic print if at least one output is defined
#define PANIC_PRINT_ENABLED          \
    (defined(PANIC_USART1_TX_PIN) || \
     defined(PANIC_USART2_TX_PIN) || \
     defined(PANIC_USART3_TX_PIN) || \
     defined(PANIC_USART4_TX_PIN))

// panic is only needed if core debug mode is enabled
#ifdef __CORE_DEBUG

#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif
#ifdef PANIC_PRINT_ENABLED
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
#else // !PANIC_PRINT_ENABLED
    #define panic_begin()
    #define panic_printf(fmt, ...) 0
#endif // PANIC_PRINT_ENABLED

    /**
     * @brief finalize panic, halt or reset MCU
     * @note this function never returns
     */
    __attribute__((noreturn)) void panic_end(void);

    /**
     * @brief core panic handler
     * @param message message to print before panicing. may be set to NULL to omit message
     */
    inline void panic(const char *message)
    {
#ifdef PANIC_PRINT_ENABLED
        if (message != NULL)
        {
            panic_begin();
            panic_printf(message);
        }
#endif

        panic_end();
    }

#ifdef __cplusplus
}
#endif
#endif // __CORE_DEBUG
