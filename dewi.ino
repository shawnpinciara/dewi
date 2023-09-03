#include <Wire.h>
#include "Adafruit_MPR121.h"

//#define midi_ble

#ifdef midi_ble
  #include <BLEMIDI_Transport.h>
  #include <hardware/BLEMIDI_ESP32.h>
  BLEMIDI_CREATE_INSTANCE("DEWI",MIDI);
#else  
  #include "MIDIUSB.h"
  //MIDI_CREATE_DEFAULT_INSTANCE();
#endif

//LINUX port:
//sudo chmod 0777 /dev/ttyACM0

//PINS:
//LEFT HAND: near:  15 - 2 - 4 - 32
//RIGHT HAND: near: 12 - 14 - 27 - 33

//BREATH SENSOR
const int HX_OUT_PIN = 0;
const int HX_SCK_PIN = 2;
enum HX_MODE { NONE, DIFF_10Hz, TEMP_40Hz, DIFF_40Hz};
const byte HX_MODE = DIFF_40Hz;
//16689194 base, 8595203 piano,10305762 forte

//binary no midi note map es: binary:0001 -> decimal: 1 -->note: D (1 in midi value)
const int noteArray[16] = {0,2,11,4,12,9,5,7,1,3,0,0,16,10,6,8};
// int noteIndex[] = {1,3,12,5,1,10,6,8,2,4,0,0,5,11,7,9};
char scale[] = {'C','C#','D','D#','E','F','F#','G','G#','A','A#','B','C','C#','D','D#','E','F','F#','G','G#','A','A#','B'};
// boolean ottavaSopra = false;
int octave = 5;
int velocity = 60;
int threshold_bottom = 8000000;
int threshold_top = 13000000;
unsigned long breath = 0;
const uint16_t mask_key = 0b0000000011110000;
const uint16_t mask_note = 0b0000000000001111;
const uint16_t mask_octave = 0b0000111100000000;
const uint16_t mask_cc = 0b0000100000000000;
uint16_t currentNote = 0;
uint16_t lastNote = 0b0111111111111111;
uint16_t sendNote;
uint16_t currentKey = 0;
uint16_t lastKey = 0;
uint16_t currentOctave = 0;
uint16_t lastOctave = 0;
uint16_t mpr121 = 0;
boolean breathAttack = true;
boolean breathRelease = false;
int cc = 0;
int cc_debounce = 1;
int bank = 0;

Adafruit_MPR121 mpr = Adafruit_MPR121();

//MIDI FUNCTIONS:
// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).
void noteOn(byte pitch, byte velocity, byte channel) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}
void noteOff(byte pitch, byte velocity, byte channel) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}
void controlChange(byte value, byte control, byte channel) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

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
  
  //MIDI.begin(10);
  Serial.begin(115200);

  //MPR121 setup
  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }
  Serial.println("Adafruit MPR121 Capacitive Touch sensor test"); 
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!mpr.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
}

void loop() {


  computeHX();
  
  if (breath > threshold_bottom && breath < threshold_top) {
    //lettura valori e manipolazione i bit
    velocity = map(breath,8500000,10310000,40,127);
    mpr121 = mpr.touched(); //valore letto da sensore (12 bit: 00000000000)
    currentKey = (mpr121 & mask_key)>>4;
    currentOctave = (mpr121 & mask_octave)>>8; 
//    cc = (mpr121 & mask_cc) >>11;
    currentNote = ((currentOctave+octave)*12)+(noteArray[mpr121 & mask_note]+noteArray[currentKey]);

    //gestione del fiato vera e propria
    if (breathAttack) { //all'inizio della soffiata (va una volta sola)
      breathAttack=false; //cambio lo stato cosi non ci entro piu in questo if
      breathRelease = true; //accendo la possibilià di entrare nell'if di quando interromperò il fiato
      //MIDI.sendNoteOn(currentNote, velocity, 1);  // Send a MIDI note 
      noteOn(currentNote, velocity, 1);  // Send a MIDI note 
      MidiUSB.flush();
      Serial.println(currentNote);
    } else { //durante la soffiata (si ripete continuamente)
      if (currentNote != lastNote) { //se il valore letto da sensore è diverso da quello letto in precedenza
        velocity = map(breath,8500000,10310000,40,127);
        //fai smettere di suonare la nota precedente (perchè siamo in monofonia)
        //MIDI.sendNoteOff(lastNote,velocity,1);
        noteOff(lastNote,velocity,1);
        MidiUSB.flush();
        //aggiorna valore di nota precedente
        lastNote = currentNote;
        //inizia a suonare la nota premuta
        //MIDI.sendNoteOn(currentNote, velocity, 1);  // Send a MIDI note 
        noteOn(currentNote, velocity, 1);  // Send a MIDI note 
        MidiUSB.flush();
        Serial.println(currentKey); //log
      } else {
        //TODO: inviare segnale midi per cambio di velocity esssendo che la nota suonata è la stessa ma puo variare l'intensità
        velocity = map(breath,8500000,10310000,40,127);
        //MIDI.send(midi::ControlChange, 11, velocity, 1);
        controlChange(velocity,11,1);
        MidiUSB.flush();
      }
    }
    
  } else { 
    if (breathRelease==true) { //funziona una volta sola solamente quando rilascio il fiato dopo aver soffiato
      breathAttack=true;
      breathRelease=false;
      //fai smettere di suonare l'ultima nota suonata
     // MIDI.sendNoteOff(lastNote,velocity,1);
      noteOff(lastNote,velocity,1);
      MidiUSB.flush();   
    }  
  }

//  if (cc>0 && cc_debounce == 1) {
//    MIDI.send(midi::ControlChange, 0, bank, 1);
//    cc_debounce == 0;
//  } else {
//    cc_debounce = 1;
//    bank++;
//  }
  
  //delay(500);
    
}
