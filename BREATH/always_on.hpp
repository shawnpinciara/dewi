//Like the breath is always on
const int HX_OUT_PIN = 4;
const int HX_SCK_PIN = 5;
enum HX_MODE { NONE, DIFF_10Hz, TEMP_40Hz, DIFF_40Hz};
const byte HX_MODE = DIFF_40Hz;
//BREATH READ FUNCTION
long getBreath(int HX_SCK_PIN, int HX_OUT_PIN, byte HX_MODE) {
  long result = 2147483647;
  return result;
}

void setupHx(int HX_SCK_PIN, int HX_OUT_PIN) {
  pinMode(HX_SCK_PIN, OUTPUT);
  pinMode(HX_OUT_PIN, INPUT);
}