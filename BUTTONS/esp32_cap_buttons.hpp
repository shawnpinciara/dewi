//Setup buttons

const int TOUCH_PINS[4] = {27,14,12,13}; //btn on pin 27 is nearest to the mouth

void setupButtons() {
  for (int i=0; i<4 ;i++) {
    pinMode(TOUCH_PINS[i], GPIO_MODE_INPUT);
  }
  //touch_value_t touch_buttons_threshold = 100000;
}

uint16_t getButtonsState() {
  uint16_t buttons_state;
  //TODO: handle octaves
  for (int i=0; i<4 ;i++) {
    if (touchRead(TOUCH_PINS[i])>=100000) { //button pressed
      buttons_state = buttons_state || 1;
      buttons_state << 1;
    }
  }
  return buttons_state;
}