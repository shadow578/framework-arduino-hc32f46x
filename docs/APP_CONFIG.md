# `app_config.h` to replace `build_flags`

the core allows applications to define a `app_config.h` file, which is automatically included for all source files.
this file can be used to define the same options as `build_flags`, but allows for more complex configurations.

to use `app_config.h`, either create a file with that name in the root of your project, or specify a custom path in the `platformio.ini` file.

```ini
[env:hc32f460]
board_build.app_config = src/my_app_config.h
# ...
```
