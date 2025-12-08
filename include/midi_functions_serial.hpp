//MIDI FUNCTIONS SERIAL:
//SEE HERE: http://www.music.mcgill.ca/~ich/classes/mumt306/StandardMIDIfileformat.html Appendix 1.1 - Table of Major MIDI Messages
void noteOn(byte channel,byte pitch, byte velocity) {
  Serial1.write(0b10010000 | channel);
  Serial1.write(pitch);
  Serial1.write(velocity);
}

void noteOff(byte channel,byte pitch, byte velocity) {
  Serial1.write(0b10000000 | channel);
  Serial1.write(pitch);
  Serial1.write(velocity);
}

void PolyphonicKeyPressure(byte channel, byte note,byte velocity) {
  Serial1.write(0b10100000 | channel);
  Serial1.write(note);
  Serial1.write(velocity);
}

void channelPressure(byte channel, byte note,byte velocity) {
  Serial1.write(0b11010000 | channel);
  Serial1.write(note);
  Serial1.write(velocity);
}

void controlChange(byte channel,byte controller_number, byte value) {
  Serial1.write(0b10110000 | channel);
  Serial1.write(controller_number);
  Serial1.write(value);
}

// void pitchWheel(byte channel, uint16_t value) { //TODO: might not work
//   //PitchWheel value is 2 byte value with this form: 0xxxxxxx 0yyyyyyy
//   //The middle position is: 00100000 00000000 (which is int 8192)
//   //So the max number should be 01111111 01111111 (which is int 32639)
//   Serial1.write(0b11100000 | channel);
//   Serial1.write(value && 0b0111111101111111);
// }

/*
 * @brief Invia un messaggio di pitch bend MIDI su un canale specificato.
 *
 * Questa funzione invia un messaggio di pitch bend (controllo della ruota del pitch)
 * tramite Serial1, come definito dalla specifica MIDI. Il valore del pitch bend
 * è un intero a 14 bit, che va da 0 a 16383. Il valore centrale (nessun pitch bend)
 * è 8192.
 *
 * @param channel Il canale MIDI (da 0 a 15) su cui inviare il messaggio.
 * @param value Il valore del pitch bend (da 0 a 16383).
 */
// void pitchWheel(byte channel, uint16_t value) {
//   // Il canale MIDI deve essere compreso tra 0 e 15.
//   // La maschera 0b00001111 assicura che solo i bit del canale siano utilizzati.
//   // E0-EF è il range per il messaggio di Pitch Bend. E0 è il canale 0.
//   // I quattro bit meno significativi rappresentano il canale (0-15).
//   byte statusByte = 0b11100000 | (channel & 0b00001111);

//   // Il pitch bend MIDI è un valore a 14 bit, diviso in due byte a 7 bit.
//   // Il byte meno significativo (LSB) contiene i 7 bit inferiori del valore.
//   // La maschera 0b01111111 estrae i primi 7 bit.
//   byte lsb = value & 0b01111111;

//   // Il byte più significativo (MSB) contiene i 7 bit superiori del valore.
//   // Si sposta il valore di 7 bit a destra e poi si maschera per prendere solo i 7 bit.
//   // La maschera 0b01111111 estrae i 7 bit successivi.
//   byte msb = (value >> 0b00000111) & 0b01111111;

//   // Invia i tre byte del messaggio di pitch bend.
//   // Serial1 è tipicamente usato su schede come l'Arduino Mega per la comunicazione MIDI hardware.
//   Serial1.write(statusByte); // Status Byte (0b1110CCCC, dove CCCC è il canale)
//   Serial1.write(lsb);        // Least Significant Byte (0b0LLLLLLL)
//   Serial1.write(msb);        // Most Significant Byte (0b0MMMMMMM)
// }


// /*
//  * @brief Invia un messaggio di pitch bend MIDI su un canale specificato
//  * per raggiungere un'intonazione corrispondente a una nota MIDI target.
//  *
//  * Questa funzione calcola il valore di pitch bend a 14 bit necessario per raggiungere
//  * la 'midiNote' target partendo dalla 'referencePitch' originale, tenendo conto
//  * del 'bendSize' (range massimo del pitch bend sul synth) in cent.
//  *
//  * @param channel Il canale MIDI (da 0 a 15) su cui inviare il messaggio.
//  * @param midiNote La nota MIDI target a cui si desidera arrivare tramite il bend (es. 60 per Do centrale).
//  * @param bendSize La dimensione massima del range di pitch bend configurato sul sintetizzatore,
//  * in cent (es. 100 per +/- 1 semitono, 200 per +/- 2 semitoni, 1200 per +/- 1 ottava).
//  * Questo è il valore massimo di deviazione in una singola direzione.
//  * @param referencePitch La nota MIDI di riferimento (la nota su cui è stato inviato l'ultimo "Note On").
//  */
// void pitchWheel1(byte channel, byte midiNote, uint16_t bendSize, byte referencePitch) {
//   // Gestione di un bendSize pari a zero per evitare divisione per zero.
//   // In questo caso, si invia il valore centrale (nessun pitch bend) poiché non è possibile piegare.
//   if (bendSize == 0) {
//     pitchWheelRaw(channel, 8192); // Invia il valore centrale (nessun bend)
//     return;
//   }

//   // Calcola la differenza in cent tra la nota target e la nota di riferimento.
//   // 1 semitono = 100 cent.
//   float centsDifference = (float)(midiNote - referencePitch) * 100.0f;

//   // Normalizza la differenza in cent rispetto al range massimo del pitch bend.
//   // Il valore risultante sarà tra -1.0 (max bend giù) e +1.0 (max bend su).
//   float normalizedDeviation = centsDifference / (float)bendSize;

//   // Clampa il valore normalizzato tra -1.0 e 1.0. Questo assicura che il bend
//   // non superi mai il range fisico del pitch bend del sintetizzatore, anche se
//   // la midiNote target è molto lontana dalla referencePitch.
//   normalizedDeviation = constrain(normalizedDeviation, -1.0f, 1.0f);

//   // Mappa il valore normalizzato (range da -1.0 a 1.0) al range del pitch bend MIDI (da 0 a 16383).
//   // La formula (normalizedDeviation + 1.0f) / 2.0f trasforma il range [-1, 1] a [0, 1].
//   // Poi si scala questo valore al range [0, 16383].
//   uint16_t pitchBendValue = (uint16_t)(((normalizedDeviation + 1.0f) / 2.0f) * 16383.0f);

//   // Chiama la funzione helper per inviare il valore di pitch bend calcolato.
//   // pitchWheelRaw(channel, pitchBendValue);
// }

/*
 * @brief Funzione helper interna per inviare direttamente un valore di pitch bend MIDI a 14 bit.
 *
 * Non destinata ad essere chiamata direttamente dall'utente, ma utilizzata da pitchWheel().
 *
 * @param channel Il canale MIDI (da 0 a 15) su cui inviare il messaggio.
 * @param value Il valore del pitch bend raw (da 0 a 16383).
 */
// void pitchWheelRaw(byte channel, uint16_t value) {
//   // Lo Status Byte per il Pitch Bend è 0b1110xxxx, dove xxxx è il canale MIDI (0-15).
//   // La maschera 0b00001111 isola i bit del canale.
//   byte statusByte = 0b11100000 | (channel & 0b00001111);
//   // Il valore del Pitch Bend è a 14 bit e viene diviso in due byte a 7 bit.
//   // LSB (Least Significant Byte) contiene i 7 bit inferiori.
//   byte lsb = value & 0b01111111;
//   // MSB (Most Significant Byte) contiene i 7 bit superiori.
//   // Si sposta il valore di 7 bit a destra e poi si maschera per prendere i 7 bit.
//   byte msb = (value >> 0b00000111) & 0b01111111;
//   // Invia i tre byte MIDI tramite Serial1.
//   Serial1.write(statusByte); // Status Byte
//   Serial1.write(lsb);        // Byte meno significativo
//   Serial1.write(msb);        // Byte più significativo
// }

void allNotesOff(byte channel) {
  Serial1.write(123);
  Serial1.write(0x00 | channel);
}


void allSoundsOff(byte channel) {
  Serial1.write(0b01111000);
  Serial1.write(0x00 | channel);
}

void monoMode(byte channel) {
  // Serial1.write(0x01111110);
  Serial1.write(126);
  Serial1.write(0x01 | channel);
}