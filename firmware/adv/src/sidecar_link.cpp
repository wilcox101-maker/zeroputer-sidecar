#include "sidecar_link.h"
#include "sidecar_config.h"
#include "sidecar_codec.h"

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <string.h>

static sc_codec_t codec;
static uint8_t peer_mac[6];
static bool have_peer = false;
static HardwareSerial &linkUart = Serial1;

static void on_msg(const sc_msg_t *msg, void *user)
{
    (void)user;
    if (msg->type == SC_HELLO && msg->payload_len >= sizeof(sc_hello_t)) {
        const sc_hello_t *h = (const sc_hello_t *)msg->payload;
        memcpy(peer_mac, h->mac, 6);
        have_peer = true;
        Serial.printf("[sidecar] HELLO from %02X:%02X:%02X:%02X:%02X:%02X\n",
                      peer_mac[0], peer_mac[1], peer_mac[2],
                      peer_mac[3], peer_mac[4], peer_mac[5]);
    } else if (msg->type == SC_PONG) {
        Serial.println("[sidecar] PONG");
    } else if (msg->type == SC_KEY && msg->payload_len >= sizeof(sc_key_t)) {
        const sc_key_t *k = (const sc_key_t *)msg->payload;
        Serial.printf("[sidecar] KEY kind=%u code=%u x=%u y=%u\n",
                      k->kind, k->code, k->x, k->y);
    } else if (msg->type == SC_BAT && msg->payload_len >= 2) {
        uint16_t mv = msg->payload[0] | (msg->payload[1] << 8);
        Serial.printf("[sidecar] BAT %u mV\n", mv);
    }
}

#if SIDECAR_LINK == SC_LINK_ESPNOW
static void on_rx(const uint8_t *mac, const uint8_t *data, int len)
{
    (void)mac;
    sc_codec_push(&codec, data, (size_t)len, on_msg, NULL);
}
#endif

void sidecar_begin()
{
    sc_codec_init(&codec);
#if SIDECAR_LINK == SC_LINK_UART
    linkUart.begin(SC_UART_BAUD, SERIAL_8N1, SC_UART_RX_PIN, SC_UART_TX_PIN);
    Serial.println("[sidecar] UART Grove G2/G1");
#elif SIDECAR_LINK == SC_LINK_ESPNOW
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    if (esp_now_init() != ESP_OK) {
        Serial.println("[sidecar] ESP-NOW init failed");
        return;
    }
    esp_now_register_recv_cb(on_rx);
    Serial.printf("[sidecar] ESP-NOW ch %d MAC %s\n",
                  SC_ESPNOW_CHANNEL, WiFi.macAddress().c_str());
#endif
}

static void raw_send(const uint8_t *frame, size_t n)
{
#if SIDECAR_LINK == SC_LINK_UART
    linkUart.write(frame, n);
#elif SIDECAR_LINK == SC_LINK_ESPNOW
    if (!have_peer) {
        uint8_t bcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        esp_now_peer_info_t info = {};
        memcpy(info.peer_addr, bcast, 6);
        info.channel = SC_ESPNOW_CHANNEL;
        info.encrypt = false;
        if (!esp_now_is_peer_exist(bcast)) {
            esp_now_add_peer(&info);
        }
        esp_now_send(bcast, frame, n);
        return;
    }
    esp_now_peer_info_t info = {};
    memcpy(info.peer_addr, peer_mac, 6);
    info.channel = SC_ESPNOW_CHANNEL;
    info.encrypt = false;
    if (!esp_now_is_peer_exist(peer_mac)) {
        esp_now_add_peer(&info);
    }
    esp_now_send(peer_mac, frame, n);
#endif
}

bool sidecar_send(uint8_t type, const void *payload, uint16_t len)
{
    uint8_t frame[SC_MAX_FRAME];
    size_t n = sc_codec_make(&codec, frame, sizeof(frame), type, payload, len);
    if (!n) {
        return false;
    }
    raw_send(frame, n);
    return true;
}

bool sidecar_send_log(const char *line)
{
    if (!line) {
        return false;
    }
    size_t n = strlen(line);
    if (n > SC_MAX_PAYLOAD) {
        n = SC_MAX_PAYLOAD;
    }
    return sidecar_send(SC_LOG, line, (uint16_t)n);
}

bool sidecar_send_text(uint8_t widget, const char *utf8)
{
    uint8_t buf[SC_MAX_PAYLOAD];
    buf[0] = widget;
    size_t n = utf8 ? strlen(utf8) : 0;
    if (n > SC_MAX_PAYLOAD - 1) {
        n = SC_MAX_PAYLOAD - 1;
    }
    memcpy(buf + 1, utf8, n);
    return sidecar_send(SC_TEXT, buf, (uint16_t)(n + 1));
}

bool sidecar_send_page(uint8_t page)
{
    return sidecar_send(SC_PAGE, &page, 1);
}

bool sidecar_paired()
{
    return have_peer;
}

void sidecar_poll()
{
#if SIDECAR_LINK == SC_LINK_UART
    uint8_t tmp[64];
    int n = linkUart.available();
    if (n > 0) {
        if (n > (int)sizeof(tmp)) {
            n = sizeof(tmp);
        }
        n = linkUart.readBytes(tmp, n);
        sc_codec_push(&codec, tmp, (size_t)n, on_msg, NULL);
    }
#endif
}

void sidecar_send_hello()
{
    sc_hello_t h = {};
    h.proto = SC_PROTO_VER;
    h.role = SC_ROLE_ADV;
    h.link = SIDECAR_LINK;
    h.pages = 0xFF;
    h.width = 240;
    h.height = 135;
    h.fw_ver = SC_FW_VER;
    WiFi.macAddress(h.mac);
    sidecar_send(SC_HELLO, &h, sizeof(h));
}
