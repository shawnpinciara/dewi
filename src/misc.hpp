if (cc>0 && cc_debounce == 1) {
  MIDI.send(midi::ControlChange, 0, bank, 1);
  cc_debounce == 0;
} else {
  cc_debounce = 1;
  bank++;
}
    
delay(500);
Serial.print("ComputeHX time: " + String(startTimer - endComputeHX));
Serial.print("Loop time: " + String(millis()-startTimer));
Serial.println();


        /*Serial.print("Breath " + String(breath));
        Serial.print(" , Threshold " + String(threshold_bottom));
        Serial.print(" , Velocity " + String(velocity));
        Serial.print(" , CurrentKey " + String(keyArray[currentKey]));
        Serial.print(" , CurrentOctave " + String(currentOctave));
        Serial.println(" , Breath>threshold: " + String(breath > threshold_bottom));*/


//BUTTONS SCANNING OPTIONS

//MIDI OPTIONS:
//Only one of these 4 can be defined:d:\Coding\Arduino\dewi\presets\preset_dewi_micro_mpr121.h
//#define serial_midi //midi serial or midi usb (if device is midi compliant)
//#define midi_ble //for midi bluetooth
//#define usb_midi
//#define synth //to synthetize audio and not only send midi

// #ifdef midi_ble
//   #include <BLEMIDI_Transport.h>
//   #include <hardware/BLEMIDI_ESP32.h>
//   BLEMIDI_CREATE_INSTANCE("DEWI",MIDI);
// #endif
// #ifdef serial_midi
//   #include "midi_functions_serial.hpp"
// #endif
// #ifdef usb_midi
//   #include "midi_functions_usb.hpp"
// #endif
// #ifdef synth
//   #include "synth.hpp"
// #endif

// #include "breath_sensor.hpp"


//LINUX port:
//sudo chmod 0777 /dev/ttyACM0
