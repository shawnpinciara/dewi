void noteOn(byte channel,byte pitch, byte velocity) {
  Serial.print("NOn: ");
  Serial.print("Channel " + String(channel));
  Serial.print(" , Pitch " + String(pitch));
  Serial.print(" , Velocity " + String(velocity));
  Serial.println();
}

void noteOff(byte channel,byte pitch, byte velocity) {
  Serial.print("NOff: ");
  Serial.print("Channel " + String(channel));
  Serial.print(" , Pitch " + String(pitch));
  Serial.print(" , Velocity " + String(velocity));
  Serial.println();
}

void PolyphonicKeyPressure(byte channel, byte pitch,byte velocity) {
  Serial.print("PolyphonicKeyPressure: ");
  Serial.print("Channel " + String(channel));
  Serial.print(" , Pitch " + String(pitch));
  Serial.print(" , Velocity " + String(velocity));
  Serial.println();
}

void channelPressure(byte channel, byte pitch,byte velocity) {
  Serial.print("channelPressure: ");
  Serial.print("Channel " + String(channel));
  Serial.print(" , Pitch " + String(pitch));
  Serial.print(" , Velocity " + String(velocity));
  Serial.println();
}

void controlChange(byte channel, byte pitch,byte velocity) {
  // Serial.print("CC: ");
  // Serial.print("Channel " + String(channel));
  // Serial.print(" , Pitch " + String(pitch));
  // Serial.print(" , Velocity " + String(velocity));
  // Serial.println();
}

void monoMode(int mode) {

}


