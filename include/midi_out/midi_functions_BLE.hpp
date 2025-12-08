//MIDI FUNCTIONS:
// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).
d:\Coding\Arduino\dewi\presets\preset_shawn_minidewi.h


//TODO: the order of bytes is wrong, DONT USE ANY OF THIS NOWd:\Coding\Arduino\dewi\presets\preset_dewi_micro_mpr121.h




void noteOn(byte pitch, byte velocity, byte channel) {

  MIDI.sendNoteOn(currentNote, velocity, channel);  // Send a MIDI note
}

void noteOff(byte pitch, byte velocity, byte channel) {

  MIDI.sendNoteOff(lastNote,velocity,channel);
}

void channelPressure(byte value) {
  midiEventPacket_t event = {0x0D, 0b11010001, value};
  MIDI.sendMIDI(event);
}

void controlChange(byte value, byte control, byte channel) {

  MIDI.sendControlChange( value, control, channel);
}

void allNotesOff(byte channel) {
  midiEventPacket_t event = {0x7B, 0x00 | channel};
  MIDI.sendMIDI(event);
}

void allSoundsOff(byte channel) {
  midiEventPacket_t event = {0x78, 0x00 | channel};
  MIDI.sendMIDI(event);
}

void monoMode(byte channel) {
  midiEventPacket_t event = {0x01111110, 0x01 | channel};
  MIDI.sendMIDI(event);
}