# Arduinio Core Panic

the arduino core includes a panic mechanism to handle unrecoverable errors.
the panic mechanism is designed to provide useful information about the error that occurred and to allow the developer to take appropriate action.


## C / C++ API

the panic mechanism is exposed through the `panic.h` header file.

a panic can be caused using the  `panic(msg)` macro, where `msg` is a string literal that describes the error that occurred.

```cpp
#include <panic.h>

void setup()
{
    panic("this is a panic message");
}

void loop() {}
```


alternative, the `size_t panic_printf(const char *fmt, ...)` function may be used to format a panic message using `printf`-style formatting.
in this case, the panic must be manually started and ended using the `void panic_begin()` and `void panic_end()` function.

```cpp
#include <panic.h>

void setup()
{
    panic_begin();
    panic_printf("this is a panic message with a number: %d \n", 42);
    panic_printf("multiple panic_printf calls are supported too!");
    panic_end();
}

void loop() {}
```


## Custom Panic 

to define a custom panic output, first define the `PANIC_ENABLE` option while removing any `PANIC_USART<n>_TX_PIN` options.
then, implement the following functions defined in the `panic_api` header:

```cpp
#include <panic_api.h>

void panic_begin()
{
    // prepare panic output
}

void panic_puts(const char *str)
{
    // print string to panic output
}

void panic_end()
{
    // finalize panic output, then
    // halt or reset MCU
}
```

