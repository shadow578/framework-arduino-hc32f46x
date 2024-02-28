# PlatformIO Arduino support for HUADA HC32F460 Series

This repository contains Arduino framework for the [HUADA HC32F460 Series](https://www.hdsc.com.cn/Category83-1487), for use with the [the hc32f46x platform](https://github.com/shadow578/platform-hc32f46x).

Primary usage is for [Marlin](https://github.com/shadow578/Marlin-H32), tho other use cases should work too.

# Arduino Core options

the core offers the following multiple options, which can be set in `platformio.ini` using the `build_flags` option.

## Debug Options

| Option                        | Description                                                                                   |
| ----------------------------- | --------------------------------------------------------------------------------------------- |
| `__CORE_DEBUG`                | enable debug mode. this enables additional debug output at the cost of flash space.           |
| `__PANIC_SHORT_FILENAMES`     | use only the filename, not the full path, in debug output. saves some flash space.            |
| `__OMIT_PANIC_MESSAGE`        | omit error messages in `panic()` output. saves some flash space.                              |
| `CORE_DISABLE_FAULT_HANDLER`  | disable the core-internal fault handler. only recommended if you have your own fault handler. |
| `HARDFAULT_EXCLUDE_CFSR_INFO` | exclude CFSR flag parsing from fault output. saves some flash space.                          |
| `HANG_ON_PANIC`               | enter a infinite loop on `panic()`. default behaviour is to reset the mcu.                    |
| `PANIC_USART<n>_TX_PIN`       | set the gpio pin that is used to transmit panic messages. `[n]` can be any value in [1,2,3,4] |

## Hardware Serial Options

| Option                           | Description                                                                         |
| -------------------------------- | ----------------------------------------------------------------------------------- |
| `SERIAL_BUFFER_SIZE`             | set the size of both the RX and TX buffer. default value is `64`                    |
| `SERIAL_TX_BUFFER_SIZE`          | set the size of the TX buffer. default value is `SERIAL_BUFFER_SIZE`                |
| `SERIAL_RX_BUFFER_SIZE`          | set the size of the RX buffer. default value is `SERIAL_BUFFER_SIZE`                |
| `DISABLE_SERIAL_GLOBALS`         | disable `Serial<n>` global variables.                                               |
| `USART_AUTO_CLKDIV_OS_CONFIG`    | enable automatic clock divider and oversampling configuration in the `Usart` driver |
| `USART_RX_DMA_SUPPORT`           | enable support for RX DMA in the `Usart` driver                                     |
| `USART_RX_ERROR_COUNTERS_ENABLE` | enable error counters in the `Usart` driver.                                        |

### `USART_AUTO_CLKDIV_OS_CONFIG` Option

when defining the `USART_AUTO_CLKDIV_OS_CONFIG` option, the `Usart` driver will automatically configure the clock divider and oversampling settings based on the baudrate and the system clock frequency in such a way that the error to the actually achieved baudrate is minimized.
this decreases the chance of communication errors due to a mismatch between the configured and the actual baudrate, at the cost of about 560 bytes of flash space.

not enabling this option may limit the values that can be used for the baudrate when using the 'traditional' `Usart::begin()` functions.
when using `Usart::begin(uint32_t baud, const stc_usart_uart_init_t *config)`, this option is not required, as the clock divider and oversampling settings can be set manually.

> [!NOTE]
> this option is opt-in to keep backwards compatibility.
> it is recommended to enable this option if you can afford the flash space.

### `USART_RX_DMA_SUPPORT` Option

when defining the `USART_RX_DMA_SUPPORT` option, the `Usart` driver class will support receiving data using DMA.
to use this feature, call `Usart::enableRxDma()` before calling `Usart::begin()`.
you must pass a unused DMA peripheral and channel to `Usart::enableRxDma()`. failing to do so may result in undefined behaviour.

> [!NOTE]
> enabling this option will increase the flash usage by about 1KB.

#### example usage:

platformio.ini:

```ini
build_flags      =
  -D USART_RX_DMA_SUPPORT # enable RX DMA support
```

main.cpp:

```cpp
#include <Arduino.h>

void setup()
{
    Serial.enableRxDma(M4_DMA1, DmaCh0); // enable RX DMA w/ DMA1, channel 0
    Serial.begin(115200); // start serial
    // ...
}

void loop() {}
```

### `USART_RX_ERROR_COUNTERS_ENABLE` Option

when defining the `USART_RX_ERROR_COUNTERS_ENABLE` option, the `Usart` driver class will keep track of the number of framing, parity, overrun and dropped data errors.

the counters can be accessed using the `Usart::getFramingErrorCount()`, `Usart::getParityErrorCount()`, `Usart::getOverrunErrorCount()` and `Usart::getDroppedDataErrorCount()` functions.

## Miscellanous Options

| Option                                 | Description                                                                                                                                                   |
| -------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `CORE_ADC_RESOLUTION`                  | set the default resolution of ADC driver. can be `8`, `10`, or `12`. default is `10`. can be overwritten using `analogReadResolution()`                       |
| `F_CPU=SYSTEM_CLOCK_FREQUENCIES.pclk1` | overwrites the `F_CPU` value. by default, `hclk` is used. refer to the HC32F460 user manual, Section 4.3, Table 4-1 for more details on the different clocks. |

## Example

```ini
# ...
build_flags =
    -D __CORE_DEBUG
```

# Arduino Core Panic

the core includes a panic mechanism that can print panic messages to one or more usart outputs. this is useful for debugging, as it allows you to see what went wrong.
the following options are available for the core panic mechanism. they can be set in `platformio.ini` using the `build_flags` option.

| Name                          | Description                                                                                                                                                                |
| ----------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `__CORE_DEBUG`                | enables arduino core debug mode. if disabled, panic (and thus faults) will be silent                                                                                       |
| `__PANIC_SHORT_FILENAMES`     | uses only the filename, not the full path, in panic messages                                                                                                               |
| `__OMIT_PANIC_MESSAGE`        | omits the panic message string from panic output. only file and line number will be printed. this reduces the output size, but you'll have to look up the message yourself |
| `PANIC_USART[n]_TX_PIN`       | set the gpio pin that is used to transmit panic messages. `[n]` can be any value in [1,2,3,4], and up to four outputs may be defined                                       |
| `HARDFAULT_EXCLUDE_CFSR_INFO` | exclude CFSR flag messages from fault output. this will reduce the output size, but you'll have to look up the flag values yourself                                        |
| `HANG_ON_PANIC`               | enter a infinite loop on panic condition. default behaviour is to reset the mcu                                                                                            |

```ini
build_flags =
    -D __CORE_DEBUG
    -D PANIC_USART1_TX_PIN=PA1
    -D PANIC_USART2_TX_PIN=PB1
```

## Usage

the core panic mechanism is used by calling the `panic()` function.
this function takes a single argument, which is the panic message to print.
the message can be a c string, or `NULL` to omit the message.

```cpp
#include <drivers/panic/panic.h>

panic("this is a panic message");
panic(NULL); // omit message
```

if multiple lines of output are required, the `panic_printf()` function can be used.
in this case, `panic_begin()` and `panic_end()` must be called before and after the `panic_printf()` calls.

```cpp
#include <drivers/panic/panic.h>

panic_begin(); // start panic output
panic_printf("this is a panic message\n");
panic_printf("this is another panic message\n");
panic_end(); // end panic output and reset mcu
```

## Assertions

the arduino core asserts are implemented using the panic mechanism.
if an assertion fails, the panic mechanism is called with a message containing the file and line number of the assertion.

```cpp
#include <core_debug.h>

CORE_ASSERT(1 == 2, "1 is not 2"); // assert expression, fail if false
CORE_ASSERT_FAIL("this is a fail message"); // assert always fails
```

## CPU Fault Handler

the core includes a `HardFault` handler that prints information about the fault using the panic mechanism.
For more information about the fault handler, see [HardFault.md](docs/HardFault.md).

# Credits

This project could not have been possible without the following projects:

- [Voxelab-64/Aquila_X2](https://github.com/Voxelab-64/Aquila_X2) (original implementation)
- [kgoveas/Arduino-Core-Template](https://github.com/kgoveas/Arduino-Core-Template) (template for Arduino headers)
- [stm32duino/Arduino_Core_STM32](https://github.com/stm32duino/Arduino_Core_STM32) (misc. Arduino functions)
