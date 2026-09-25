#pragma once
#include "sidecar.h"

#ifndef SIDECAR_LINK
#define SIDECAR_LINK        SC_LINK_ESPNOW
#endif

#define SC_ESPNOW_CHANNEL   6
#define SC_UART_BAUD        115200
#define SC_UART_TX_PIN      2     /* Grove yellow */
#define SC_UART_RX_PIN      1     /* Grove white */
#define SC_FW_VER           0x0100
