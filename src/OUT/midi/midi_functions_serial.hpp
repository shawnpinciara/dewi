//MIDI FUNCTIONS SERIAL:
//SEE HERE: http://www.music.mcgill.ca/~ich/classes/mumt306/StandardMIDIfileformat.html Appendix 1.1 - Table of Major MIDI Messages
void noteOn(byte channel,byte pitch, byte velocity) {
  Serial1.write(0b10010000 | channel);
  Serial1.write(pitch);
  Serial1.write(velocity);
}

void noteOff(byte channel,byte pitch, byte velocity) {
  Serial1.write(0b10000000 | channel);
  Serial1.write(pitch);
  Serial1.write(velocity);
}

void channelPressure(byte channel, byte note,byte velocity) {
  Serial1.write(0b10100000 | channel);
  Serial1.write(note);
  Serial1.write(velocity);
}

void controlChange(byte channel,byte controller_number, byte value) {
  Serial1.write(0b10110000 | channel);
  Serial1.write(controller_number);
  Serial1.write(value);
}

void pitchWheel(byte channel, uint16_t value) { //TODO: might not work
  //PitchWheel value is 2 byte value with this form: 0xxxxxxx 0yyyyyyy
  //The middle position is: 00100000 00000000 (which is int 8192)
  //So the max number should be 01111111 01111111 (which is int 32639)
  Serial1.write(0b11100000 | channel);
  Serial1.write(value && 0b0111111101111111);
}

void allNotesOff(byte channel) {
  Serial1.write(0b01111011);
  Serial1.write(0x00 | channel);
}


void allSoundsOff(byte channel) {
  Serial1.write(0b01111000);
  Serial1.write(0x00 | channel);
}

void monoMode(byte channel) {
  Serial1.write(0x01111110);
  Serial1.write(0x01 | channel);
}