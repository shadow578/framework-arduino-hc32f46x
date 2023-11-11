#include "panic.h"

#if ENABLE_PANIC_HANDLER

  #include "../usart/usart_sync.h"
  #include <hc32_ddl.h>
  #include <stdarg.h>
  #include <stdio.h>

//
// default panic output
//
__attribute__((weak)) void panic_begin(void)
{
  #if PANIC_OUTPUT_AVAILABLE
  const stc_usart_uart_init_t panic_usart_config = {
      .enClkMode = UsartIntClkCkNoOutput,
      .enClkDiv = UsartClkDiv_1,
      .enDataLength = UsartDataBits8,
      .enDirection = UsartDataLsbFirst,
      .enStopBit = UsartOneStopBit,
      .enParity = UsartParityNone,
      .enSampleMode = UsartSampleBit8,
      .enDetectMode = UsartStartBitFallEdge,
      .enHwFlow = UsartRtsEnable,
  };

    // initialize USART
    #ifdef PANIC_USART1_TX_PIN
  usart_sync_init(M4_USART1, PANIC_USART1_TX_PIN, PANIC_USART_BAUDRATE, &panic_usart_config);
    #endif
    #ifdef PANIC_USART2_TX_PIN
  usart_sync_init(M4_USART2, PANIC_USART2_TX_PIN, PANIC_USART_BAUDRATE, &panic_usart_config);
    #endif
    #ifdef PANIC_USART3_TX_PIN
  usart_sync_init(M4_USART3, PANIC_USART3_TX_PIN, PANIC_USART_BAUDRATE, &panic_usart_config);
    #endif
    #ifdef PANIC_USART4_TX_PIN
  usart_sync_init(M4_USART4, PANIC_USART4_TX_PIN, PANIC_USART_BAUDRATE, &panic_usart_config);
    #endif
  #endif // PANIC_OUTPUT_AVAILABLE
}

__attribute__((weak)) void panic_puts(const char *str)
{
  // print to USART
  #ifdef PANIC_USART1_TX_PIN
  usart_sync_write(M4_USART1, str);
  #endif
  #ifdef PANIC_USART2_TX_PIN
  usart_sync_write(M4_USART2, str);
  #endif
  #ifdef PANIC_USART3_TX_PIN
  usart_sync_write(M4_USART3, str);
  #endif
  #ifdef PANIC_USART4_TX_PIN
  usart_sync_write(M4_USART4, str);
  #endif
}

__attribute__((noreturn, weak)) void panic_end(void)
{
  #ifdef HANG_ON_PANIC
  // enter infinite loop
  while (1)
    ;
  #else
  // reset MCU
  NVIC_SystemReset();
  #endif
}

//
// helper functions
//
static char panic_printf_buffer[PANIC_PRINTF_BUFFER_SIZE];

size_t panic_printf(const char *fmt, ...)
{
  // format the string into buffer
  va_list args;
  va_start(args, fmt);
  size_t len = vsnprintf(panic_printf_buffer, PANIC_PRINTF_BUFFER_SIZE, fmt, args);
  va_end(args);

  // then use puts to print it
  panic_puts(panic_printf_buffer);

  // return length of formatted string
  return len;
}

void _panic(const char *message)
{
  if (message != NULL)
  {
    panic_begin();
    panic_printf(message);
  }

  panic_end();
}

#endif // ENABLE_PANIC_HANDLER
