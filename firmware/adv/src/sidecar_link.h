#pragma once
#include <stddef.h>
#include <stdint.h>
#include "sidecar.h"

void sidecar_begin();
void sidecar_poll();
bool sidecar_send(uint8_t type, const void *payload, uint16_t len);
bool sidecar_send_log(const char *line);
bool sidecar_send_text(uint8_t widget, const char *utf8);
bool sidecar_send_page(uint8_t page);
bool sidecar_paired();
void sidecar_send_hello();
