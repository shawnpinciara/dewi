//MIDI FUNCTIONS:
// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).



//https://www.arduino.cc/reference/en/libraries/midiusb/

#include "MIDIUSB.h"

void noteOn(byte channel,byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
  //MIDI.sendNoteOn(pitch, velocity, 1);  // Send a MIDI note
}

void noteOff(byte channel,byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
  //MIDI.sendNoteOff(pitch,velocity,1);
}

void PolyphonicKeyPressure(byte channel, byte note,byte velocity) {
  midiEventPacket_t event = {0b10100000 | channel, note, velocity};
  MidiUSB.sendMIDI(event);
}
void channelPressure(byte channel, byte note,byte velocity) {
  midiEventPacket_t event = {0b11010000 | channel, note, velocity};
  MidiUSB.sendMIDI(event);
}

void controlChange(byte channel,byte controller_number, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, controller_number, value};
  MidiUSB.sendMIDI(event);
}

void allNotesOff(byte channel) {
  midiEventPacket_t event = {0x7B, 0x00 | channel};
  MidiUSB.sendMIDI(event);
}

void allSoundsOff(byte channel) {
  midiEventPacket_t event = {0x78, 0b00000000 | channel};
  MidiUSB.sendMIDI(event);
}

void monoMode(byte channel) {
  midiEventPacket_t event = {0b01111110, 0b00000001 | channel};
  MidiUSB.sendMIDI(event);
}