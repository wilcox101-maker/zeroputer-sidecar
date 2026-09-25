#include "sidecar_codec.h"
#include <string.h>

void sc_codec_init(sc_codec_t *c)
{
    memset(c, 0, sizeof(*c));
}

int sc_codec_push(sc_codec_t *c, const uint8_t *data, size_t n,
                  sc_on_msg cb, void *user)
{
    if (!c || !data || !n) {
        return 0;
    }
    if (c->n + n > sizeof(c->buf)) {
        c->n = 0;
    }
    memcpy(c->buf + c->n, data, n);
    c->n += n;

    int good = 0;
    size_t off = 0;
    while (off < c->n) {
        sc_msg_t msg;
        bool bad = false;
        size_t used = sc_unpack(c->buf + off, c->n - off, &msg, &bad);
        if (used == 0) {
            break;
        }
        if (!bad && msg.type && cb) {
            cb(&msg, user);
            good++;
        }
        off += used;
    }
    if (off && off < c->n) {
        memmove(c->buf, c->buf + off, c->n - off);
    }
    c->n -= off;
    return good;
}

size_t sc_codec_make(sc_codec_t *c, uint8_t *out, size_t cap,
                     uint8_t type, const void *payload, uint16_t plen)
{
    uint8_t seq = c->tx_seq++;
    return sc_pack(out, cap, type, seq, payload, plen);
}
