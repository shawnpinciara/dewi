//BREATH
#include "../BREATH/breath_sensor.hpp"

//BUTTONS
#include "../BUTTONS/mpr121.hpp"

//OUT
#include "../OUT/midi/midi_functions_serial.hpp"

//MODWHEEL
#define modWheel A0
uint16_t modwheelVal = 999;
uint16_t modwheelValNew;

//POT
#define pot A1
int potVal = 999;
int potValNew;

//ENCODER
Encoder encoder(14, 15);
long encoderPos  = -999;
int encoderVal = 56;

void setupMisc(){};

void setupControls() {
  pinMode(modWheel, INPUT);
  pinMode(pot, INPUT);
}

void updateEncoder() {
  long encoderPosNew = encoder.read();
  if (encoderPos != encoderPosNew) {
    if (encoderPos > encoderPosNew) {
      encoderVal++;
      //encoderVal=encoderVal+10;
    } else {
      encoderVal--;
      //encoderVal=encoderVal-10;
    }
    if (encoderVal > 127)
    {
      encoderVal--;
      //encoderVal=encoderVal-10;
    }
    else if (encoderVal < 0){
      encoderVal++;
      //encoderVal=encoderVal+10;
    }
    encoderPos = encoderPosNew;
    //controlChange(0,1,modwheelVal);
  }
  controlChange(0,81,encoderVal);
  Serial.println(encoderVal);
}

void updatePot() {
  potValNew = analogRead(pot);
  
  if (potVal != potValNew) {
    potVal = potValNew;
    
  }
  controlChange(0,117,map(potVal,1023,0,0,127));
  //Serial.print(potValNew);
}

void updateModwheel() {
  modwheelValNew = analogRead(modWheel); // min (avanti) 350 - 500 - 650 max
  if (modwheelVal != modwheelValNew) {
    modwheelVal = modwheelValNew;
    if (modwheelVal <= 500-5) {
      modwheelVal = map(modwheelValNew,500-5,350,8192,32639); //TODO: to analyze if true
      //controlChange(0,74,modwheelVal);
      //pitchWheel(0,modwheelVal);
    } else if (modwheelVal >= 500+5) {
      modwheelVal = map(modwheelValNew,500+5,650,8192,0);
      //controlChange(0,74,modwheelVal);
      //pitchWheel(0,modwheelVal);
    } else {
      //controlChange(0,74,modwheelVal);
      //pitchWheel(0,8192); //middle rest position
    }
    controlChange(0,1,modwheelVal);
    Serial.println(modwheelVal);
  }
}

void updateControls() {
  updateEncoder();
  //updateModwheel();
  updatePot();
}