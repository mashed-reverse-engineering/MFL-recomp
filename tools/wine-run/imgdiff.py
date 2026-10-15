#!/usr/bin/env python3
"""imgdiff.py a.png b.png -> mean absolute difference per channel (0..255) of two 8 bit RGB PNGs (stdlib only, handles the PNG filters).
Two runs of the same scenario are never pixel identical (animations, sound-driven effects), but similar images give a small value (< ~10); a different
scene gives 40+."""
import struct, sys, zlib


def load(p):
    d = open(p, "rb").read()
    assert d[:8] == b"\x89PNG\r\n\x1a\n"
    i, idat, w = 8, b"", None
    while i < len(d):
        n, t = struct.unpack(">I4s", d[i:i + 8])
        body = d[i + 8:i + 8 + n]
        if t == b"IHDR":
            w, h, bd, ct = struct.unpack(">IIBB", body[:10])
        elif t == b"IDAT":
            idat += body
        i += 12 + n
    bpp = {2: 3, 6: 4, 0: 1}[ct]
    raw, stride, prev, rows = zlib.decompress(idat), w * bpp, bytearray(w * bpp), []
    for y in range(h):
        f = raw[y * (stride + 1)]
        cur = bytearray(raw[y * (stride + 1) + 1:(y + 1) * (stride + 1)])
        for x in range(stride):
            a = cur[x - bpp] if x >= bpp else 0
            b = prev[x]
            c = prev[x - bpp] if x >= bpp else 0
            if f == 1: cur[x] = (cur[x] + a) & 255
            elif f == 2: cur[x] = (cur[x] + b) & 255
            elif f == 3: cur[x] = (cur[x] + ((a + b) >> 1)) & 255
            elif f == 4:
                pa, pb, pc = abs(b - c), abs(a - c), abs(a + b - 2 * c)
                cur[x] = (cur[x] + (a if pa <= pb and pa <= pc else (b if pb <= pc else c))) & 255
        rows.append(cur); prev = cur
    return w, h, bpp, rows


def diff(pa, pb, step=4):
    wa, ha, ba, ra = load(pa)
    wb, hb, bb, rb = load(pb)
    if (wa, ha, ba) != (wb, hb, bb):
        return 255.0
    tot = n = 0
    for y in range(0, ha, step):
        a, b = ra[y], rb[y]
        for x in range(0, wa * ba, step if ba == 1 else ba * step):
            for c in range(min(ba, 3)):
                tot += abs(a[x + c] - b[x + c]); n += 1
    return tot / max(n, 1)


if __name__ == "__main__":
    print("%.1f" % diff(sys.argv[1], sys.argv[2]))
