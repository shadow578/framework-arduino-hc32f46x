#include "usart_sync.h"
#include <addon_usart.h>

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

void usart_sync_init(M4_USART_TypeDef *usart, const gpio_pin_t tx_pin, const uint32_t baudrate, const stc_usart_uart_init_t *config)
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
    SetUartBaudrate(usart, baudrate);
}

void usart_sync_putc(M4_USART_TypeDef *usart, const char ch)
{
    // enable TX function
    USART_FuncCmd(usart, UsartTx, Enable);

    // wait until TX buffer is empty
    while (USART_GetStatus(usart, UsartTxEmpty) == Reset)
        ;

    // write char to TX buffer
    USART_SendData(usart, ch);
}

void usart_sync_write(M4_USART_TypeDef *usart, const char *str)
{
    // enable TX function
    USART_FuncCmd(usart, UsartTx, Enable);

    // print the string
    for (size_t i = 0; str[i] != '\0'; i++)
    {
        usart_sync_putc(usart, str[i]);
    }
}
