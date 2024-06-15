//Sensor: HX710B
//BREATH SENSOR
//https://swharden.com/blog/2022-11-14-hx710b-arduino/

const int HX_OUT_PIN = 4;
const int HX_SCK_PIN = 5;

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
  long result = 0;
  result += (long)data[2] << 16;
  result += (long)data[1] << 8;
  result += (long)data[0];
  return result;
}

long getBreath2() {
  while (digitalRead(HX_OUT_PIN)) {}

  // read 24 bits
  long result = 0;
  for (int i = 0; i < 24; i++) {
    digitalWrite(HX_SCK_PIN, HIGH);
    digitalWrite(HX_SCK_PIN, LOW);
    result = result << 1;
    if (digitalRead(HX_OUT_PIN)) {
      result++;
    }
  }

  // get the 2s compliment
  result = result ^ 0x800000;

  // pulse the clock line 3 times to start the next pressure reading
  for (char i = 0; i < 3; i++) {
    digitalWrite(HX_SCK_PIN, HIGH);
    digitalWrite(HX_SCK_PIN, LOW);
  }

  return result;
}


void setupBreath() {
  pinMode(HX_SCK_PIN, OUTPUT);
  pinMode(HX_OUT_PIN, INPUT);
}