#include "app_link.h"
#include "app_ui.h"
#include "board_es3c28p.h"
#include "sidecar.h"

#include "driver/gpio.h"
#include "esp_mac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    gpio_config_t io = {
        .pin_bit_mask = 1ULL << ES_TFT_BL,
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io);
    gpio_set_level(ES_TFT_BL, 1);

    app_ui_start();
    app_link_start();

    sc_hello_t h = {
        .proto = SC_PROTO_VER,
        .role = SC_ROLE_SIDECAR,
        .link = SC_LINK_ESPNOW,
        .pages = 0xFF,
        .width = ES_WIDTH,
        .height = ES_HEIGHT,
        .fw_ver = 0x0100,
    };
    esp_read_mac(h.mac, ESP_MAC_WIFI_STA);

    while (1) {
        app_link_send(SC_HELLO, &h, sizeof(h));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
