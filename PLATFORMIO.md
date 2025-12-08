# DEWI - PlatformIO Project

Questo progetto è stato convertito per PlatformIO e compila per Teensy 4.0.

## Struttura del Progetto

```
dewi/
├── platformio.ini          # Configurazione PlatformIO
├── src/
│   └── main.cpp           # File principale (ex dewi.ino)
├── include/
│   ├── always_on.hpp
│   ├── serial_print.hpp
│   ├── midi_functions_serial.hpp
│   └── midi_out/
│       ├── midi_functions_BLE.hpp
│       └── midi_functions_usb.hpp
└── tests/                  # Test hardware
```

## Compilazione

```bash
# Compila il progetto
pio run

# Compila e carica su Teensy 4.0
pio run -t upload

# Monitor seriale
pio device monitor
```

## Configurazione Hardware

- **Board**: Teensy 4.0
- **Framework**: Arduino
- **USB Type**: MIDI (configurato automaticamente)

## Librerie Utilizzate

- Adafruit Unified Sensor
- Adafruit BNO055 (IMU sensor)
- Adafruit MPR121 (capacitive touch)
- Adafruit NeoPixel
- MIDI Library (Serial MIDI)

## Note per Teensy

Teensy 4.0 ha supporto USB MIDI nativo tramite `usbMIDI`, non è necessaria la libreria MIDIUSB.
Le chiamate a `usbMIDI.sendNoteOn()`, `usbMIDI.sendNoteOff()`, etc. funzionano direttamente.
