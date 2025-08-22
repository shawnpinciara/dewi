#include "HX711.h"

// HX711 circuit wiring
const int HX711_OUT = 26;
const int HX711_SCK = 27;

HX711 hx711;

void setup() {
  Serial.begin(9600);
  hx711.begin(HX711_OUT, HX711_SCK);
}

void loop() {

  if (hx711.is_ready()) {
    long reading = hx711.read();
    Serial.print("HX711 reading: ");
    Serial.println(reading);
  }

  delay(50);
  
}
