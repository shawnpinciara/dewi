#include <Wire.h>
#include <Encoder.h>

//OPTIONS:
#include "presets/preset_dewi_micro_mpr121.h"
//#include "presets/preset_shawn_minidewi.h"


const int piezoPin1 = 2;

//binary no midi note map es: binary:0001 -> decimal: 1 -->note: D (1 in midi value)
const int noteArray[16] = {0,2,11,4,12,9,5,7,1,3,12,5,16,10,6,8};
const int keyArray[16] =  {0,2,11,4,0,9,5,7,1,3,0,5,16,10,6,8};
const int octaveArray[16] = {0,1,-1,0,-2,-1,-3,0,0,0,0,0,0,0,0,0}; //missing button is X000, change octave is 0X00, -1 (X) e +1 (Y) octave is 00XY
int octave = 5;

#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable
unsigned int velocity = 60;
unsigned long threshold_bottom = 8100000;
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
  
  //Serial1.begin(31250);
  Serial.begin(115200);

  setupBreath();
  setupButtons();
  setupControls();
  setupMisc();

  tone(piezoPin1,300);
  delay(300);
  noTone(piezoPin1);
  monoMode(0);

}

void updateBreath() {
    breath = getBreath();
    if (breath > threshold_bottom) {
      //lettura valori e manipolazione i bit
      velocity = map(breath,threshold_bottom,threshold_top,40,127);
      lastKey = currentKey;
      mpr121 = getButtonsState();
      currentKey = (mpr121 & mask_key)>>4;
      if (currentKey == 0) {currentKey = lastKey;}
      currentOctave = (mpr121 & mask_octave)>>8; 
      //cc = (mpr121 & mask_cc) >>11;
      currentNote = ((octave+octaveArray[currentOctave])*12)+(noteArray[mpr121 & mask_note]+keyArray[currentKey]);
      //gestione del fiato vera e propria
      if (breathAttack) { //all'inizio della soffiata (va una volta sola)
        breathAttack=false; //cambio lo stato cosi non ci entro piu in questo if
        breathRelease = true; //accendo la possibilià di entrare nell'if di quando interromperò il fiato
        velocity = map(breath,threshold_bottom,threshold_top,40,127);
        noteOn(0,currentNote, velocity);
      } else { //durante la soffiata (si ripete continuamente)
        if (currentNote != lastNote) { //se il valore letto da sensore è diverso da quello letto in precedenza          
          noteOff(0,lastNote,velocity);    //fai smettere di suonare la nota precedente (perchè siamo in monofonia)
          lastNote = currentNote; //aggiorna valore di nota precedente
          noteOn(0,currentNote, velocity);  //inizia a suonare la nota premuta      
        } else {
          channelPressure(0, currentNote, velocity);
        }
      }
    } else { 
      if (breathRelease==true) { //funziona una volta sola solamente quando rilascio il fiato dopo aver soffiato
        breathAttack=true;
        breathRelease=false;
        noteOff(0,lastNote,velocity); //fai smettere di suonare l'ultima nota suonata
      }  
    }
    lastNote = currentNote;
}

#ifdef mozzi:
void updateControl() {
  // put changing controls in here
  updateBreath();
  updateControls();
}
#endif

//MAIN
void loop() {
  #ifndef mozzi:
    updateBreath();
    updateControls();
  #endif
  #ifdef mozzi:
    audioHook();
  #endif
}
