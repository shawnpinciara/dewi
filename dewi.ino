#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32.h>
#include <Wire.h>
#include "Adafruit_MPR121.h"

//LINUX port:
//sudo chmod 0777 /dev/ttyACM0

//PINS:
//LEFT HAND: near:  15 - 2 - 4 - 32
//RIGHT HAND: near: 12 - 14 - 27 - 33


//binary no midi note map es: binary:0001 -> decimal: 1 -->note: D (1 in midi value)
const int noteArray[16] = {0,2,11,4,12,9,5,7,1,3,0,0,16,10,6,8};
// int noteIndex[] = {1,3,12,5,1,10,6,8,2,4,0,0,5,11,7,9};
char scale[] = {'C','C#','D','D#','E','F','F#','G','G#','A','A#','B','C','C#','D','D#','E','F','F#','G','G#','A','A#','B'};
// boolean ottavaSopra = false;
int octave = 5;
int velocity = 60;
int threshold = 300;
int breath = 0;
const uint16_t mask_key = 0b0000000011110000;
const uint16_t mask_note = 0b0000000000001111;
const uint16_t mask_octave = 0b0000111100000000;
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

Adafruit_MPR121 mpr = Adafruit_MPR121();
BLEMIDI_CREATE_INSTANCE("DEWI",MIDI);


void setup() {
  // put your setup code here, to run once:
  MIDI.begin(10);
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


  breath = 400;
  
  if (breath > threshold) {
    //lettura valori e manipolazione i bit
    mpr121 = mpr.touched(); //valore letto da sensore (12 bit: 00000000000)
    currentKey = (mpr121 & mask_key)>>4;
    currentOctave = (mpr121 & mask_octave)>>8; //TODO: attaccare fisicamente i sensori all'arduino
    // currentNote = mpr121 & mask_note; //maschera per leggere solo le note
    currentNote = ((currentOctave+octave)*12)+(noteArray[mpr121 & mask_note]+noteArray[currentKey]);
    // sendNote = ((currentOctave+octave)*12)+(noteArray[currentNote]+noteArray[currentKey]);
    //gestione del fiato vera e propria
    if (breathAttack) { //all'inizio della soffiata (va una volta sola)
      breathAttack=false; //cambio lo stato cosi non ci entro piu in questo if
      breathRelease = true; //accendo la possibilià di entrare nell'if di quando interromperò il fiato
      MIDI.sendNoteOn(currentNote, velocity, 1);  // Send a MIDI note 
      Serial.println(currentNote);
    } else { //durante la soffiata (si ripete continuamente)
      if (currentNote != lastNote) { //se il valore letto da sensore è diverso da quello letto in precedenza
        //fai smettere di suonare la nota precedente (perchè siamo in monofonia)
        MIDI.sendNoteOff(lastNote,velocity,1);
        //aggiorna valore di nota precedente
        lastNote = currentNote;
        //inizia a suonare la nota premuta
        MIDI.sendNoteOn(currentNote, velocity, 1);  // Send a MIDI note 
        Serial.println(currentKey); //log
      } else {
        //TODO: inviare segnale midi per cambio di velocity esssendo che la nota suonata è la stessa ma puo variare l'intensità
      }
    }
    
  } else { 
    if (breathRelease==true) { //funziona una volta sola solamente quando rilascio il fiato dopo aver soffiato
      breathAttack=true;
      breathRelease=false;
      //fai smettere di suonare l'ultima nota suonata
      MIDI.sendNoteOff(lastNote,velocity,1);   
    }  
  }
  
  //delay(500);
    
}
