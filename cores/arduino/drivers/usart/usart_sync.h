#include <stdarg.h>
#include <stdio.h>
#include <hc32_ddl.h>
#include "../gpio/gpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief initialize USART device for synchronous output
     * @param usart USART device to use
     * @param tx_pin GPIO pin to use for TX (used with GPIO_* functions)
     * @note existing USART configuration is overwritten without a way to restore it
     * @note use of this function could lead to multiple TX pins being configured for the same USART device. be careful!
     * @note somewhat internal function, not intended for direct use. do not use this unless you know what you're doing!
     */
    void usart_sync_init(M4_USART_TypeDef *usart, const gpio_pin_t tx_pin, const uint32_t baudrate, const stc_usart_uart_init_t *config);

    /**
     * @brief synchronously write a single character to USART device
     * @param usart USART device to use
     * @param ch character to write
     * @note usart must be initialized using usart_sync_init() first
     * @note somewhat internal function, not intended for direct use. do not use this unless you know what you're doing!
     */
    void usart_sync_putc(M4_USART_TypeDef *usart, const char ch);

    /**
     * @brief synchronously write a string to USART device
     * @param usart USART device to use
     * @param str string to print, null-terminated
     * @note need to manually initialize USART device with usart_sync_init() first
     * @note somewhat internal function, not intended for direct use. do not use this unless you know what you're doing!
     */
    void usart_sync_write(M4_USART_TypeDef *usart, const char *str);

#ifdef __cplusplus
}
#endif