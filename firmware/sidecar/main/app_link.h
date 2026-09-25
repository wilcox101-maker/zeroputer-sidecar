#pragma once
#include "sidecar.h"
void app_link_start(void);
void app_link_send(uint8_t type, const void *payload, uint16_t len);
