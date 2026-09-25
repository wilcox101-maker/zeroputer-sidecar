# Build order

1. Flash `firmware/sidecar-arduino` to the ES3C28P. Confirm backlight and battery millivolts on IO9.
2. Flash `firmware/adv` to the ADV. Serial monitor 115200. Confirm HELLO attempts on ESP-NOW.
3. Put both on channel 6. MACs print on both screens.
4. Enable SC_LOG on sidecar. Type on ADV serial: lines appear on 2.8".
5. Wire Grove UART, set SIDECAR_LINK SC_LINK_UART, repeat step 4.
6. Hook one real event from Bruce later.
7. Touch a sidecar button -> SC_KEY -> ADV serial echo.
8. Print the wallet. Only then add LVGL (`firmware/sidecar`).
