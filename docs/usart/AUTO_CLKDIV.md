# `USART_AUTO_CLKDIV_OS_CONFIG` Option

when defining the `USART_AUTO_CLKDIV_OS_CONFIG` option, the `Usart` driver will automatically configure the clock divider and oversampling settings based on the baudrate and the system clock frequency in such a way that the error to the actually achieved baudrate is minimized.
this decreases the chance of communication errors due to a mismatch between the configured and the actual baudrate, at the cost of about 560 bytes of flash space.

not enabling this option may limit the values that can be used for the baudrate when using the 'traditional' `Usart::begin()` functions.
when using `Usart::begin(uint32_t baud, const stc_usart_uart_init_t *config)`, this option is not required, as the clock divider and oversampling settings can be set manually.

> [!NOTE]
> this option is opt-in to keep backwards compatibility.
> it is recommended to enable this option if you can afford the flash space.
