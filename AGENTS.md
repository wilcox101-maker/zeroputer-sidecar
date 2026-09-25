# AGENTS.md

Chat instructions override this file.
Smallest correct change. Plausibility is not correctness.

This repo is **Zeroputer Sidecar**: Cardputer ADV (ESP32-S3FN8) as controller
plus LCDWiki ES3C28P (ESP32-S3 N16R8) as display/speaker sidecar.
Firmware first. Dual-S3 link. Hydra/CC1101 stays on the ADV 14-pin header.

---

## Never

- Invent pin maps, UART/SPI framing, baud rates, or HAL return values. Read `docs/pin-card.md`, `docs/hardware.md`, `include/sidecar.h`, or the board header in the firmware you are editing.
- Hang anything off ADV EXT 2.54-14P. That bus is Hydra + microSD SPI.
- Use ES3C28P IO43/IO44 for the sidecar link (UART0 / USB-Serial-JTAG).
- Use ADV G8/G9 for the sidecar (keyboard + ES8311 + BMI270).
- Claim a flash, build, or test passed unless you ran it.
- Commit secrets, Wi-Fi passwords, API keys, `.env`, peer keys, or dumped flash.
- Add Python, Rust, Node, or a third build system because a template mentioned them. `tools/sidecar_frame.py` is the only host helper unless the user asks for another.
- Put Bruce modules, CC1101, nRF24, or IR decode on the sidecar.
- Rewrite one board's firmware in the other board's tree "to unify."
- Block in an ISR. malloc/printf in an ISR. Ignore `esp_err_t`.
- Loop 5V between boards. Grove Red stays open unless the user measured the ADV 5V direction switch.

---

## Stack (this repo)

| Need | Code lives | Tooling |
|---|---|---|
| Keyboard, radios, IR, Bruce hook | `firmware/adv/` | PlatformIO `espressif32`, Arduino |
| First paint on 2.8" | `firmware/sidecar-arduino/` | PlatformIO + TFT_eSPI |
| Optional LVGL path | `firmware/sidecar/` | ESP-IDF 5.4.x -- do not start here |
| Wire contract | `include/sidecar.h`, `common/` | No HAL calls in these files |
| Pin card / power / case | `docs/`, `hardware/` | Markdown only |

Shared types are packed and sized the same on both S3s. Static-assert sizes when you change a struct.
Do not put both boards' main loops in one translation unit.

---

## Pins (do not invent)

ADV Grove UART: TX=`G2`, RX=`G1`, GND common, 5V open.
ES3C28P UART1: RX=`IO2`, TX=`IO3`.
ES3C28P LCD: CS=`IO10` DC=`IO46` SCK=`IO12` MOSI=`IO11` MISO=`IO13` BL=`IO45`.
ES3C28P touch: SDA=`IO16` SCL=`IO15` RST=`IO18` INT=`IO17` addr `0x38`.
ESP-NOW channel default `6`.

Baud start 115200 8N1. Frame: `[AA 55][len:u16le][type][seq][payload][xor]`.
Max payload 240. Drop truncated and bad-XOR frames.

---

## Commands

```text
pipx install platformio

pio run -d firmware/adv
pio run -d firmware/sidecar-arduino

pio run -d firmware/adv -t upload
pio run -d firmware/sidecar-arduino -t upload

pio device monitor -d firmware/adv
python3 tools/sidecar_frame.py pack LOG hello
```

IDF path only if the user asked for LVGL:

```text
cd firmware/sidecar
idf.py set-target esp32s3
idf.py build
```

Flash Download Tool 3.9.8: **DoNotChgBin checked**. Do not retune SPI speed/mode on a vendor `.img`. ADV: side power switch OFF, then USB-C.

Host sanitizers do not apply on-device. Done means: the env you edited **builds**, and you state whether it was flashed.

---

## Error-proofing

- Check every ESP-IDF / Arduino call that returns a status.
- Frames: length, type, seq, XOR, max size. Drop truncated frames. Never treat short reads as zeros.
- Timeouts on every bus wait. Watchdog-friendly loops.
- Bounded buffers only. `snprintf`, never `sprintf`.
- Init order: clocks -> link (ESP-NOW or UART1) -> display -> app.
- Packed wire structs stay packed. Static assert sizes after edits.

---

## Code style

- C11 / C++17. Match the file you edit.
- Braces on every `if`/`for`/`while`.
- Pin names from a single header per board. No raw GPIO numbers in app code.
- Comments only for why, invariants, and wire layout.
- No new dependency or PlatformIO lib without asking.

---

## Workflow

1. Name which board the change belongs to. If both, change `include/sidecar.h` plus both sides only when the wire contract changes. Bump `SC_PROTO_VER`.
2. Read the pin card and frame struct before editing.
3. Patch the smallest path.
4. `pio run` for every env you touched.
5. Report: files, env built, flashed or not, residual risk.

## Stop

- Pinout or connector is unspecified and would change hardware.
- Protocol change would break the other board and you only have one tree open.
- Board not present and the task requires a flash or bus capture.
- Task needs credentials or radio behavior you cannot legally or physically exercise.

## Done

- Behavior exists on the board the need required.
- Invalid frames and HAL failures are explicit.
- Touched env built because you ran `pio run`.
- Diff does not add a second language or build system.
