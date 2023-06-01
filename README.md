# PlatformIO Arduino support for HUADA HC32F460 Series

This repository contains Arduino framework for the [HUADA HC32F460 Series](https://www.hdsc.com.cn/Category83-1487), for use with the [the hc32f46x platform](https://github.com/shadow578/platform-hc32f46x).

Primary usage is for [Marlin](https://github.com/shadow578/Marlin-H32), tho other use cases should work too.

# Arduino Core options

the core offers the following options, which can be set in `platformio.ini` using the `build_flags` option.

| Name                                     | Description                                                                                                                                                                                                                  |
| ---------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `__CORE_DEBUG`                           | enable arduino core debug mode. enabled additional debug output at the cost of flash space                                                                                                                                   |
| `ENABLE_MICROS`                          | enable `micros()` function. this increases the speed of the SysTick interrupt, which may cause reduced performance. By default, the SysTick interrupt is called every 1ms. With this option enabled, it is called every 1us. |
| `DISABLE_SERIAL_GLOBALS`                 | disable `Serial1`, `Serial2`, `Serial3`, `Serial4` global variables. this saves a few bytes of flash space, but you'll have to define the serial objects yourself.                                                           |
| `F_CPU=(SYSTEM_CLOCK_FREQUENCIES.pclk1)` | overwrites the `F_CPU` value. by default, `hclk` is used. refer to the HC32F460 user manual, Section 4.3, Table 4-1 for more details on the different clocks.                                                                |

# Arduino Core Panic

the core includes a panic mechanism that can print panic messages to one or more usart outputs. this is useful for debugging, as it allows you to see what went wrong.

## Options

the following options are available for the core panic mechanism. they can be set in `platformio.ini` using the `build_flags` option.

| Name                          | Description                                                                                                                          |
| ----------------------------- | ------------------------------------------------------------------------------------------------------------------------------------ |
| `__CORE_DEBUG`                | enables arduino core debug mode. if disabled, panic (and thus faults) will be silent                                                 |
| `PANIC_USART[n]_TX_PIN`       | set the gpio pin that is used to transmit panic messages. `[n]` can be any value in [1,2,3,4], and up to four outputs may be defined |
| `HARDFAULT_EXCLUDE_CFSR_INFO` | exclude CFSR flag messages from fault output. this will reduce the output size, but you'll have to look up the flag values yourself  |
| `HANG_ON_PANIC`               | enter a infinite loop on panic condition. default behaviour is to reset the mcu                                                      |

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

## CPU Fault Handler

the core includes a `HardFault` handler that prints information about the fault using the panic mechanism.
For more information about the fault handler, see [HardFault.md](docs/HardFault.md).

# Notice

this framework is still in development, and not yet ready for production use. expect things to break over time.
