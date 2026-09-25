#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SC_MAGIC0           0xAAu
#define SC_MAGIC1           0x55u
#define SC_PROTO_VER        1u
#define SC_MAX_PAYLOAD      240u
#define SC_MAX_LEN          (2u + SC_MAX_PAYLOAD)
#define SC_HDR_SIZE         4u
#define SC_OVERHEAD         (SC_HDR_SIZE + 1u)
#define SC_MAX_FRAME        (SC_OVERHEAD + SC_MAX_LEN)

#define SC_HELLO            0x01u
#define SC_PAGE             0x02u
#define SC_TEXT             0x03u
#define SC_TILE             0x04u
#define SC_KEY              0x05u
#define SC_AUDIO            0x06u
#define SC_BAT              0x07u
#define SC_LOG              0x08u
#define SC_ACK              0x09u
#define SC_PING             0x0Au
#define SC_PONG             0x0Bu

#define SC_PAGE_BOOT        0u
#define SC_PAGE_LOG         1u
#define SC_PAGE_RF          2u
#define SC_PAGE_IR          3u
#define SC_PAGE_HEX         4u
#define SC_PAGE_VOICE       5u
#define SC_PAGE_MAP         6u
#define SC_PAGE_STATUS      7u

#define SC_WID_TITLE        0u
#define SC_WID_SUB          1u
#define SC_WID_BODY0        2u
#define SC_WID_CLEAR        255u

#define SC_ST_OK            0u
#define SC_ST_BAD           1u
#define SC_ST_UNKNOWN       2u
#define SC_ST_BUSY          3u
#define SC_ST_UNSUP         4u

#define SC_KEY_TOUCH        0u
#define SC_KEY_VIRTUAL      1u
#define SC_KEY_HOLD         2u
#define SC_KEY_RELEASE      3u

#define SC_AUD_PLAY_FILE    0u
#define SC_AUD_STOP         1u
#define SC_AUD_PCM8         2u
#define SC_AUD_SAY          3u

#define SC_ROLE_ADV         1u
#define SC_ROLE_SIDECAR     2u

#define SC_LINK_NONE        0u
#define SC_LINK_ESPNOW      1u
#define SC_LINK_UART        2u

typedef struct __attribute__((packed)) {
    uint8_t proto;
    uint8_t role;
    uint8_t link;
    uint8_t pages;
    uint16_t width;
    uint16_t height;
    uint16_t fw_ver;
    uint8_t mac[6];
} sc_hello_t;

typedef struct __attribute__((packed)) {
    uint16_t x, y, w, h, offset;
    uint8_t fmt, reserved;
} sc_tile_t;

typedef struct __attribute__((packed)) {
    uint8_t kind, code;
    uint16_t x, y;
} sc_key_t;

typedef struct __attribute__((packed)) {
    uint8_t kind, reserved;
    uint16_t n;
} sc_audio_t;

typedef struct {
    uint8_t type;
    uint8_t seq;
    uint16_t payload_len;
    const uint8_t *payload;
} sc_msg_t;

static inline uint8_t sc_xor(const uint8_t *len_byte, size_t n)
{
    uint8_t x = 0;
    for (size_t i = 0; i < n; i++) x ^= len_byte[i];
    return x;
}

static inline size_t sc_pack(uint8_t *out, size_t out_cap,
                             uint8_t type, uint8_t seq,
                             const void *payload, uint16_t payload_len)
{
    if (payload_len > SC_MAX_PAYLOAD) return 0;
    uint16_t len = (uint16_t)(2u + payload_len);
    size_t total = (size_t)SC_OVERHEAD + len;
    if (out_cap < total) return 0;
    out[0] = SC_MAGIC0; out[1] = SC_MAGIC1;
    out[2] = (uint8_t)(len & 0xFF); out[3] = (uint8_t)(len >> 8);
    out[4] = type; out[5] = seq;
    if (payload_len && payload) {
        const uint8_t *p = (const uint8_t *)payload;
        for (uint16_t i = 0; i < payload_len; i++) out[6 + i] = p[i];
    }
    out[6 + payload_len] = sc_xor(&out[2], (size_t)2 + len);
    return total;
}

static inline size_t sc_unpack(const uint8_t *p, size_t n, sc_msg_t *msg, bool *bad)
{
    if (bad) *bad = false;
    if (n < SC_OVERHEAD + 2) return 0;
    size_t i = 0;
    while (i + 1 < n && !(p[i] == SC_MAGIC0 && p[i + 1] == SC_MAGIC1)) i++;
    if (i + SC_OVERHEAD + 2 > n) return 0;
    if (i) { if (bad) *bad = true; return i; }
    uint16_t len = (uint16_t)p[2] | ((uint16_t)p[3] << 8);
    if (len < 2 || len > SC_MAX_LEN) { if (bad) *bad = true; return 2; }
    size_t total = (size_t)SC_OVERHEAD + len;
    if (n < total) return 0;
    uint8_t got = p[6 + (len - 2)];
    uint8_t expect = sc_xor(&p[2], (size_t)2 + len);
    if (got != expect) { if (bad) *bad = true; return total; }
    if (msg) {
        msg->type = p[4];
        msg->seq = p[5];
        msg->payload_len = (uint16_t)(len - 2);
        msg->payload = (len > 2) ? &p[6] : NULL;
    }
    return total;
}

#ifdef __cplusplus
}
#endif
