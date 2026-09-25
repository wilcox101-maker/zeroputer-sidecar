#pragma once

/* LCDWiki ES3C28P -- do not use UART0 (43/44) for the sidecar link. */

#define ES_TFT_CS       10
#define ES_TFT_DC       46
#define ES_TFT_SCK      12
#define ES_TFT_MOSI     11
#define ES_TFT_MISO     13
#define ES_TFT_BL       45

#define ES_TP_SDA       16
#define ES_TP_SCL       15
#define ES_TP_RST       18
#define ES_TP_INT       17
#define ES_TP_ADDR      0x38

#define ES_UART1_RX     2
#define ES_UART1_TX     3
#define ES_EXPAND_IO14  14
#define ES_EXPAND_IO21  21

#define ES_BAT_ADC      9
#define ES_RGB          42

#define ES_WIDTH        240
#define ES_HEIGHT       320
