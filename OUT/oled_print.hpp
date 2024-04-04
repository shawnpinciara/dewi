#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET); //Crea l'oggetto display

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void displayNote(byte pitch,byte velocity) {
  display.clearDisplay();
  display.setTextSize(4);  //Imposta la grandezza del testo
  display.setCursor(30,20); //Sposta il cursore di 10 pixel piu in basso per non sovrapporre le stringhe
  display.println(pitch);
  display.setTextSize(1);  //Imposta la grandezza del testo
  display.setCursor(0,0); //Sposta il cursore di 10 pixel piu in basso per non sovrapporre le stringhe
  display.println(velocity);
  display.display();
}

void noteOn(byte channel,byte pitch, byte velocity) {
  displayNote(pitch,velocity);
}

void noteOff(byte channel,byte pitch, byte velocity) {
  displayNote(pitch,velocity);
}

void channelPressure(byte channel, byte note,byte velocity) {
  displayNote(pitch,velocity);
}

void controlChange(byte channel, byte note,byte velocity) {
  displayNote(pitch,velocity);
}

void monoMode(int mode) {

}

void setupControls() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  //Impostare l'indirizzo i2c annotato in precedenza
  display.clearDisplay();  //Pulisce il buffer da inviare al display
  display.setTextColor(WHITE); //Imposta il colore del testo (Solo bianco)
}