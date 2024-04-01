# Arduino Support for HUADA HC32F460 Series

<p align="center">
    <a href="" alt="Version">
        <img src="https://img.shields.io/github/package-json/v/shadow578/framework-arduino-hc32f46x" />
    </a>
    <a href="https://github.com/shadow578/framework-arduino-hc32f46x/pulse" alt="Activity">
        <img src="https://img.shields.io/github/commit-activity/m/shadow578/framework-arduino-hc32f46x" />
    </a>
    <a href="https://github.com/shadow578/framework-arduino-hc32f46x/actions/workflows/build_examples.yaml">
        <img src="https://github.com/shadow578/framework-arduino-hc32f46x/actions/workflows/build_examples.yaml/badge.svg?branch=main" alt="ci status">
    </a>
    <a href="https://github.com/shadow578/framework-arduino-hc32f46x/actions/workflows/unit_tests.yaml">
        <img src="https://github.com/shadow578/framework-arduino-hc32f46x/actions/workflows/unit_tests.yaml/badge.svg?branch=main" alt="ci status">
    </a>
</p>

This repository contains Arduino framework for the [HUADA HC32F460 Series](https://www.hdsc.com.cn/Category83-1487), for use with [PlatformIO](https://platformio.org/) and [the hc32f46x platform](https://github.com/shadow578/platform-hc32f46x).

> [!NOTE]
> while the primary use-case of this framework is for use with [Marlin](https://github.com/shadow578/Marlin-H32), other use-cases should work too.


## Getting Started

to get started using the HC32F460 Arduino core, use the following in your `platformio.ini`:

current *development* version:
```ini
[env:my_env]
platform = https://github.com/shadow578/platform-hc32f46x.git
framework = arduino
board = generic_hc32f460
```

latest release versions:
```ini
[env:my_env]
platform = https://github.com/shadow578/platform-hc32f46x/archive/1.0.0.zip
platform_packages =
  framework-hc32f46x-ddl @ https://github.com/shadow578/framework-hc32f46x-ddl/archive/2.2.1.zip
  framework-arduino-hc32f46x @ https://github.com/shadow578/framework-arduino-hc32f46x/archive/1.1.0.zip

framework = arduino
board = generic_hc32f460
```

> [!TIP]
> when pinning the version, check you're using the latest release(s) to benefit from the latest improvements.


### Examples

usage examples are available in the [examples](./examples) directory.


## Configuration and Documentation

for documentation on configuration and non-standard features, see following documents:

- [Arduino Core Configuration Options](./docs/CONFIGURATION_OPTIONS.md)
- [Checking the Version](./docs/VERSION_CHECK.md)
- [Dynamic Core Configuration](./docs/APP_CONFIG.md)
- [Assert Mechanism](./docs/ASSERT.md)
- [Panic Mechanism](./docs/PANIC.md)
- [Fault Handler](./docs/FAULT_HANDLER.md)


## License

this project is licensed under the [GPL-3.0](./LICENSE) license.


## Credits

This project could not have been possible without the following projects:

- [Voxelab-64/Aquila_X2](https://github.com/Voxelab-64/Aquila_X2) (original implementation)
- [kgoveas/Arduino-Core-Template](https://github.com/kgoveas/Arduino-Core-Template) (template for Arduino headers)
- [stm32duino/Arduino_Core_STM32](https://github.com/stm32duino/Arduino_Core_STM32) (misc. Arduino functions)
