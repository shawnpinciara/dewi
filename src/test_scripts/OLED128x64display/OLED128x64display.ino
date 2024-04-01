
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET); //Crea l'oggetto display

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


void setup()   {    
          
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  //Impostare l'indirizzo i2c annotato in precedenza
  display.clearDisplay();  //Pulisce il buffer da inviare al display
  display.setTextSize(1);  //Imposta la grandezza del testo
  display.setTextColor(WHITE); //Imposta il colore del testo (Solo bianco)

  //Riga 1
  display.setCursor(0,0); //Imposta la posizione del cursore (Larghezza,Altezza)
  display.println("LORENZOCASABURO.IT"); //Stringa da visualizzare
 
  //Riga 2
  display.setCursor(0,10); //Sposta il cursore di 10 pixel piu in basso per non sovrapporre le stringhe
  display.println("GUIDA DISPLAY OLED");
 
  display.display(); //Invia il buffer da visualizzare al display
  display.setCursor(0,20); //Sposta il cursore di 10 pixel piu in basso per non sovrapporre le stringhe
  display.println("test.....");
 
  display.display(); //Invia il buffer da visualizzare al display



}


void loop() {
 
}