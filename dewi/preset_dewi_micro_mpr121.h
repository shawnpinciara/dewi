//BREATH
#include "./breath_sensor.hpp"

//BUTTONS
#include "./mpr121.hpp"

//OUT
//#include "../OUT/midi/midi_functions_serial.hpp"
// #include "./midi_functions_usb.hpp"
#include "./serial_print.hpp"


#define modWheel A0
#define pot A1

//MODWHEEL
uint16_t modwheelVal = 999;
uint16_t modwheelValNew;

//POT
int potVal = 999;
int potValNew;



void setupMisc(){};

void setupControls() {
  pinMode(modWheel, INPUT);
  pinMode(pot, INPUT);
}


void updatePot() {
  potValNew = analogRead(pot);
  
  if (potVal != potValNew) {
    potVal = potValNew;
    
  }
  controlChange(0,117,map(potVal,1023,0,0,127));
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
  }
}

void updateControls() {
  //updateEncoder();
  //updateModwheel();
  //updatePot();
}