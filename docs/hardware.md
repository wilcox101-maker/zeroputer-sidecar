# Hardware constraints

## Why the sidecar is a second MCU

Cardputer ADV is ESP32-S3FN8: 8 MB flash, no PSRAM, 240x135 panel.
A 240x320 RGB565 frame is 153600 bytes. Bruce + Hydra cannot own that buffer.
ES3C28P is N16R8 (16 MB flash + 8 MB OPI PSRAM).

## Occupied ADV pins -- do not reuse

| Function | Pins |
|---|---|
| LCD ST7789V2 | G38 BL, G33 RST, G34 RS, G35 DAT, G36 SCK, G37 CS |
| Audio ES8311 | G8 SDA, G9 SCL, G41 SCLK, G46 ASDOUT, G43 LRCK, G42 DSDIN |
| IMU BMI270 | G8, G9 |
| Keyboard TCA8418 | G8, G9, G11 INT |
| microSD | G12 CS, G14 MOSI, G40 CLK, G39 MISO |
| IR TX | G44 |
| Battery ADC | G10 |
| EXT 14P SPI | G40/G14/G39/G5 -- Hydra cap + SD bus |

## Sidecar link pins

| Port | Pins | Use |
|---|---|---|
| Grove HY2.0-4P | GND, 5V, G2, G1 | UART tether |

## ES3C28P map

| Block | Pins |
|---|---|
| LCD ILI9341V | CS IO10, DC IO46, SCK IO12, MOSI IO11, MISO IO13, BL IO45, RST=CHIP_PU |
| Touch FT6336G | SDA IO16, SCL IO15, RST IO18, INT IO17, addr 0x38 |
| Expand 1.25 mm 4P | IO2, IO3, IO14, IO21 |
| Labeled UART 4P | IO43/IO44 = UART0 -- conflicts with USB-Serial-JTAG |
| Battery ADC | IO9 |
| RGB LED | IO42 |

Sidecar UART1: IO2 = RX, IO3 = TX.
Two batteries. No 5V loop. ESP-NOW needs no electrical link.
