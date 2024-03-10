#include <Wire.h>
#include "Adafruit_MPR121.h"
#include <Encoder.h>

//#define midi_ble

#ifdef midi_ble
  #include <BLEMIDI_Transport.h>
  #include <hardware/BLEMIDI_ESP32.h>
  BLEMIDI_CREATE_INSTANCE("DEWI",MIDI);
#else  
  #include "MIDIUSB.h"
  //MIDI_CREATE_DEFAULT_INSTANCE();
#endif

#define serial_midi

#ifdef serial_midi
  #include "midi_functions_serial.hpp"
#else
  #include "midi_functions_usb.hpp"
#endif

//LINUX port:
//sudo chmod 0777 /dev/ttyACM0

//PINS:
//LEFT HAND: near:  15 - 2 - 4 - 32
//RIGHT HAND: near: 12 - 14 - 27 - 33

//BREATH SENSOR
const int HX_OUT_PIN = 4;
const int HX_SCK_PIN = 5;
enum HX_MODE { NONE, DIFF_10Hz, TEMP_40Hz, DIFF_40Hz};
const byte HX_MODE = DIFF_40Hz;

//MODWHEEL
#define modWheel A0
int modwheelVal = 999;
int modwheelValNew;

//POT
#define pot A1
int potVal = 999;
int potValNew;

//ENCODER
Encoder encoder(14, 15);
long encoderPos  = -999;
int encoderVal = 56;

const int piezoPin1 = 6;
//binary no midi note map es: binary:0001 -> decimal: 1 -->note: D (1 in midi value)
const int noteArray[16] = {0,2,11,4,12,9,5,7,1,3,12,5,16,10,6,8};
const int keyArray[16] =  {0,2,11,4,0,9,5,7,1,3,0,5,16,10,6,8};
const int octaveArray[16] = {0,1,-1,0,-2,-1,-3,0,0,0,0,0,0,0,0,0}; //missing button is X000, change octave is 0X00, -1 (X) e +1 (Y) octave is 00XY
int octave = 5;

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

Adafruit_MPR121 mpr = Adafruit_MPR121();



//BREATH READ FUNCTION
void computeHX() {
  // pulse clock line to start a reading
  for (char i = 0; i < HX_MODE; i++) {
    digitalWrite(HX_SCK_PIN, HIGH);
    digitalWrite(HX_SCK_PIN, LOW);
  }
  // wait for the reading to finish
  while (digitalRead(HX_OUT_PIN)) {}

  // read the 24-bit pressure as 3 bytes using SPI
  byte data[3];
  for (byte j = 3; j--;) {
    data[j] = shiftIn(HX_OUT_PIN, HX_SCK_PIN, MSBFIRST);
  }
  data[2] ^= 0x80;  // see note
  // shift the 3 bytes into a large integer
  long result;
  result += (long)data[2] << 16;
  result += (long)data[1] << 8;
  result += (long)data[0];
  breath = result;
}

void setup() {
  //BREATH SENSOR
  pinMode(HX_SCK_PIN, OUTPUT);
  pinMode(HX_OUT_PIN, INPUT);
  pinMode(modWheel, INPUT);
  pinMode(pot, INPUT);
  
  Serial1.begin(31250);

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
  tone(piezoPin1,300);
  delay(300);
  noTone(piezoPin1);
  monoMode(0);
}

void updateBreath() {
    computeHX();
    //int endComputeHX = millis();

    if (breath > threshold_bottom) {
      //lettura valori e manipolazione i bit
      velocity = map(breath,threshold_bottom,threshold_top,40,127);
      lastKey = currentKey;
      mpr121 = mpr.touched(); //valore letto da sensore (12 bit: 00000000000)

      currentKey = (mpr121 & mask_key)>>4;
      if (currentKey == 0) {
        currentKey = lastKey;
        //Serial.print(" Current KEY IS 0");
      }
      currentOctave = (mpr121 & mask_octave)>>8; 
        //cc = (mpr121 & mask_cc) >>11;
      currentNote = ((octave+octaveArray[currentOctave])*12)+(noteArray[mpr121 & mask_note]+keyArray[currentKey]);
      //gestione del fiato vera e propria
      if (breathAttack) { //all'inizio della soffiata (va una volta sola)
        breathAttack=false; //cambio lo stato cosi non ci entro piu in questo if
        breathRelease = true; //accendo la possibilià di entrare nell'if di quando interromperò il fiato
        velocity = map(breath,threshold_bottom,threshold_top,40,127);
        noteOn(currentNote, velocity,0);
        /*
        Serial.print("Breath " + String(breath));
        Serial.print(" , Threshold " + String(threshold_bottom));
        Serial.print(" , Velocity " + String(velocity));
        Serial.print(" , CurrentKey " + String(keyArray[currentKey]));
        Serial.print(" , CurrentOctave " + String(currentOctave));
        Serial.println(" , Breath>threshold: " + String(breath > threshold_bottom));*/
      } else { //durante la soffiata (si ripete continuamente)
        if (currentNote != lastNote) { //se il valore letto da sensore è diverso da quello letto in precedenza          
          noteOff(lastNote,velocity,0);    //fai smettere di suonare la nota precedente (perchè siamo in monofonia)
          lastNote = currentNote; //aggiorna valore di nota precedente
          noteOn(currentNote, velocity,0);  //inizia a suonare la nota premuta
          //Serial.println(currentKey); //log
        } else {
          channelPressure(currentNote, velocity);
        }
      }
    } else { 
      if (breathRelease==true) { //funziona una volta sola solamente quando rilascio il fiato dopo aver soffiato
        breathAttack=true;
        breathRelease=false;
        noteOff(lastNote,velocity,1); //fai smettere di suonare l'ultima nota suonata
      }  
    }

    lastNote = currentNote;

}

void updateEncoder() {
  long encoderPosNew = encoder.read();
  if (encoderPos != encoderPosNew) {
    if (encoderPos > encoderPosNew) {
      encoderVal++;
    } else {
      encoderVal--;
    }
    encoderPos = encoderPosNew;
    controlChange(encoderVal,1,0);
    Serial.println(encoderVal);
  }
}

void updatePot() {
  potValNew = analogRead(pot);
  if (potVal != potValNew) {
    potVal = potValNew;
    controlChange(map(potVal,1023,0,0,127),7,1);
    //Serial.print(potVal);
  }
}

void updateModwheel() {
  modwheelValNew = analogRead(modWheel); // min (avanti) 350 - 500 - 650 max
  if (modwheelVal != modwheelValNew) {
    modwheelVal = map(modwheelValNew,650,350,0,127);
    controlChange(modwheelVal,7,1);
    Serial.println(modwheelVal);
  }
}

//MAIN
void loop() {
  updateBreath();
  updateEncoder();
  //updateModwheel();
  //updatePot();
}
