#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32.h>
BLEMIDI_CREATE_INSTANCE("EWII",MIDI);
  
//sudo chmod 0777 /dev/ttyACM0
const int numButtons = 4;  // The number of touch buttons you have
const int buttonPins[numButtons] = { 4, 0, 2, 15};  // The pins of the touch buttons
const int midiNotes[numButtons] = { 60, 62, 64, 65 };  // The MIDI notes to send when a button is pressed
bool buttonStates[numButtons] = { false ,false ,false ,false };  // Initialize all buttons to be off

int tempo = 500;
void setup() {
  MIDI.begin(10);
}

void loop() {
  for (int i = 0; i < numButtons; i++) {
    int touchValue = touchRead(buttonPins[i]);  // Read the touch value of the current button 
    if (touchValue < 30) {  // If the button is pressed
      if (!buttonStates[i]) {  // If the button state has changed to on
        buttonStates[i] = true;  // Update the button state
        MIDI.sendNoteOn(midiNotes[i], 127, 1);  // Send a MIDI note on message
      }
    }
    else {  // If the button is released
      if (buttonStates[i]) {  // If the button state has changed to off
        buttonStates[i] = false;  // Update the button state
        MIDI.sendNoteOff(midiNotes[i], 127, 1);  // Send a MIDI note off message
      }
    }
  }
}