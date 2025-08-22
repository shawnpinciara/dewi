#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>


//PINS
#define piezoPin1 8
#define modWheel 5
#define HX_OUT_PIN 3
#define HX_SCK_PIN 4


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

//OUT
//#include "./midi_out/midi_functions_serial.hpp"
// #include "./midi_out/midi_functions_usb.hpp"
#include "./serial_print.hpp"




//MODWHEEL
uint16_t modwheelVal = 999;
uint16_t modwheelValNew;

//TOUCH MODULE
//https://learn.adafruit.com/adafruit-mpr121-12-key-capacitive-touch-sensor-breakout-tutorial/overview
#include "Adafruit_MPR121.h"
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

void setup() {
  Serial.begin(115200);

  pinMode(modWheel, INPUT);
  pinMode(HX_SCK_PIN, OUTPUT);
  pinMode(HX_OUT_PIN, INPUT);

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

  Serial.println("Orientation Sensor Test"); Serial.println("");
  if (!bno.begin()) {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  tone(piezoPin1,300);
  delay(300);
  noTone(piezoPin1);
  monoMode(0);
  noteOn(0,60, 100);

  delay(1000);
  Serial.println("Starting the DEWI");

}


//MAIN
void loop() {
  breath = getBreath();
  
  Serial.println(breath);
  if (breath > threshold_bottom) {
    mpr121 = getButtonsState();
  //   velocity = map(breath,threshold_bottom,threshold_top,40,127);
  //   lastKey = currentKey;
  //   currentKey = (mpr121 & mask_key)>>4;

  //   if (currentKey == 0) {currentKey = lastKey;}
  //   currentOctave = (mpr121 & mask_octave)>>8; 
  //   //cc = (mpr121 & mask_cc) >>11;
  //   currentNote = ((octave+octaveArray[currentOctave])*12)+(noteArray[mpr121 & mask_note]+keyArray[currentKey]);
  //   if (breathAttack) { 
  //     breathAttack=false; breathRelease = true;
  //     velocity = map(breath,threshold_bottom,threshold_top,40,127);
  //     noteOn(0,currentNote, velocity);
  //     Serial.println("note on");
  //   } else { 
  //     if (currentNote != lastNote) {          
  //       noteOff(0,lastNote,velocity);    
  //       lastNote = currentNote;
  //       noteOn(0,currentNote, velocity);  
  //     } else {
  //       PolyphonicKeyPressure(0, currentNote, velocity);
  //     }
  //   }
  // } else { 
  //   if (breathRelease==true) { 
  //     breathAttack=true; breathRelease=false;
  //     noteOff(0,lastNote,velocity); 
  //   }  
  }
  // lastNote = currentNote;

    //could add VECTOR_ACCELEROMETER, VECTOR_MAGNETOMETER,VECTOR_GRAVITY...
  sensors_event_t orientationData , angVelocityData , linearAccelData, magnetometerData, accelerometerData, gravityData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
  bno.getEvent(&magnetometerData, Adafruit_BNO055::VECTOR_MAGNETOMETER);
  bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
  bno.getEvent(&gravityData, Adafruit_BNO055::VECTOR_GRAVITY);

  printEvent(&orientationData);
  printEvent(&angVelocityData);
  printEvent(&linearAccelData);
  printEvent(&magnetometerData);
  printEvent(&accelerometerData);
  printEvent(&gravityData);

  int8_t boardTemp = bno.getTemp();
  Serial.println();
  Serial.print(F("temperature: "));
  Serial.println(boardTemp);

  uint8_t system, gyro, accel, mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);
  Serial.println();
  Serial.print("Calibration: Sys=");
  Serial.print(system);
  Serial.print(" Gyro=");
  Serial.print(gyro);
  Serial.print(" Accel=");
  Serial.print(accel);
  Serial.print(" Mag=");
  Serial.println(mag);

  Serial.println("--");
}

void printEvent(sensors_event_t* event) {
  double x = -1000000, y = -1000000 , z = -1000000; //dumb values, easy to spot problem
  if (event->type == SENSOR_TYPE_ACCELEROMETER) {
    Serial.print("Accl:");
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
  }
  else if (event->type == SENSOR_TYPE_ORIENTATION) {
    Serial.print("Orient:");
    x = event->orientation.x;
    y = event->orientation.y;
    z = event->orientation.z;
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
    Serial.print("Unk:");
  }

  Serial.print("\tx= ");
  Serial.print(x);
  Serial.print(" |\ty= ");
  Serial.print(y);
  Serial.print(" |\tz= ");
  Serial.println(z);
}
