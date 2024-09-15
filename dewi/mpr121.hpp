//https://learn.adafruit.com/adafruit-mpr121-12-key-capacitive-touch-sensor-breakout-tutorial/overview
#include "Adafruit_MPR121.h"
Adafruit_MPR121 mpr = Adafruit_MPR121();

void setupButtons() {
  //MPR121 setup
  while (!Serial1) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }
  Serial.println("Adafruit MPR121"); 
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!mpr.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
}

uint16_t getButtonsState() {
  return mpr.touched(); //valore letto da sensore (12 bit: 00000000000)
}