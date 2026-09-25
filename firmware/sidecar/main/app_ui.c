#include "app_ui.h"
#include "esp_log.h"

static const char *TAG = "ui";

void app_ui_start(void)
{
    ESP_LOGI(TAG, "LVGL hook point -- use sidecar-arduino until LVGL is vendored");
}

void app_ui_on_msg(const sc_msg_t *msg)
{
    ESP_LOGI(TAG, "rx type=0x%02X seq=%u n=%u", msg->type, msg->seq, msg->payload_len);
}
