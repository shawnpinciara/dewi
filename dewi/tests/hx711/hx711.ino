/* 
  Source: https://www.electroschematics.com/pressure-sensor-guide/
*/
#include "HX711.h"

HX711 scale;

void setup() {
  Serial.begin(9600);
  Serial.println("HX710B Demo with HX711 Library");
  Serial.println("Initializing the scale");

  // parameter "gain" is ommited; the default value 128 is used by the library
  // HX711.DOUT   - pin #26
  // HX711.PD_SCK - pin #27

  scale.begin(26, 27);

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());                  // print a raw reading from the ADC
  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));        // print the average of 20 readings from the ADC
  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));            // print the average of 5 readings from the ADC minus the tare weight (not set yet)
  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);         // print the average of 5 readings from the ADC minus tare weight (not set) divided

  // by the SCALE parameter (not set yet)
  scale.set_scale(2280.f);                       // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();                                  // reset the scale to 0

  Serial.println("After setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());                  // print a raw reading from the ADC
  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));        // print the average of 20 readings from the ADC
  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));            // print the average of 5 readings from the ADC minus the tare weight, set with tare()
  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);         // print the average of 5 readings from the ADC minus tare weight, divided

  // by the SCALE parameter set with set_scale
  Serial.println("Readings:");
}

void loop() {
  Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print("\t| average:\t");
  Serial.println(scale.get_units(10), 1);
  scale.power_down();                            // put the ADC in sleep mode
  delay(200);
  scale.power_up();
}