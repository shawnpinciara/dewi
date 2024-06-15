
#include <Wire.h>
#include "../BREATH/breath_sensor.hpp"

void setup() {
    setupBreath();
    Serial.begin(9600);

}

void loop() {
    Serial.println(getBreath());
}