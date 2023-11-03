#pragma once
#include "panic_api.h"
#include <stdlib.h>

// determine if at least one panic output is defined
#define PANIC_OUTPUT_AVAILABLE                                                                                         \
  (defined(PANIC_USART1_TX_PIN) || defined(PANIC_USART2_TX_PIN) || defined(PANIC_USART3_TX_PIN) ||                     \
   defined(PANIC_USART4_TX_PIN))

// determine if panic handler should be active:
// - either manually enabled
// - or panic output is available
#define ENABLE_PANIC_HANDLER (defined(PANIC_ENABLE) || PANIC_OUTPUT_AVAILABLE)

#if ENABLE_PANIC_HANDLER

  // control printf buffer size
  #ifndef PANIC_PRINTF_BUFFER_SIZE
    #define PANIC_PRINTF_BUFFER_SIZE 256
  #endif

  // panic usart baud rate
  #ifndef PANIC_USART_BAUDRATE
    #define PANIC_USART_BAUDRATE 115200
  #endif

  #ifdef __cplusplus
extern "C"
{
  #endif
  /**
   * @brief print message to panic output, if enabled
   * @param fmt format string
   * @param ... format arguments
   * @return number of characters printed
   */
  size_t panic_printf(const char *fmt, ...);

  /**
   * @brief internal panic handler
   * @param message message to print before panicing. may be set to NULL to omit
   * message
   */
  void _panic(const char *message);

  #ifdef __cplusplus
}
  #endif

  // define file name macro for panic() to use
  #if defined(__PANIC_SHORT_FILENAMES) && defined(__SOURCE_FILE_NAME__)
    #define PANIC_FILE_NAME __SOURCE_FILE_NAME__ // only filename
  #else
    // no short filenames, or __SOURCE_FILE_NAME__ not available
    #if defined(__PANIC_SHORT_FILENAMES)
      #warning "__PANIC_SHORT_FILENAMES is defined, but __SOURCE_FILE_NAME__ is not available."
    #endif

    #define PANIC_FILE_NAME __FILE__
  #endif

  #define PANIC_LINE_NUMBER_STR STRINGIFY(__LINE__)

  /**
   * @brief core panic handler
   * @param message message to print before panicing. may be set to NULL to
   * omit message
   * @note automatically adds file and line number to message
   */
  #ifdef __OMIT_PANIC_MESSAGE
    #define panic(msg) _panic(PANIC_FILE_NAME "l" PANIC_LINE_NUMBER_STR, NULL)
  #else
    #define panic(msg) _panic("[" PANIC_FILE_NAME " l" PANIC_LINE_NUMBER_STR "]" msg)
  #endif
#else // !ENABLE_PANIC_HANDLER
  #define panic_printf(fmt, ...)
  #define panic(msg)
#endif // ENABLE_PANIC_HANDLER
