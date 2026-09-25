#include "app_link.h"
#include "app_ui.h"
#include "board_es3c28p.h"
#include "sidecar_codec.h"

#include "esp_log.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "string.h"

static const char *TAG = "link";
static sc_codec_t codec;

static void on_msg(const sc_msg_t *msg, void *user)
{
    (void)user;
    app_ui_on_msg(msg);
}

static void now_rx(const esp_now_recv_info_t *info, const uint8_t *data, int len)
{
    (void)info;
    sc_codec_push(&codec, data, (size_t)len, on_msg, NULL);
}

void app_link_send(uint8_t type, const void *payload, uint16_t len)
{
    uint8_t frame[SC_MAX_FRAME];
    size_t n = sc_codec_make(&codec, frame, sizeof(frame), type, payload, len);
    if (!n) {
        return;
    }
    uint8_t bcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    esp_now_send(bcast, frame, (int)n);
    uart_write_bytes(UART_NUM_1, (const char *)frame, n);
}

void app_link_start(void)
{
    sc_codec_init(&codec);
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_recv_cb(now_rx));

    uart_config_t uc = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, 1024, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uc));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, ES_UART1_TX, ES_UART1_RX,
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_LOGI(TAG, "ESP-NOW + UART1 ready");
}
