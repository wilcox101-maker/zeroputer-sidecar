#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <esp_now.h>
#include <string.h>

#include "sidecar.h"
#include "sidecar_codec.h"
#include "board_es3c28p.h"

#define SC_ESPNOW_CHANNEL 6
#define SC_UART_BAUD      115200

TFT_eSPI tft;
sc_codec_t codec;
uint8_t page = SC_PAGE_BOOT;
uint8_t peer[6];
bool have_peer = false;
HardwareSerial linkUart(1);
uint8_t log_lines = 0;

bool sidecar_send_local(uint8_t type, const void *payload, uint16_t len);

static void paint_boot(const char *msg)
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(8, 8);
    tft.println("ES3C28P sidecar");
    tft.setTextSize(1);
    tft.setCursor(8, 40);
    tft.println(WiFi.macAddress());
    tft.setCursor(8, 56);
    tft.println(msg);
}

static void on_msg(const sc_msg_t *msg, void *user)
{
    (void)user;
    if (msg->type == SC_HELLO && msg->payload_len >= sizeof(sc_hello_t)) {
        const sc_hello_t *h = (const sc_hello_t *)msg->payload;
        memcpy(peer, h->mac, 6);
        have_peer = true;
        paint_boot("ADV paired");
    } else if (msg->type == SC_PAGE && msg->payload_len >= 1) {
        page = msg->payload[0];
        tft.fillScreen(TFT_NAVY);
        tft.setCursor(8, 8);
        tft.printf("page %u", page);
        log_lines = 0;
    } else if (msg->type == SC_LOG) {
        if (log_lines > 24) {
            tft.fillRect(0, 24, ES_WIDTH, ES_HEIGHT - 24, TFT_NAVY);
            log_lines = 0;
        }
        tft.setCursor(4, 28 + log_lines * 12);
        tft.setTextColor(TFT_GREEN, TFT_NAVY);
        for (uint16_t i = 0; i < msg->payload_len && i < 38; i++) {
            tft.print((char)msg->payload[i]);
        }
        log_lines++;
    } else if (msg->type == SC_TEXT && msg->payload_len >= 1) {
        uint8_t wid = msg->payload[0];
        tft.setCursor(4, 8 + wid * 14);
        tft.setTextColor(TFT_YELLOW, TFT_NAVY);
        for (uint16_t i = 1; i < msg->payload_len; i++) {
            tft.print((char)msg->payload[i]);
        }
    } else if (msg->type == SC_PING) {
        sidecar_send_local(SC_PONG, msg->payload, msg->payload_len);
    }
}

static void on_rx(const uint8_t *mac, const uint8_t *data, int len)
{
    (void)mac;
    sc_codec_push(&codec, data, (size_t)len, on_msg, NULL);
}

bool sidecar_send_local(uint8_t type, const void *payload, uint16_t len)
{
    uint8_t frame[SC_MAX_FRAME];
    size_t n = sc_codec_make(&codec, frame, sizeof(frame), type, payload, len);
    if (!n) {
        return false;
    }
    if (have_peer) {
        esp_now_peer_info_t info = {};
        memcpy(info.peer_addr, peer, 6);
        info.channel = SC_ESPNOW_CHANNEL;
        info.encrypt = false;
        if (!esp_now_is_peer_exist(peer)) {
            esp_now_add_peer(&info);
        }
        esp_now_send(peer, frame, n);
    } else {
        uint8_t bcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        esp_now_peer_info_t info = {};
        memcpy(info.peer_addr, bcast, 6);
        info.channel = SC_ESPNOW_CHANNEL;
        info.encrypt = false;
        if (!esp_now_is_peer_exist(bcast)) {
            esp_now_add_peer(&info);
        }
        esp_now_send(bcast, frame, n);
    }
    linkUart.write(frame, n);
    return true;
}

static void send_hello()
{
    sc_hello_t h = {};
    h.proto = SC_PROTO_VER;
    h.role = SC_ROLE_SIDECAR;
    h.link = SC_LINK_ESPNOW;
    h.pages = 0xFF;
    h.width = ES_WIDTH;
    h.height = ES_HEIGHT;
    h.fw_ver = 0x0100;
    WiFi.macAddress(h.mac);
    sidecar_send_local(SC_HELLO, &h, sizeof(h));
}

void setup()
{
    Serial.begin(115200);
    pinMode(ES_TFT_BL, OUTPUT);
    digitalWrite(ES_TFT_BL, HIGH);
    tft.init();
    tft.setRotation(0);
    paint_boot("ESP-NOW + UART1");

    sc_codec_init(&codec);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    if (esp_now_init() == ESP_OK) {
        esp_now_register_recv_cb(on_rx);
    }
    linkUart.begin(SC_UART_BAUD, SERIAL_8N1, ES_UART1_RX, ES_UART1_TX);
    send_hello();
}

void loop()
{
    uint8_t tmp[64];
    int n = linkUart.available();
    if (n > 0) {
        if (n > (int)sizeof(tmp)) n = sizeof(tmp);
        n = linkUart.readBytes(tmp, n);
        sc_codec_push(&codec, tmp, (size_t)n, on_msg, NULL);
    }

    static uint32_t last = 0;
    if (millis() - last > 1000) {
        last = millis();
        send_hello();
        uint16_t mv = (uint16_t)((analogRead(ES_BAT_ADC) * 2 * 3300) / 4095);
        sidecar_send_local(SC_BAT, &mv, 2);
    }
}
