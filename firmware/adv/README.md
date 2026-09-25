# ADV standalone demo

PlatformIO Arduino sketch. Not a Bruce fork.

- ESP-NOW broadcast until the sidecar HELLO arrives, then lock MAC
- Grove UART1 on G2/G1 when `SIDECAR_LINK` is `SC_LINK_UART`
- USB serial: type a line, it becomes `SC_LOG` on the 2.8"

Change link mode in `src/sidecar_config.h`.
