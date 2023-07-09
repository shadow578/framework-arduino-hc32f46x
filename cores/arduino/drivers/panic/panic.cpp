#include "panic.h"

// only compile in core debug mode
#ifdef __CORE_DEBUG
#include "../gpio/gpio.h"
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

// USART_TypeDef to gpio function select mapping
#define USART_DEV_TO_TX_FUNC(usart)       \
    usart == M4_USART1   ? Func_Usart1_Tx \
    : usart == M4_USART2 ? Func_Usart2_Tx \
    : usart == M4_USART3 ? Func_Usart3_Tx \
                         : Func_Usart4_Tx

// USART_TypeDef to PWC_FCG1_PERIPH_USARTx mapping
#define USART_DEV_TO_PERIPH_CLOCK(usart)          \
    usart == M4_USART1   ? PWC_FCG1_PERIPH_USART1 \
    : usart == M4_USART2 ? PWC_FCG1_PERIPH_USART2 \
    : usart == M4_USART3 ? PWC_FCG1_PERIPH_USART3 \
                         : PWC_FCG1_PERIPH_USART4

/**
 * @brief initialize USART device for panic output via panic_usart_putchar()
 * @param usart USART device to use for panic output
 * @param tx_pin GPIO pin to use for TX (used with GPIO_* functions)
 * @note existing USART configuration is overwritten without a way to restore it
 */
void panic_usart_init(M4_USART_TypeDef *usart, const gpio_pin_t tx_pin, const uint32_t baudrate, const stc_usart_uart_init_t *config)
{
    // disable and de-init usart peripheral
    USART_FuncCmd(usart, UsartRx, Disable);
    USART_FuncCmd(usart, UsartRxInt, Disable);
    USART_FuncCmd(usart, UsartTx, Disable);
    USART_FuncCmd(usart, UsartTxEmptyInt, Disable);
    USART_FuncCmd(usart, UsartTxCmpltInt, Disable);
    USART_DeInit(usart);

    // set tx pin function to USART TX output
    GPIO_SetFunc(tx_pin, USART_DEV_TO_TX_FUNC(usart), Disable);

    // enable USART clock
    PWC_Fcg1PeriphClockCmd(USART_DEV_TO_PERIPH_CLOCK(usart), Enable);

    // initialize USART peripheral and set baudrate
    USART_UART_Init(usart, config);
    USART_SetBaudrate(usart, baudrate);
}

/**
 * @brief write data to USART device for panic output
 * @param usart USART device to use for panic output
 * @param str string to print, null-terminated
 * @note need to manually initialize USART device with panic_usart_init() first
 */
void panic_usart_print(M4_USART_TypeDef *usart, const char *str)
{
    // enable TX function
    USART_FuncCmd(usart, UsartTx, Enable);

    // print the string
    for (size_t i = 0; str[i] != '\0'; i++)
    {
        // write data to TX buffer
        USART_SendData(usart, str[i]);

        // wait until TX buffer is empty
        while (USART_GetStatus(usart, UsartTxEmpty) == Reset)
            ;
    }
}

//
// panic helper functions
//
#define PANIC_PRINTF_BUFFER_SIZE 256
char panic_printf_buffer[PANIC_PRINTF_BUFFER_SIZE];

void panic_begin(void)
{
// initialize USART
#ifdef PANIC_USART1_TX_PIN
    panic_usart_init(M4_USART1, PANIC_USART1_TX_PIN, PANIC_USART_BAUDRATE, &panic_usart_config);
#endif
#ifdef PANIC_USART2_TX_PIN
    panic_usart_init(M4_USART2, PANIC_USART2_TX_PIN, PANIC_USART_BAUDRATE, &panic_usart_config);
#endif
#ifdef PANIC_USART3_TX_PIN
    panic_usart_init(M4_USART3, PANIC_USART3_TX_PIN, PANIC_USART_BAUDRATE, &panic_usart_config);
#endif
#ifdef PANIC_USART4_TX_PIN
    panic_usart_init(M4_USART4, PANIC_USART4_TX_PIN, PANIC_USART_BAUDRATE, &panic_usart_config);
#endif
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
    panic_usart_print(M4_USART1, panic_printf_buffer);
#endif
#ifdef PANIC_USART2_TX_PIN
    panic_usart_print(M4_USART2, panic_printf_buffer);
#endif
#ifdef PANIC_USART3_TX_PIN
    panic_usart_print(M4_USART3, panic_printf_buffer);
#endif
#ifdef PANIC_USART4_TX_PIN
    panic_usart_print(M4_USART4, panic_printf_buffer);
#endif

    // return length of formatted string
    return len;
}

#endif // PANIC_PRINT_ENABLED

void panic_end(void)
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

#endif // __CORE_DEBUG
