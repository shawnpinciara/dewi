//BREATH
#include "../BREATH/always_on.hpp"

//BUTTONS
#include "../BUTTONS/esp32_cap_buttons.hpp"

//OUT
#include "../OUT/test/serial_print.hpp"

#define mozzi


#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);


// use #define for CONTROL_RATE, not a constant
void setupMisc() {
  startMozzi(CONTROL_RATE); // :)
  aSin.setFreq(440); // set the frequency
}
void updateControls(){};

void updateControl(){
  // put changing controls in here
}

AudioOutput_t updateAudio(){
  return MonoOutput::from8Bit(aSin.next()); // return an int signal centred around 0
}