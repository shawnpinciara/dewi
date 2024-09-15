//ENCODER
#include <Encoder.h>
Encoder encoder(14, 15);
long encoderPos  = -999;
int encoderVal = 56;


void updateEncoder() {
  long encoderPosNew = encoder.read();
  if (encoderPos != encoderPosNew) {
    if (encoderPos > encoderPosNew) {
      encoderVal++;
      //encoderVal=encoderVal+10;
    } else {
      encoderVal--;
      //encoderVal=encoderVal-10;
    }
    if (encoderVal > 127)
    {
      encoderVal--;
      //encoderVal=encoderVal-10;
    }
    else if (encoderVal < 0){
      encoderVal++;
      //encoderVal=encoderVal+10;
    }
    encoderPos = encoderPosNew;
    //controlChange(0,1,modwheelVal);
  }
  controlChange(0,81,encoderVal);
  Serial.println(encoderVal);
}