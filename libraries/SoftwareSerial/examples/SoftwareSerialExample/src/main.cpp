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

SoftwareSerial mySerial(/* RX */ PB0, /* TX */ PB1);

void setup() 
{
  Serial.begin(9600);
  Serial.println("Goodnight moon!");

  mySerial.begin(9600);
  mySerial.println("Hello, world?");
}

void loop() 
{
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }

  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
}
