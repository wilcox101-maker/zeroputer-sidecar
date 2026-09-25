#!/usr/bin/env python3
"""Pack / unpack Zeroputer sidecar frames for serial tests."""
from __future__ import annotations

import argparse
import sys

MAGIC = bytes((0xAA, 0x55))

TYPES = {
    0x01: "HELLO",
    0x02: "PAGE",
    0x03: "TEXT",
    0x04: "TILE",
    0x05: "KEY",
    0x06: "AUDIO",
    0x07: "BAT",
    0x08: "LOG",
    0x09: "ACK",
    0x0A: "PING",
    0x0B: "PONG",
}


def xor_bytes(data: bytes) -> int:
    x = 0
    for b in data:
        x ^= b
    return x


def pack(type_id: int, seq: int, payload: bytes) -> bytes:
    if len(payload) > 240:
        raise ValueError("payload > 240")
    length = 2 + len(payload)
    body = bytes((length & 0xFF, length >> 8, type_id & 0xFF, seq & 0xFF)) + payload
    return MAGIC + body + bytes((xor_bytes(body),))


def unpack(buf: bytes):
    i = buf.find(MAGIC)
    if i < 0 or i + 6 > len(buf):
        return None
    length = buf[i + 2] | (buf[i + 3] << 8)
    total = 4 + length + 1
    if i + total > len(buf):
        return None
    frame = buf[i : i + total]
    body = frame[2:-1]
    if xor_bytes(body) != frame[-1]:
        raise ValueError("bad xor")
    return {
        "type": frame[4],
        "name": TYPES.get(frame[4], "UNK"),
        "seq": frame[5],
        "payload": frame[6:-1],
        "raw": frame,
    }


def main() -> int:
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)

    p = sub.add_parser("pack")
    p.add_argument("type", help="name or 0xNN")
    p.add_argument("payload", nargs="?", default="")
    p.add_argument("--seq", type=int, default=0)

    u = sub.add_parser("unpack")
    u.add_argument("hex")

    args = ap.parse_args()
    if args.cmd == "pack":
        rev = {v: k for k, v in TYPES.items()}
        if args.type.upper() in rev:
            t = rev[args.type.upper()]
        else:
            t = int(args.type, 0)
        frame = pack(t, args.seq, args.payload.encode())
        sys.stdout.write(frame.hex() + "\n")
        return 0
    msg = unpack(bytes.fromhex(args.hex))
    if not msg:
        print("no frame", file=sys.stderr)
        return 1
    print(f"{msg['name']} seq={msg['seq']} payload={msg['payload']!r}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
