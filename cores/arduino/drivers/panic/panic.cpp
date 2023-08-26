#include "panic.h"

// only compile in core debug mode
#ifdef __CORE_DEBUG
#include "../gpio/gpio.h"
#include "../../core_hooks.h"
#include "../usart/usart_sync.h"
#include <stdarg.h>
#include <stdio.h>
#include <hc32_ddl.h>

// only compile usart output code if a panic print is enabled
#ifdef PANIC_PRINT_ENABLED

// panic usart baud rate
#ifndef PANIC_USART_BAUDRATE
#define PANIC_USART_BAUDRATE 115200
#endif

// panic usart configuration
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

//
// panic helper functions
//
#define PANIC_PRINTF_BUFFER_SIZE 256
char panic_printf_buffer[PANIC_PRINTF_BUFFER_SIZE];

void panic_begin(void)
{
    core_hook_panic_begin_pre();

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

    core_hook_panic_begin_post();
}

size_t panic_printf(const char *fmt, ...)
{
    // format the string into buffer
    va_list args;
    va_start(args, fmt);
    size_t len = vsnprintf(panic_printf_buffer, PANIC_PRINTF_BUFFER_SIZE, fmt, args);
    va_end(args);

    // print to USART
#ifdef PANIC_USART1_TX_PIN
    usart_sync_write(M4_USART1, panic_printf_buffer);
#endif
#ifdef PANIC_USART2_TX_PIN
    usart_sync_write(M4_USART2, panic_printf_buffer);
#endif
#ifdef PANIC_USART3_TX_PIN
    usart_sync_write(M4_USART3, panic_printf_buffer);
#endif
#ifdef PANIC_USART4_TX_PIN
    usart_sync_write(M4_USART4, panic_printf_buffer);
#endif

    // return length of formatted string
    return len;
}

#endif // PANIC_PRINT_ENABLED

void panic_end(void)
{
    core_hook_panic_end();

#ifdef HANG_ON_PANIC
    // enter infinite loop
    while (1)
        ;
#else
    // reset MCU
    NVIC_SystemReset();
#endif
}

#endif // __CORE_DEBUG
