/*
 * Software serial basic example.
 *
 * echos any character received on the software serial port back to the sender.
 * also allows to change the baud rate by sending a number from 1 to 8.
 * 
 * without changing the system clock, the maximum baud rate is around 9600 baud.
 */
#include <Arduino.h>
#include <SoftwareSerial.h>

constexpr gpio_pin_t RX_PIN = PA15;
constexpr gpio_pin_t TX_PIN = PA9;

SoftwareSerial mySerial(/* RX */ RX_PIN, /* TX */ TX_PIN);

void setup() 
{
  mySerial.begin(9600);
  mySerial.println("Hello, world!");
}

void loop() 
{
  while (mySerial.available())
  {
    const char c = mySerial.read();
    mySerial.write(c);

    uint32_t new_baud = 0;
    switch(c)
    {
      case '1': new_baud = 1200; break;
      case '2': new_baud = 2400; break;
      case '3': new_baud = 4800; break;
      case '4': new_baud = 9600; break;
      case '5': new_baud = 19200; break;
      case '6': new_baud = 38400; break;
      case '7': new_baud = 57600; break;
      case '8': new_baud = 115200; break;
      default: break;
    }
    if (new_baud != 0)
    {
      mySerial.print("Set baud to:");
      mySerial.print(new_baud);
      delay(100);
      mySerial.begin(new_baud);
      mySerial.println(" - Done");
    }
  }

  delay(10);
}
