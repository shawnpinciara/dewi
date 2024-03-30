#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32.h>

BLEMIDI_CREATE_DEFAULT_INSTANCE()
// BLEMIDI_CREATE_INSTANCE("AWESOME_INSTRUMENT", MIDI)


const int btnPin = 13;
const int potPin = 15;
const int potPin2 = 2;

int midiNote;
int btnState;
int lastBtnState;
int velocity;

void setup() {


  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); 
  
  BLEMIDI.setHandleConnected(onConnected);
  BLEMIDI.setHandleDisconnected(onDisconnected);

  MIDI.begin();

}


void loop() {
  

}


void onConnected() {
  digitalWrite(LED_BUILTIN, HIGH);
}
void onDisconnected() {
  digitalWrite(LED_BUILTIN, LOW);
}
