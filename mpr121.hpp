Adafruit_MPR121 mpr = Adafruit_MPR121();

uint16_t getButtonsState() {
  return mpr.touched(); //valore letto da sensore (12 bit: 00000000000)
}