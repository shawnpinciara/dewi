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
