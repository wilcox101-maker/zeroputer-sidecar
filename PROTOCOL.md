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

Invalid magic, length, or XOR: drop the frame.

## Types

| id | name | dir | payload |
|---|---|---|---|
| 0x01 | SC_HELLO | both | sc_hello_t |
| 0x02 | SC_PAGE | ADV->side | u8 page_id |
| 0x03 | SC_TEXT | ADV->side | u8 widget_id + UTF-8 |
| 0x04 | SC_TILE | ADV->side | sc_tile_t + pixels |
| 0x05 | SC_KEY | side->ADV | sc_key_t |
| 0x06 | SC_AUDIO | either | sc_audio_t |
| 0x07 | SC_BAT | both | u16 mv |
| 0x08 | SC_LOG | ADV->side | UTF-8 line |
| 0x09 | SC_ACK | both | u8 status + u8 of_seq |
| 0x0A | SC_PING | both | u32 uptime_ms |
| 0x0B | SC_PONG | both | copy of ping payload |

Pages: BOOT=0 LOG=1 RF=2 IR=3 HEX=4 VOICE=5 MAP=6 STATUS=7
