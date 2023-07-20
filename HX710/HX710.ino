

<<<<<<< HEAD
const int HX_OUT_PIN = 39;
const int HX_SCK_PIN = 36;
=======
const int HX_OUT_PIN = 36;
const int HX_SCK_PIN = 39;
>>>>>>> 9390d750d6960215a5d51a0a51fa839afbf206b8

enum HX_MODE { NONE, DIFF_10Hz, TEMP_40Hz, DIFF_40Hz};
const byte HX_MODE = DIFF_40Hz;

void setup() {
  pinMode(HX_SCK_PIN, OUTPUT);
  pinMode(HX_OUT_PIN, INPUT);
  Serial.begin(115200);
}

void loop() {
  Serial.println(readHX());
}

unsigned long readHX() {

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
  long result;Serial.println(readHX());
  result += (long)data[2] << 16;
  result += (long)data[1] << 8;
  result += (long)data[0];

  return result;
}
