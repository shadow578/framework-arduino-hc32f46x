# `USART_RX_DMA_SUPPORT` Option

when defining the `USART_RX_DMA_SUPPORT` option, the `Usart` driver class will support receiving data using DMA.
to use this feature, call `Usart::enableRxDma()` before calling `Usart::begin()`.
you must pass a unused DMA peripheral and channel to `Usart::enableRxDma()`. failing to do so may result in undefined behaviour.

> [!NOTE]
> enabling this option will increase the flash usage by about 1KB.


# Example Usage

for a more detailed example, see the [`serial_dma` example](../../examples/serial_dma).

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
    Serial.enableRxDma(M4_DMA1, DmaCh0);
    Serial.begin(115200);
}

void loop() {}
```
