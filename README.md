# The Diatonical Electric Wind Instrument project
By [Shawn Pinciara](https://www.instagram.com/shawnpinciara) and [Romeo Gabriele](https://linktr.ee/garaggio).

## Idea:
What if instead of having one hands position per note (on all the canonic wind instruments) you could enconde it more efficiently? _That's the reason why we built the DEWI_.

The notes of one scale can be encoded with 3 buttons (2^3 for the white keys) and 1 button more (to add all the sharps); now add another identical 4 buttons row (to keep your other hand entertained) and you can control with one hand the scale, and the other one the key/signature in wich the scale is played... a Diatonical EWI (aka DEWI).

Mapping the 4 buttons to the 12 notes has been a challenge but we found a good balance between beeing logical and organic:

//TODO SHAWN: disegno con i 4 bottoni e le note corrispondenti

## Why Diatonical?
//TODO ROMEO: spiegare perchè la scelta di farla diatonica (seguendo il discorso accennato sopra)

## Schematics & BOM
### Schematics:
//TODO SHAWN: foto schematiche
### BOM:
- ESP32 DEVKIT-V1-38 pin microcontroller
- hx710b breath sensor (based on the MPS20N0040D-D)
- MPR121 touch sensor (you can technically use the 7 embedded esp32 touch pins)

Misc (but kinda essential):
- Paper metal fix (perfect as metal touch buttons, they can be plugged on jumpercables too)

## Installation:
The following libraries should be installed:
//TODO SHAWN: mettere lista di

Then you can upload the dewi.ino onto your board.

## Implementation (if you want to use another type of microcontroller)
### How the code works:
- implementazione codice

### Future implementation:
- Using interrupts
- Adding MIDI DIN 5 pin support
- On board battery with support circuitery
- Adding OSC support
- Responsive lights' show

