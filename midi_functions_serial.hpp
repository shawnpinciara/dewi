//MIDI FUNCTIONS SERIAL:

void noteOn(byte pitch, byte velocity, byte channel) {
  Serial1.write(0b10010000 | channel);
  Serial1.write(pitch);
  Serial1.write(velocity);
}

void noteOff(byte pitch, byte velocity, byte channel) {
  Serial1.write(0x80 | channel);
  Serial1.write(pitch);
  Serial1.write(velocity);
}

void channelPressure(byte note,byte velocity) {
  Serial1.write(0b10100000);
  Serial1.write(note);
  Serial1.write(velocity);
}

void controlChange(byte value, byte control, byte channel) {
  Serial1.write(0x0B | channel);
  Serial1.write(control);
  Serial1.write(value);
}

void allNotesOff(byte channel) {
  Serial1.write(0x7B);
  Serial1.write(0x00 | channel);
}


void allSoundsOff(byte channel) {
  Serial1.write(0x78);
  Serial1.write(0x00 | channel);
}

void monoMode(byte channel) {
  Serial1.write(0x01111110);
  Serial1.write(0x01 | channel);
}