
//SETUP MOZZI
#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include "mozzi_midi.h"
// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

// use #define for CONTROL_RATE, not a constant
void setupMisc() {
  startMozzi(CONTROL_RATE); // :)
  aSin.setFreq(440); // set the frequency
}


AudioOutput_t updateAudio(){
  return MonoOutput::from8Bit(aSin.next()); // return an int signal centred around 0
}


//SEND AUDIO

void noteOn(byte channel,byte pitch, byte velocity) {
  //TODO: send audio out
  aSin.setFreq(mtof(pitch)); // set the frequency
  Serial.println(String(pitch));
}


void noteOff(byte channel,byte pitch, byte velocity) {
  aSin.setFreq(30); // set the frequency
  Serial.println(String(pitch));
}

void channelPressure(byte channel, byte pitch,byte velocity) {
  Serial.print("CP: ");
  Serial.print("Channel " + String(channel));
  Serial.print(" , Pitch " + String(pitch));
  Serial.print(" , Velocity " + String(velocity));
  Serial.println();
}

void controlChange(byte channel, byte pitch,byte velocity) {
  Serial.print("CC: ");
  Serial.print("Channel " + String(channel));
  Serial.print(" , Pitch " + String(pitch));
  Serial.print(" , Velocity " + String(velocity));
  Serial.println();
}

void monoMode(int mode) {

}