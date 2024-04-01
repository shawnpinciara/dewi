//Sensor: HX710B
//BREATH SENSOR

//On arduino micro:
const int HX_OUT_PIN = 4;
const int HX_SCK_PIN = 5;

//on ESP32
//const int HX_OUT_PIN = 2;
//const int HX_SCK_PIN = 4;
enum HX_MODE { NONE, DIFF_10Hz, TEMP_40Hz, DIFF_40Hz};
const byte HX_MODE = DIFF_40Hz;

//BREATH READ FUNCTION
long getBreath() {
  // pulse clock line to start a reading
  for (char i = 0; i < HX_MODE; i++) {
    digitalWrite(HX_SCK_PIN, HIGH);
    digitalWrite(HX_SCK_PIN, LOW);
  }
  // wait for the reading to finish
  while (digitalRead(HX_OUT_PIN)) {}

  // read the 24-bit pressure as 3 bytes using SPI
  byte data[3];
  for (byte j = 3; j--;) {
    data[j] = shiftIn(HX_OUT_PIN, HX_SCK_PIN, MSBFIRST);
  }
  data[2] ^= 0x80;  // see note
  // shift the 3 bytes into a large integer
  long result;
  result += (long)data[2] << 16;
  result += (long)data[1] << 8;
  result += (long)data[0];
  Serial.println(result);
  return result;
}

void setupHx(int HX_SCK_PIN, int HX_OUT_PIN) {
  pinMode(HX_SCK_PIN, OUTPUT);
  pinMode(HX_OUT_PIN, INPUT);
}

void setupBreath() {
  setupHx(HX_SCK_PIN,HX_OUT_PIN);
}