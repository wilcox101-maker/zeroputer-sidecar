# Zeroputer Sidecar

Agent contract: [`AGENTS.md`](AGENTS.md) (pointer: [`Code_Agents.md`](Code_Agents.md)).

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

## Link order

1. **ESP-NOW** -- default. Hydra cap stays seated. No cable.
2. **Grove UART** -- fallback. ADV `G2->ES3C28P IO2`, ADV `G1<-ES3C28P IO3`.

## Protocol

Shared header: [`include/sidecar.h`](include/sidecar.h)

```
[AA 55][len:u16le][type:u8][seq:u8][payload][xor]
```

Full spec: [PROTOCOL.md](PROTOCOL.md)

## Build

```bash
pio run -d firmware/adv
pio run -d firmware/sidecar-arduino
```

Set `SIDECAR_LINK` in `firmware/adv/src/sidecar_config.h`.
Flash Download Tool 3.9.8: DoNotChgBin checked. ADV side switch OFF, then USB-C.

## License

MIT for this tree. Bruce integration is AGPL-3.0 -- see [NOTICE](NOTICE).
