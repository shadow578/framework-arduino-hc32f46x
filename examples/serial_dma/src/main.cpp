/**
 * a simple example that demonstrates how to use the serial port with RX DMA enabled
 * to echo back any characters received from the serial port.
 * 
 * this example requires setting 'USART_RX_DMA_SUPPORT' to make dma functions available. 
 */
#include <Arduino.h>

void setup()
{
  // to enable serial rx dma, you have to assign the dma unit and channel
  // to the serial port before calling Serial.begin
  //
  // IMPORTANT:
  // you have to make sure that the dma unit and channel you are using
  // are not already in use, for example another serial port or SDIO
  Serial.enableRxDma(M4_DMA1, DmaCh0);

  // begin serial like normal
  Serial.begin(115200);
  Serial.println("Hello, World!");
}

void loop()
{
  // echo back any characters received from the serial port
  while (Serial.available() > 0)
  {
    char ch = Serial.read();
    Serial.print(ch);
  }
}
