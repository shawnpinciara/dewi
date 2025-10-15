#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h> //da installare
#include "Adafruit_MPR121.h" //da installare
#include <utility/imumaths.h>
#include <MIDI.h>



/////////////////////// MAPPING
int vibrato_cc = 1; //41
int vibrato_min = 0;
int vibrato_max = 120;

int cutoff_cc = 74; //43
int cutoff_min = 50;
int cutoff_max =  127;

int modwheel_cc = 74; //43
int modwheel_min = 50;
int modwheel_max =  127;

//////////////////////

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
const int channel = 1;

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

//PINS
#define piezoPin1 8
#define modWheelPin 14
#define HX_OUT_PIN 3
#define HX_SCK_PIN 4


//binary no midi note map es: binary:0001 -> decimal: 1 -->note: D (1 in midi value)
const int noteArray[16] = {0,2,11,4,12,9,5,7,1,3,12,5,16,10,6,8};
const int keyArray[16] =  {0,2,11,4,0,9,5,7,1,3,0,5,1,10,6,8};
const int octaveArray[16] = {0,1,-1,0,-2,-1,-3,0,0,0,0,0,0,0,0,0}; //missing button is X000, change octave is 0X00, -1 (X) e +1 (Y) octave is 00XY
int octave = 5;


#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable
unsigned int velocity = 60;
unsigned long threshold_bottom = 8736143; //base: 8067761
unsigned long threshold_top = 10000000;
unsigned long breath = 0; //16689194 base, 8595203 piano,10305762 forte (breath sensor data)
const uint16_t mask_key = 0b0000000011110000;
const uint16_t mask_note = 0b0000000000001111;
const uint16_t mask_octave = 0b0000011100000000;
//const uint16_t mask_cc = 0b0000100000000000;
uint16_t currentNote = 0;
uint16_t lastNote = 0b0111111111111111;
uint16_t sendNote;
uint16_t currentKey = 0;
uint16_t lastKey = 0;
uint16_t currentOctave = 0;
uint16_t lastOctave = 0;
uint16_t mpr121 = 0;
bool breathAttack = true;
bool breathRelease = false;
int cc = 0;
int cc_debounce = 1;
int bank = 0;
int cutoff = 0;
int vibrato = 0;


//OUT
// #include "./serial_print.hpp"
#include "./midi_functions_serial.hpp"




//MODWHEEL
uint16_t modwheelVal = 999;
uint16_t modwheelValNew;

//TOUCH MODULE
//https://learn.adafruit.com/adafruit-mpr121-12-key-capacitive-touch-sensor-breakout-tutorial/overview

Adafruit_MPR121 mpr = Adafruit_MPR121();


/* Set the delay between fresh samples */
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);



uint16_t getButtonsState() {
  return mpr.touched(); //valore letto da sensore (12 bit: 00000000000)
}



//BREATH SENSOR. HX710B
//https://swharden.com/blog/2022-11-14-hx710b-arduino/


enum HX_MODE { NONE, DIFF_10Hz, TEMP_40Hz, DIFF_40Hz};
const byte HX_MODE = DIFF_40Hz;

//BREATH READ FUNCTION
long getBreath() {
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
  long result = 0;
  result += (long)data[2] << 16;
  result += (long)data[1] << 8;
  result += (long)data[0];
  return result;
}

long getBreath2() {
  while (digitalRead(HX_OUT_PIN)) {}

  // read 24 bits
  long result = 0;
  for (int i = 0; i < 24; i++) {
    digitalWrite(HX_SCK_PIN, HIGH);
    digitalWrite(HX_SCK_PIN, LOW);
    result = result << 1;
    if (digitalRead(HX_OUT_PIN)) {
      result++;
    }
  }

  // get the 2s compliment
  result = result ^ 0x800000;

  // pulse the clock line 3 times to start the next pressure reading
  for (char i = 0; i < 3; i++) {
    digitalWrite(HX_SCK_PIN, HIGH);
    digitalWrite(HX_SCK_PIN, LOW);
  }

  return result;
}

void dump_regs() {
  Serial.println("========================================");
  Serial.println("CHAN 00 01 02 03 04 05 06 07 08 09 10 11");
  Serial.println("     -- -- -- -- -- -- -- -- -- -- -- --"); 
  // CDC
  Serial.print("CDC: ");
  for (int chan=0; chan<12; chan++) {
    uint8_t reg = mpr.readRegister8(0x5F+chan);
    if (reg < 10) Serial.print(" ");
    Serial.print(reg);
    Serial.print(" ");
  }
  Serial.println();
  // CDT
  Serial.print("CDT: ");
  for (int chan=0; chan<6; chan++) {
    uint8_t reg = mpr.readRegister8(0x6C+chan);
    uint8_t cdtx = reg & 0b111;
    uint8_t cdty = (reg >> 4) & 0b111;
    if (cdtx < 10) Serial.print(" ");
    Serial.print(cdtx);
    Serial.print(" ");
    if (cdty < 10) Serial.print(" ");
    Serial.print(cdty);
    Serial.print(" ");
  }
  Serial.println();
  Serial.println("========================================");
}
void allNotesOFF() {
  for (int i=0;i<127;i++) {
    MIDI.sendNoteOff(i,velocity,channel); 
  }
}
void setup() {
  Serial.begin(115200);

  MIDI.begin();

  pinMode(modWheelPin, INPUT);
  pinMode(HX_SCK_PIN, OUTPUT);
  pinMode(HX_OUT_PIN, INPUT);

  //MPR121 setup
  while (!Serial1) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }

  Serial.println("cap.begin..");
  if (!mpr.begin(0x5A, &Wire,10,8,true)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
  delay(100);

  Serial.println("Initial CDC/CDT values:");
  //dump_regs(); //TODO: forse bisogna rimuovere questo

  //mpr.setAutoconfig(true);
  //mpr.setThreshholds(0x10, 0x0C); //10 -11 TODO: bisogna mettere valori che permettono letture stabili

  Serial.println("After autoconfig CDC/CDT values:");
  dump_regs();



  Serial.println("Orientation Sensor Test"); Serial.println("");
  if (!bno.begin()) {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  
  monoMode(channel);


  delay(1000);
  Serial.println("Starting the DEWI");
  monoMode(channel);
  allNotesOFF();
  breath = 0;

  for (int i=0; i<10; i++) {
    breath = getBreath();
  }

  tone(piezoPin1,300);
  delay(300);
  noTone(piezoPin1);

}
void printBinary(int number) {
  // Itera da 31 (il bit più significativo di un int a 32 bit) fino a 0.
  for (int i = 16; i >= 0; i--) {
    // Utilizza un'operazione bitwise AND per verificare se il bit i-esimo è impostato.
    // (1 << i) crea un '1' nella posizione del bit i.
    if ((number >> i) & 1) {
      Serial.print("1");
    } else {
      Serial.print("0");
    }
  }
  Serial.println(); // Aggiunge un ritorno a capo alla fine
}


void updateDewi() {

  breath = getBreath();
  mpr121 = getButtonsState();
  

  if (breath > threshold_bottom && breath < threshold_top) {
    mpr121 = getButtonsState();
    velocity = map(breath,threshold_bottom,threshold_top,40,127);
    currentKey = (mpr121 & mask_key)>>4;

    if (currentKey == 0) {
      currentKey = lastKey;
    }

    currentOctave = (mpr121 & mask_octave)>>8; 
    currentNote = ((octave+octaveArray[currentOctave])*12)+(noteArray[mpr121 & mask_note]+keyArray[currentKey]);
    // int t0 = millis();

    // while ((millis() - t0) < 20) {
    //     // busy waiting
    //     mpr121 = getButtonsState();
    // }
    

    if (breathAttack) { 
      breathAttack=false; breathRelease = true;
      int t0 = millis();
      while (millis()-t0 < 15) {
        mpr121 = getButtonsState();
        breath = getBreath();
      }
      velocity = map(breath,threshold_bottom,threshold_top,1,127);
      currentNote = ((octave+octaveArray[currentOctave])*12)+(noteArray[mpr121 & mask_note]+keyArray[currentKey]);
      MIDI.sendNoteOn(currentNote,velocity,channel);
      Serial.print("note on with vel: ");
      Serial.println(velocity);
    } else { 
      if (currentNote != lastNote) {          
        //MIDI.sendNoteOff(lastNote,velocity,channel);  
        lastNote = currentNote;
        int t0 = millis();
        while (millis()-t0 < 15) {
          mpr121 = getButtonsState();
        }
        currentNote = ((octave+octaveArray[currentOctave])*12)+(noteArray[mpr121 & mask_note]+keyArray[currentKey]);
        MIDI.sendNoteOn(currentNote,velocity,channel);
        Serial.println("note on while breathing");
      }
    }
    cutoff = constrain(map(breath,threshold_bottom,threshold_top,cutoff_min,cutoff_max),cutoff_min,cutoff_max);
    MIDI.sendControlChange(cutoff_cc,cutoff,channel);
    MIDI.sendControlChange(vibrato_cc,vibrato,channel);
    modwheelVal = constrain(analogRead(modWheelPin),350,650);
    modwheelVal = map(modwheelVal,650,350,MIDI_PITCHBEND_MIN,MIDI_PITCHBEND_MAX);
    
    //MIDI.sendPitchBend((int)modwheelVal,channel);
    Serial.print("modwheel ");
    Serial.print(modwheelVal);
    Serial.print(" ,cutoff ");
    Serial.print(cutoff);
    Serial.print(" ,vibrato ");
    Serial.print(vibrato);
    Serial.print(" ,breath ");
    Serial.print(breath);
    Serial.print(" , buttons: ");
    printBinary(mpr121);

  } else { 
    if (breathRelease==true) { 
      breathAttack=true; breathRelease=false;
      MIDI.sendNoteOff(lastNote,velocity,channel);  
      Serial.println("note off");
      allNotesOFF();
    }

  }

  lastKey = currentKey;
  
}
void updateDewiPython() {
    breath = getBreath();
    // Serial.print("Breath: ");
    // Serial.println(breath);
    mpr121 = getButtonsState();
    // Serial.print("Mpr121: ");
    // Serial.println(mpr121);
    if (breath > threshold_bottom) {
        mpr121 = getButtonsState();
        
        // key handling
        lastKey = currentKey;
        currentKey = (mpr121 & mask_key) >> 4;
        if (currentKey == 0) {
            currentKey = lastKey;
        }
        
        currentOctave = (mpr121 & mask_octave) >> 8;
        currentNote = mpr121 & mask_note;

        if (currentNote != lastNote) {
            int t0 = millis();

            while ((millis() - t0) < 20) {
                // busy waiting
                mpr121 = getButtonsState();
            }
            
            currentNote = mpr121 & mask_note;
            velocity = round(map(breath, threshold_bottom, threshold_top, 40, 127));

            int noteOffToSend = ((octave + octaveArray[currentOctave]) * 12) + (noteArray[lastNote] + keyArray[currentKey]);
            MIDI.sendNoteOff(noteOffToSend,velocity,channel);
            //noteOff(0, noteOffToSend, velocity);
            
            lastNote = currentNote;
            int noteOnToSend = ((octave + octaveArray[currentOctave]) * 12) + (noteArray[currentNote] + keyArray[currentKey]);
            MIDI.sendNoteOn(noteOnToSend,velocity,channel);
            //noteOn(0, noteOnToSend, velocity);
        } else {
            // noteToSend = ((octave+octaveArray[currentOctave])*12)+(noteArray[currentNote]+keyArray[currentKey]);
            PolyphonicKeyPressure(0, currentNote, velocity);
        }
    } else {
        allNotesOff(channel);
    }
}

//MAIN
void loop() {
  updateDewi();

  //could add VECTOR_ACCELEROMETER, VECTOR_MAGNETOMETER,VECTOR_GRAVITY...
  sensors_event_t orientationData; //, angVelocityData , linearAccelData, magnetometerData, accelerometerData, gravityData
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  handleEvent(&orientationData);

  uint8_t system, gyro, accel, mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);


}


void handleEvent(sensors_event_t* event) {
  
  // bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  // bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
  // bno.getEvent(&magnetometerData, Adafruit_BNO055::VECTOR_MAGNETOMETER);
  // bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
  // bno.getEvent(&gravityData, Adafruit_BNO055::VECTOR_GRAVITY);

  //handleEvent(&orientationData);
  // printEvent(&angVelocityData);
  // printEvent(&linearAccelData);
  // printEvent(&magnetometerData);
  // printEvent(&accelerometerData);
  // printEvent(&gravityData);
  double x = -1000000, y = -1000000 , z = -1000000; //dumb values, easy to spot problem
  if (event->type == SENSOR_TYPE_ACCELEROMETER) {
    Serial.print("Accl:");
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
  }
  else if (event->type == SENSOR_TYPE_ORIENTATION) {
    //Serial.print("Orient:");
    x = event->orientation.x;
    y = event->orientation.y;
    z = constrain(event->acceleration.z,-60,0);
    //Serial.println(z);
    vibrato = map(z,-60,0,vibrato_min,vibrato_max);
    
  }
  else if (event->type == SENSOR_TYPE_MAGNETIC_FIELD) {
    Serial.print("Mag:");
    x = event->magnetic.x;
    y = event->magnetic.y;
    z = event->magnetic.z;
  }
  else if (event->type == SENSOR_TYPE_GYROSCOPE) {
    Serial.print("Gyro:");
    x = event->gyro.x;
    y = event->gyro.y;
    z = event->gyro.z;
  }
  else if (event->type == SENSOR_TYPE_ROTATION_VECTOR) {
    Serial.print("Rot:");
    x = event->gyro.x;
    y = event->gyro.y;
    z = event->gyro.z;
  }
  else if (event->type == SENSOR_TYPE_LINEAR_ACCELERATION) {
    Serial.print("Linear:");
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
  }
  else if (event->type == SENSOR_TYPE_GRAVITY) {
    Serial.print("Gravity:");
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
  }
  else {
    //Serial.print("Unk:");
  }

  // Serial.print("\tx= ");
  // Serial.print(x);
  // Serial.print(" |\ty= ");
  // Serial.print(y);
  // Serial.print(" |\tz= ");
  // Serial.println(z);
  // int8_t boardTemp = bno.getTemp();
  // Serial.println();
  // Serial.print(F("temperature: "));
  // Serial.println(boardTemp);
}
