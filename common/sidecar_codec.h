#pragma once

#include "sidecar.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t buf[SC_MAX_FRAME * 2];
    size_t n;
    uint8_t tx_seq;
    uint8_t rx_seq;
} sc_codec_t;

void sc_codec_init(sc_codec_t *c);

typedef void (*sc_on_msg)(const sc_msg_t *msg, void *user);

int sc_codec_push(sc_codec_t *c, const uint8_t *data, size_t n,
                  sc_on_msg cb, void *user);

size_t sc_codec_make(sc_codec_t *c, uint8_t *out, size_t cap,
                     uint8_t type, const void *payload, uint16_t plen);

#ifdef __cplusplus
}
#endif
