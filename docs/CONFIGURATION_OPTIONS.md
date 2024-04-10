# Configuration Options

the arduino core supports many options to configure the behavior of the core. These options can be set in the `platformio.ini` file.

## Example

```ini
[env:my_env]
# ...
build_flags = 
    -D __CORE_DEBUG
```

## Available Options

## Hardware Serial Options

the following options configure the hardware serial (`Usart`) driver.

| Option                           | Module | Description                                                                                            | Default Value        |
| -------------------------------- | ------ | ------------------------------------------------------------------------------------------------------ | -------------------- |
| `SERIAL_BUFFER_SIZE`             | Usart  | set the size of both the RX and TX buffer.                                                             | `64`                 |
| `SERIAL_TX_BUFFER_SIZE`          | Usart  | set the size of the TX buffer.                                                                         | `SERIAL_BUFFER_SIZE` |
| `SERIAL_RX_BUFFER_SIZE`          | Usart  | set the size of the RX buffer.                                                                         | `SERIAL_BUFFER_SIZE` |
| `DISABLE_SERIAL_GLOBALS`         | Usart  | disable `Serial<n>` global variables.                                                                  | disabled             |
| `USART_AUTO_CLKDIV_OS_CONFIG`    | Usart  | enable automatic clock divider and oversampling configuration. [Documentation](./usart/AUTO_CLKDIV.md) | disabled             |
| `USART_RX_DMA_SUPPORT`           | Usart  | enable support for RX DMA. [Documentation](./usart/RX_DMA.md)                                          | disabled             |
| `USART_RX_ERROR_COUNTERS_ENABLE` | Usart  | enable error counters. [Documentation](./usart/ERROR_COUNTERS.md)                                      | disabled             |


### Debugging Options

the following options configure the debugging behavior of the core.

| Option                        | Module        | Description                                                                                           | Default Value |
| ----------------------------- | ------------- | ----------------------------------------------------------------------------------------------------- | ------------- |
| `__CORE_DEBUG`                | core_debug    | enables additional assertions and debug output at the cost of flash usage.                            | disabled      |
| `PANIC_ENABLE`                | panic         | force enable the panic mechanism, even if no output is configured.                                    | disabled      |
| `__PANIC_SHORT_FILENAMES`     | panic         | use filenames instead of paths in panic output to reduce flash usage.                                 | disabled      |
| `__OMIT_PANIC_MESSAGE`        | panic         | omit error messages in `panic()` output, reducing the flash usage.                                    | disabled      |
| `HANG_ON_PANIC`               | panic         | enter a infinite loop on `panic()`. if not set, the CPU will reset.                                   | disabled      |
| `PANIC_USART<n>_TX_PIN`       | panic         | set the gpio pin that is used to transmit panic messages. `[n]` can be any value in [1,2,3,4]         | disabled      |
| `CORE_DISABLE_FAULT_HANDLER`  | fault_handler | disable the core-internal fault handler. this is only recommended if you have your own fault handler. | disabled      |
| `HARDFAULT_EXCLUDE_CFSR_INFO` | fault_handler | exclude CFSR flag parsing from fault output, reducing flash usage.                                    | disabled      |

see the Documentation for the [`panic`](./PANIC.md) and [`fault_handler`](./FAULT_HANDLER.md) modules for more information.


## Miscellanous Options

| Option                                 | Module     | Description                                                                                                                       | Default Value                   |
| -------------------------------------- | ---------- | --------------------------------------------------------------------------------------------------------------------------------- | ------------------------------- |
| `CORE_ADC_RESOLUTION`                  | adc        | set the default resolution of ADC driver. can be `8`, `10`, or `12`. can be overwritten using `analogReadResolution()`            | `10`                            |
| `F_CPU=SYSTEM_CLOCK_FREQUENCIES.pclk1` | sysclk     | overwrites the `F_CPU` value. refer to the HC32F460 user manual, Section 4.3, Table 4-1 for more details on the different clocks. | `SYSTEM_CLOCK_FREQUENCIES.hclk` |
| `PROTECT_VECTOR_TABLE`                 | interrupts | protect the vector table from getting accidentally overwritten. [Documentation](./mpu/PROTECT_VECTOR_TABLE.md)                    | `1`                             |
| `CORE_DONT_RESTORE_DEFAULT_CLOCKS`     | init       | disable restoring the default system clock. define to not restore default clocks.                                                 | disabled                        |
| `CORE_DONT_ENABLE_ICACHE`              | init       | disable enabling flash instruction cache. define to disable icache.                                                               | disabled                        |
