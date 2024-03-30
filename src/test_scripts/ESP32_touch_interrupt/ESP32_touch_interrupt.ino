#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32.h>

volatile byte nota =  0b00000000;
const byte chiave = 0b00000000;
const byte mask1 = 0b00001000;
const byte mask2 = 0b00000100;
const byte mask3 = 0b00000010;
const byte mask4 = 0b00000001;
const char note[16] = {'C','D','B','E','C','A','F','G','C#','D#','U','U','E','A#','F#','G#'};
const int noteIndex[16] = {1,3,12,5,1,10,6,8,2,4,0,0,5,11,7,9};
// const char scale[] = {'C','C#','D','D#','E','F','F#','G','G#','A','A#','B','C','C#','D','D#','E','F','F#','G','G#','A','A#','B'};
const int scale[24] = {0,1,2,3,4,5,6,7,8,9,10,11,0+12,1+12,2+12,3+12,4+12,5+12,6+12,7+12,8+12,9+12,10+12,11+12};
volatile boolean ottavaSopra = false;
volatile int octave = 4;
volatile int velocity = 127;

BLEMIDI_CREATE_INSTANCE("EWI",MIDI);

void IRAM_ATTR pressSxButton1() {
  nota = nota | mask4; 
  MIDI.sendNoteOn(scale[noteIndex[int(nota)]+noteIndex[int(chiave)]-2]+(12*octave), velocity, 1);  // Send a MIDI note on message
}

void setup() {
  MIDI.begin(10);
  Serial.begin(9600);
  //INTERRUPTS setup
  attachInterrupt(digitalPinToInterrupt(touchRead(4)), pressSxButton1, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:

}
