#include <Wire.h>

//OPTIONS:
#include "./preset_dewi_micro_mpr121.h"
//#include "presets/preset_shawn_minidewi.h"

const int piezoPin1 = 2;

//binary no midi note map es: binary:0001 -> decimal: 1 -->note: D (1 in midi value)
const int noteArray[16] = {0,2,11,4,12,9,5,7,1,3,12,5,16,10,6,8};
const int keyArray[16] =  {0,2,11,4,0,9,5,7,1,3,0,5,16,10,6,8};
const int octaveArray[16] = {0,1,-1,0,-2,-1,-3,0,0,0,0,0,0,0,0,0}; //missing button is X000, change octave is 0X00, -1 (X) e +1 (Y) octave is 00XY
int octave = 5;

#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable
unsigned int velocity = 60;
unsigned long threshold_bottom = 8736143; //base: 8067761
unsigned long threshold_top = 14000000;
unsigned long breath = 0; //16689194 base, 8595203 piano,10305762 forte (breath sensor data)
const uint16_t mask_key = 0b0000000011110000;
const uint16_t mask_note = 0b0000000000001111;
const uint16_t mask_octave = 0b0000111100000000;
//const uint16_t mask_cc = 0b0000100000000000;
uint16_t currentNote = 0;
uint16_t lastNote = 0b0111111111111111;
uint16_t sendNote;
uint16_t currentKey = 0;
uint16_t lastKey = 1;
uint16_t currentOctave = 0;
uint16_t lastOctave = 0;
uint16_t mpr121 = 0;
bool breathAttack = true;
bool breathRelease = false;
int cc = 0;
int cc_debounce = 1;
int bank = 0;

void setup() {
  

  Serial.begin(115200);

  setupBreath();
  setupButtons();
  setupControls();
  setupMisc();

  tone(piezoPin1,300);
  delay(300);
  noTone(piezoPin1);
  monoMode(0);
  noteOn(0,60, 100);

}

// #ifdef mozzi:
// const int onTime=200; // in ms
// bool timeDebounce = true;
// unsigned long startTime =0;
// void updateControl(){
//   if(timeDebounce && millis()>startTime+onTime) {
//     timeDebounce = false;
//     startTime = millis();
//     //Serial.println(startTime);
//     updateBreath();
//   } 
  
//   if (millis()<startTime+onTime) {
//     timeDebounce = true;
//     //Serial.println(startTime);
//   }
// }
// #endif

void updateBreath() {
    breath = getBreath();
    Serial.println("AAAAAA");
    Serial.println(breath);
    if (breath > threshold_bottom) {
      mpr121 = getButtonsState();
      velocity = map(breath,threshold_bottom,threshold_top,40,127);
      lastKey = currentKey;
      currentKey = (mpr121 & mask_key)>>4;

      if (currentKey == 0) {currentKey = lastKey;}
      currentOctave = (mpr121 & mask_octave)>>8; 
      //cc = (mpr121 & mask_cc) >>11;
      currentNote = ((octave+octaveArray[currentOctave])*12)+(noteArray[mpr121 & mask_note]+keyArray[currentKey]);
      if (breathAttack) { 
        breathAttack=false; breathRelease = true;
        velocity = map(breath,threshold_bottom,threshold_top,40,127);
        noteOn(0,currentNote, velocity);
        Serial.println("note on");
      } else { 
        if (currentNote != lastNote) {          
          noteOff(0,lastNote,velocity);    
          lastNote = currentNote;
          noteOn(0,currentNote, velocity);  
        } else {
          PolyphonicKeyPressure(0, currentNote, velocity);
        }
      }
    } else { 
      if (breathRelease==true) { 
        breathAttack=true; breathRelease=false;
        noteOff(0,lastNote,velocity); 
      }  
    }
    lastNote = currentNote;
}


//MAIN
void loop() {
  
  // #ifndef mozzi:
  updateBreath();
  updateControls();
  // #endif

  // #ifdef mozzi:
  //   audioHook();
  // #endif
}
