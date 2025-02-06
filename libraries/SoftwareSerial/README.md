# Software Serial for the HC32F460



## Calculating `SOFTWARE_SERIAL_TIMER_PRESCALER`

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



