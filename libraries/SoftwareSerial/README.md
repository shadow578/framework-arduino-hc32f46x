# Software Serial for the HC32F460

The Software Serial library allows serial (UART) communication on any digital pin of the board, bit-banging the protocol. 
It it possible to have multiple software serial ports.

The implementation of this library is based on the [SoftwareSerial library of the STM32duino project](https://github.com/stm32duino/Arduino_Core_STM32/blob/main/libraries/SoftwareSerial/).


## Configuration Options

To configure the library, you may add the following defines to your build environment.

| Name | Default | Description |
|-|-|-|
| `SOFTWARE_SERIAL_BUFFER_SIZE` | `32` | size of the receive buffer. it's highly likely that any transmission longer than this will be partially lost. |
| `SOFTWARE_SERIAL_OVERSAMPLE` | `3` | oversampling rate. Each bit period is equal to OVERSAMPLE ticks, and bits are sampled in the middle |
| `SOFTWARE_SERIAL_HALF_DUPLEX_SWITCH_DELAY` | `5` | bit periods before half duplex switches TX to RX |
| `SOFTWARE_SERIAL_TIMER_PRESCALER` | `2` | prescaler of the TIMER0. set according to PCLK1 and desired baud rate range |
| `SOFTWARE_SERIAL_TIMER0_UNIT` | `TIMER01B_config` | TIMER0 unit to use for software serial. Using TIMER01A is not recommended |
| `SOFTWARE_SERIAL_TIMER_PRIORITY` | `3` | interrupt priority of the timer interrupt |
| `SOFTWARE_SERIAL_FLUSH_CLEARS_RX_BUFFER` | `SOFTWARE_SERIAL_STM32_API_COMPATIBILITY` | behaviour of the `flush()` method. `0` = waits for pending TX to complete. `1` = clear RX buffer. STMduino library uses behaviour `1` |
| `SOFTWARE_SERIAL_STM32_API_COMPATIBILITY` | `0` | compatibility with STM32duino library. `0` = sensible API. `1` = compatible with STM32duino API. |


> [!TIP]
> for existing projects that originated from STM32duino, you may set `SOFTWARE_SERIAL_STM32_API_COMPATIBILITY` to `1` to maintain compatibility with the STM32duino library.


### Calculating `SOFTWARE_SERIAL_TIMER_PRESCALER`

to calculate, use the following c++ program

```cpp
#include <iostream>
#include <stdint.h>

float get_real_frequency(const uint32_t frequency, const uint16_t prescaler)
{
    const uint32_t base_frequency = 50000000; // 50 MHz PCLK1

    // calculate the compare value needed to match the target frequency
    // CMP = (base_freq / prescaler) / frequency
    uint32_t compare = (base_frequency / uint32_t(prescaler)) / frequency;

    if (compare <= 0 || compare > 0xFFFF)
    {
        return -1;
    }

    // calculate the real frequency
    float real_frequency = (base_frequency / prescaler) / compare;
    return real_frequency;
}


int main()
{
    const uint32_t baud = 9600;
    const uint32_t oversampling = 3;

    const uint32_t frequency = baud * oversampling;
    const uint16_t prescalers[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};

    float min_error = 100000;
    uint16_t best_prescaler = 0;
    for (auto p : prescalers)
    {
        const float real_frequency = get_real_frequency(frequency, p);
        const float error = std::abs(real_frequency - frequency);
        if (error < min_error)
        {
            min_error = error;
            best_prescaler = p;
        }

        std::cout << "Prescaler: " << static_cast<int>(p) 
                  << ", Real frequency: " << real_frequency 
                  << ", Error: " << error 
                  << std::endl;
    }

    float best_real_frequency = get_real_frequency(frequency, best_prescaler);
    float best_baud = best_real_frequency / oversampling;
    float baud_percent_error = (best_baud - baud) / baud * 100;
    std::cout << "Best prescaler: " << static_cast<int>(best_prescaler) 
              << ", Real frequency: " << best_real_frequency
              << ", Error: " << min_error 
              << ", Real baud rate: " << best_baud
              << ", Baud rate error: " << baud_percent_error << "%"
              << std::endl;
}
```
