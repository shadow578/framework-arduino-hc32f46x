/*
 * Software serial basic example.
 *
 * echos any character received on the software serial port back to the sender.
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
    mySerial.write(mySerial.read());

  delay(10);
}
