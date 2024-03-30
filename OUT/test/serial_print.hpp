
void noteOn(byte channel,byte pitch, byte velocity) {
  Serial.println(pitch);
}

void noteOff(byte channel,byte pitch, byte velocity) {
  Serial.println(pitch);
}

void channelPressure(byte channel, byte note,byte velocity) {
  Serial.println(note);
}

void controlChange(byte channel, byte note,byte velocity) {
  Serial.println(note);
}

void monoMode(int mode) {

}