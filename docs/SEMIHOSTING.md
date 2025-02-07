# Semihosting Driver

the arduino core includes a driver for Semihosting support.
Semihosting allows a debugging target (the HC32F460 in this case) to call system calls on the debugging host machine, e.g. writing to the debuggers console.

> [!TIP]
> Semihosting is only available when the target is connected to a debugger.


## C API

the semihosting driver is exposed through the `semihosting.h` header file.
this header provides both a generic `sh_syscall` function for directly calling semihosting system calls, as well as wrapper functions for commonly used operations.

generally, the functions are fairly similar to linux system calls, but with a `sh_` prefix.


for usage examples, refer to the example given in `examples/semihosting`.


> [!WARNING]
> without a debugger attached, calling any of the semihosting functions may result in the target stopping execution.


## printf redirection

by defining `REDIRECT_PRINTF_TO_DEBUGGER=1` during build, the `printf` function will be redirected to the debugger console using semihosting.
