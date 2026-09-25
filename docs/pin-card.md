# Grove UART pin card

Print this and tape it to the cable.

```
Cardputer ADV Grove              ES3C28P expand 4P
HY2.0-4P                         1.25 mm

Black  GND  ----------------  GND
Red    5V   -- do not connect (each board uses own battery / USB)
Yellow G2   ----------------  IO2   ADV TX -> sidecar RX
White  G1   ----------------  IO3   ADV RX <- sidecar TX
```

UART 115200 8N1. ESP-NOW channel 6.
Do not wire ADV G3, ADV G8/G9, ES3C28P IO43/IO44, ES3C28P IO15/IO16.
