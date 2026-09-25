# Bruce integration notes

This repo ships a standalone ADV demo so you can prove the link without forking Bruce.

To hook Bruce later:

1. Copy `include/sidecar.h`, `common/sidecar_codec.*`, and `firmware/adv/src/sidecar_link.*` into a Bruce component.
2. Call `sidecar_send_log()` from the IR receive and RF hit paths.
3. Add `Config -> Display -> Sidecar {Off, ESP-NOW, UART}`.
4. Store peer MAC + link mode next to `brucePins.conf`.
5. Do not use EXT 14P pins that Hydra already owns.
6. Publish the result under AGPL-3.0 (Bruce's license).
