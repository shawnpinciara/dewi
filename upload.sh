# Compila
pio run

# Carica su Teensy
pio run -t upload

# Monitor seriale. Assicurati di sostituire il percorso della porta con quello corretto per il tuo sistema.
# Ad esempio, su Windows potrebbe essere COM3, su Linux /dev/ttyUSB0, su macOS /dev/cu.usbmodem145609101
#vedi la lista delle porte con: pio device list
sleep 2 && pio device monitor -p /dev/cu.usbmodem145609101 -b 115200