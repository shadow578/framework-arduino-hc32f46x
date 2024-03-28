# Arduino Core Assert Mechanism

the arduino core includes an assert mechanism to handle assertions in the code.
assertions are used to check for conditions that should never occur, and if they do, the program will [panic](./PANIC.md) with a useful error message.

> [!IMPORTANT]
> the assert mechanism will only cause a panic if the `__CORE_DEBUG` option is enabled 


## C / C++ API

the assert mechanism is exposed through the `core_debug.h` header file.

an assertion can be made using the `CORE_ASSERT(expression, message, ...)` macro, where `expression` is the expression to check.
additionally, the `ASSERT_GPIO_PIN_VALID(gpio_pin, fn_name, ...)` macro can be used to check if a given pin is a valid GPIO pin.

```cpp
#include <core_debug>

int foo(int i)
{
    // assert that i is not zero, panic with a message if it is
    // if __CORE_DEBUG is not enabled, this will be a no-op
    CORE_ASSERT(i != 0, "i should not be zero");

    // assert that i is not zero, panic with a message if it is and return -1
    // if __CORE_DEBUG is not enabled, this will only return -1
    CORE_ASSERT_RETURN(i != 0, "i should not be zero", return -1);

    // always fails, will panic with a message
    // if __CORE_DEBUG is not enabled, this will be a no-op
    CORE_ASSERT_FAIL("this always fails");

    // check that i is a valid GPIO pin, return -2 if it is not
    // if __CORE_DEBUG is not enabled, this will only return -2
    // otherwise, it will panic with a message "invalid GPIO pin supplied to foo"
    ASSERT_GPIO_PIN_VALID(i, "foo", return -2)
}
```

