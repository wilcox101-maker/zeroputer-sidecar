# Zeroputer Sidecar

Cardputer ADV (keyboard + radios + IR) driving an LCDWiki **ES3C28P**
2.8" ESP32-S3 touch panel as a display / speaker sidecar.

```
ADV  = controller   (Bruce tools, Hydra cap stays on the 14-pin header)
ES3C28P = renderer  (LVGL, PSRAM framebuffer, mic/speaker, touch keys)
```

Do not hang the panel on the ADV EXT 2.54-14P header. That bus is the
Hydra/CC1101 + nRF24 cap and shares SPI with the onboard microSD.

## Hardware

| Side | Board | SoC | Notes |
|---|---|---|---|
| Controller | M5Stack Cardputer ADV (K132-ADV) | ESP32-S3FN8, 8 MB flash, no PSRAM | Grove `G1/G2`, 1750 mAh |
| Display | LCDWiki ES3C28P | ESP32-S3 N16R8, 16 MB flash, 8 MB OPI PSRAM | 240x320 IPS ILI9341V + FT6336G |

ES3N28P (no touch) works as a render-only sidecar. Touch `KEY` frames need ES3C28P.

## Link order

1. **ESP-NOW** -- default. Hydra cap stays seated. No cable.
2. **Grove UART** -- fallback. ADV `G2->ES3C28P IO2`, ADV `G1<-ES3C28P IO3`.
3. USB-OTG and I2C-on-`G8/G9` are out of scope (header/keyboard collisions).

See [docs/hardware.md](docs/hardware.md) and [docs/pin-card.md](docs/pin-card.md).

## Protocol

Shared header: [`include/sidecar.h`](include/sidecar.h)

```
[AA 55][len:u16le][type:u8][seq:u8][payload][xor]
```

XOR is over `len..payload` inclusive. Max payload 240 bytes per frame.
Tiles larger than that are fragmented with `SC_TILE` + offset.

Full spec: [PROTOCOL.md](PROTOCOL.md)

## Repo layout

```
include/                 shared protocol (C, no ESP-IDF dependency)
common/                  frame pack/unpack used by both firmwares
firmware/adv/            standalone ADV demo (PlatformIO Arduino)
firmware/sidecar-arduino first-boot ES3C28P UI (PlatformIO + TFT_eSPI)
firmware/sidecar/        ESP-IDF + LVGL skeleton
tools/sidecar_frame.py   pack/unpack frames on a PC
docs/                    pin card, power, mechanical, build order
hardware/                cable + BOM
```

## Build

### ADV demo (PlatformIO)

```bash
cd firmware/adv
pio run -t upload
```

Set `SIDECAR_LINK` in `src/sidecar_config.h` to `SC_LINK_ESPNOW` or `SC_LINK_UART`.

### ES3C28P Arduino (fastest first paint)

```bash
cd firmware/sidecar-arduino
pio run -t upload
```

TFT_eSPI pins are already set for the ES3C28P ILI9341V mapping.

### ES3C28P ESP-IDF (LVGL path)

Needs ESP-IDF 5.4.x. LVGL is not vendored.

```bash
cd firmware/sidecar
idf.py set-target esp32s3
idf.py build flash monitor
```

## Flash notes (ES3C28P + Espressif Flash Download Tool 3.9.8)

- Chip: ESP32-S3
- Address `0x0` for a merged image
- **DoNotChgBin checked**
- SPI 80 MHz / DIO is what the vendor quick-start uses; leave the header
  alone on a prebuilt `.img`
- Baud 921600 if the port syncs

Cardputer ADV: side power switch **OFF**, then USB-C. That only disconnects
the battery; USB still powers the Stamp.

## Build order

1. Sidecar: backlight + `HELLO` on screen
2. ESP-NOW ping/pong, MACs printed both sides
3. `TEXT` + `LOG` pages
4. One real ADV hook (IR dump or RF hit list as text)
5. Touch `KEY` back-channel
6. Printed wallet case

Details: [docs/build-order.md](docs/build-order.md)

## License

MIT for this tree. Bruce integration is AGPL-3.0 -- see [NOTICE](NOTICE).
