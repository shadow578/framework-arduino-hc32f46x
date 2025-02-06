/*
 * Software serial multiple serial test
 *
 * Receives from the hardware serial, sends to software serial.
 * Receives from software serial, sends to hardware serial.
 *
 * created back in the mists of time
 * modified 25 May 2012
 * by Tom Igoe
 * based on Mikal Hart's example
 *
 * This example code is in the public domain.
 */
#include <Arduino.h>
#include <SoftwareSerial.h>

constexpr gpio_pin_t RX_PIN = PA15;
constexpr gpio_pin_t TX_PIN = PA9;

SoftwareSerial mySerial(/* RX */ RX_PIN, /* TX */ TX_PIN);

void setup() 
{
  mySerial.begin(9600);
  mySerial.println("Hello, world?");
}

void loop() 
{
  while (mySerial.available())
    mySerial.write(mySerial.read());

  mySerial.println(".");
  delay(100);
}
