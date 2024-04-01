#define mozzi

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include "../oled_128x64_print.hpp"
//BREATH
//#include "../../BREATH/always_on.hpp"

//BUTTONS
//#include "../../BUTTONS/esp32_cap_buttons.hpp"
double base_frequency = 440.0;
double base_pitch = 69.0;

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

AudioOutput_t updateAudio(){
  return MonoOutput::from8Bit(aSin.next()); // return an int signal centred around 0
}

float midiToFreq(float incoming_note) { //https://github.com/little-scale/mtof/blob/master/mtof.cpp
  return base_frequency * pow (2.0, (incoming_note - base_pitch) / 12.0);
}
void setupMisc() {
  startMozzi(CONTROL_RATE); // :)
  aSin.setFreq(440); // set the frequency
}

void noteOn(byte channel,byte pitch, byte velocity) {
  aSin.setFreq(midiToFreq(pitch));
}

void noteOff(byte channel,byte pitch, byte velocity) {

}

void channelPressure(byte channel, byte note,byte velocity) {

}

void controlChange(byte channel, byte note,byte velocity) {

}

void monoMode(int mode) {

}