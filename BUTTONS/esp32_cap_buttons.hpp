//Setup buttons

//Options
const int TOUCH_PINS[4] = {T7,T6,T5,T4}; //btn on pin 27 is nearest to the mouth


void setupButtons() {
}

uint16_t getButtonsState() {
  int buttons_state = 0;
  //TODO: handle octaves
  for (int i=0; i<4 ;i++) {
    if (touchRead(TOUCH_PINS[i])<=20) { //button pressed
      //Serial.print(" BTN" + String(i) + ": TOUCHED");
      //buttons_state = buttons_state || 0b0000000000000001;
      switch(i) {
        case 0:
          buttons_state+=1;
          break; 
        case 1:
          buttons_state+=2;
          break; 
        case 2:
          buttons_state+=4;
          break; 
        case 3:
          buttons_state+=8;
          break; 
      }
    }
    //buttons_state << 1;
  }
  return buttons_state;
}