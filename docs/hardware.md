# Hardware constraints

ADV EXT 2.54-14P is Hydra/CC1101 + nRF24 and shares SPI with microSD. Sidecar uses ESP-NOW or Grove UART only.

ADV Grove: GND, 5V, G2, G1.
ES3C28P expand UART1: IO2 RX, IO3 TX.
ES3C28P LCD: CS10 DC46 SCK12 MOSI11 MISO13 BL45.
ES3C28P touch: SDA16 SCL15 RST18 INT17 addr 0x38.
Two batteries. No 5V loop.
