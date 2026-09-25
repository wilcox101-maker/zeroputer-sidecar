# Sidecar wire protocol v1

Little-endian. Same bytes on UART and ESP-NOW.

## Frame

| Offset | Size | Field |
|---|---|---|
| 0 | 2 | Magic `0xAA 0x55` |
| 2 | 2 | `len` = size of `type + seq + payload` |
| 4 | 1 | `type` |
| 5 | 1 | `seq` (increment per direction, wraps) |
| 6 | `len-2` | payload |
| 6+len-2 | 1 | XOR of bytes from `len` through last payload byte |

`len` minimum 2 (type+seq, empty payload).
`len` maximum `SC_MAX_LEN` (242).
Max payload = 240.

Invalid magic, length, or XOR: drop the frame. Do not NAK unless you are in debug mode (`SC_ACK` with status `SC_ST_BAD`).

## Types

| id | name | dir | payload |
|---|---|---|---|
| 0x01 | `SC_HELLO` | both | `sc_hello_t` |
| 0x02 | `SC_PAGE` | ADV->side | `u8 page_id` |
| 0x03 | `SC_TEXT` | ADV->side | `u8 widget_id` + UTF-8 (no NUL required) |
| 0x04 | `SC_TILE` | ADV->side | `sc_tile_t` + pixels |
| 0x05 | `SC_KEY` | side->ADV | `sc_key_t` |
| 0x06 | `SC_AUDIO` | either | `sc_audio_t` + optional PCM |
| 0x07 | `SC_BAT` | both | `u16 mv` |
| 0x08 | `SC_LOG` | ADV->side | UTF-8 line, no NUL |
| 0x09 | `SC_ACK` | both | `u8 status` + `u8 of_seq` |
| 0x0A | `SC_PING` | both | `u32 uptime_ms` |
| 0x0B | `SC_PONG` | both | copy of ping payload |

## Page IDs

| id | name | purpose |
|---|---|---|
| 0 | `SC_PAGE_BOOT` | splash / unpaired |
| 1 | `SC_PAGE_LOG` | scrolling lines |
| 2 | `SC_PAGE_RF` | hit list from ADV |
| 3 | `SC_PAGE_IR` | named IR slots |
| 4 | `SC_PAGE_HEX` | capture inspector |
| 5 | `SC_PAGE_VOICE` | transcript |
| 6 | `SC_PAGE_MAP` | reserved |
| 7 | `SC_PAGE_STATUS` | batteries, RSSI, link |

## Widget IDs (`SC_TEXT`)

| id | typical use |
|---|---|
| 0 | title bar |
| 1 | subtitle / freq |
| 2 | body line 0 (log append if page is LOG) |
| 3-10 | body lines |
| 255 | clear all widgets on current page |

## Status codes (`SC_ACK`)

| id | meaning |
|---|---|
| 0 | ok |
| 1 | bad frame |
| 2 | unknown type |
| 3 | busy |
| 4 | unsupported page |

## ESP-NOW

- ADV is primary. Sidecar is peer-only (no encryption in v1).
- Lock channel in `sidecar_config.h` (`SC_ESPNOW_CHANNEL`, default 6).
- Payload is the raw frame including magic and XOR.
- Tiles larger than 240 bytes use multiple `SC_TILE` with increasing `offset`.

## UART

- 8N1, no flow control
- Default 115200, then 460800 after HELLO/ACK
- COBS is not used in v1. Frames are magic-delimited.
- Resync: scan for `AA 55`.

## Versioning

`sc_hello_t.proto` is `1` for this document. Bump it when types or payload layouts change. Older peers must ignore unknown types.
