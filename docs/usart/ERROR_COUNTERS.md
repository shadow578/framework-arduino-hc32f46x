# `USART_RX_ERROR_COUNTERS_ENABLE` Option

when defining the `USART_RX_ERROR_COUNTERS_ENABLE` option, the `Usart` driver class will keep track of the number of framing, parity, overrun and dropped data errors.

the counters can be accessed using the `Usart::getFramingErrorCount()`, `Usart::getParityErrorCount()`, `Usart::getOverrunErrorCount()` and `Usart::getDroppedDataErrorCount()` functions.
