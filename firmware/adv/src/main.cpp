#include <Arduino.h>
#include <WiFi.h>
#include "sidecar_link.h"
#include "sidecar_config.h"

void setup()
{
    Serial.begin(115200);
    delay(200);
    Serial.println("Zeroputer ADV sidecar demo");
    sidecar_begin();
    sidecar_send_hello();
    sidecar_send_page(SC_PAGE_LOG);
    sidecar_send_text(SC_WID_TITLE, "ADV online");
    sidecar_send_log("waiting for sidecar HELLO");
}

void loop()
{
    sidecar_poll();

    static uint32_t last_hello = 0;
    if (!sidecar_paired() && millis() - last_hello > 1000) {
        last_hello = millis();
        sidecar_send_hello();
    }

    static uint32_t last_ping = 0;
    if (sidecar_paired() && millis() - last_ping > 2000) {
        last_ping = millis();
        uint32_t up = millis();
        sidecar_send(SC_PING, &up, sizeof(up));
    }

    if (Serial.available()) {
        String line = Serial.readStringUntil('\n');
        line.trim();
        if (line.length()) {
            sidecar_send_log(line.c_str());
            Serial.printf("-> LOG %s\n", line.c_str());
        }
    }
}
