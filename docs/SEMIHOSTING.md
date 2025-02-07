# Semihosting Driver

the arduino core includes a driver for [Semihosting](https://pyocd.io/docs/semihosting.html) support.
Semihosting allows a debugging target (the HC32F460 in this case) to call system calls on the debugging host machine, e.g. writing to the debuggers console.

> [!TIP]
> Semihosting is only available when the target is connected to a debugger.

> [!TIP]
> Semihosting only works with a supported debugger and semihosting enabled.


## Usage

semihosting is by default disabled in pyOCD, so if you want to use it you'll need to enable it. 
you can do this by either running 'monitor arm semihosting enable' in the debug console, or alternatively by adding the same command to your `.platformio.ini` file:

```ini
# ...
debug_extra_cmds =
    monitor arm semihosting enable
```

> [!TIP]
> it seems like single-stepping over functions containing semihosting calls don't work correctly.   
> the target will stop execution at the semihosting call, but gdb will not pick this up.
> thus, the debugging sessions seems stuck.
> To work around this, manually pause the target and continue again (make sure to set a breakpoint where you want to end up after continuing).


### Console Output

after enabling semihosting, you can connect to the telnet server (normally on port 4444, but refer to debug console) using a telnet client like puTTY.

> [!TIP]
> when using puTTY, make sure to enable the "Implicit CR in every LF" option in the terminal settings, as output is otherwise not displayed correctly.


there is also some way to redirect the semihosting output to the debug console, but this doesn't seem to work at the moment.
commands for this should normally be `monitor set option semihost_console_type=console` or `monitor set option semihost_console_type=telnet`, and can be checked with `monitor show option semihost_console_type`.


### File I/O

the semihosting driver supports writing to files on the host machine.
relative paths are resolved relative to the pyOCD package directory, which is usually located in `~/.platformio/packages/tool-pyocd/` or `C:/Users/<username>/.platformio/packages/tool-pyocd/`.
this sadly cannot be changed at the moment.


## C API

the semihosting driver is exposed through the `semihosting.h` header file.
this header provides both a generic `sh_syscall` function for directly calling semihosting system calls, as well as wrapper functions for commonly used operations.

generally, the functions are fairly similar to linux system calls, but with a `sh_` prefix.


for usage examples, refer to the example given in `examples/semihosting`.


> [!WARNING]
> without a debugger attached, calling any of the semihosting functions may result in the target stopping execution.   
> you can also check if a debugger is attached using the `sh_is_debugger_attached` function.

## printf redirection

by defining `REDIRECT_PRINTF_TO_DEBUGGER=1` during build, the `printf` function will be redirected to the debugger console using semihosting.

> [!TIP]
> printf redirection will automatically check if a debugger is attached before redirecting output. 
> this means that you can use `printf` in your code without worrying about the target stopping execution when no debugger is attached.
