#!/usr/bin/env python3
"""Count the differing bytes of two memory dumps of regress.sh, ignoring fields that are not game behaviour.
Usage: cmpdump.py <cars|vehicles|aibots|statsrace|powerups> <a.bin> <b.bin> [<ctrl1.bin> <ctrl2.bin>: bytes that differ between these two are ignored]
Ignored (verified by looking at control-vs-control and control-vs-overlay dumps):
 cars      +0xB10 (field_b10 = real time tick stored by car_deactivate), the 6 padding words of the two RwMatrix at +0x928 / +0x968
 vehicles  heap pointers (RenderWare frame handles) that moved by a constant between two exes (the overlay section shifts the heap by 0x10000)
 vehicles  +0x220..+0x22B of every 0x2AC byte record (3 words, only set for the AI vehicles): stable within one exe, changes with the layout of the
           overlay section; ddmin blames a byte identical (MATCH) function, so it is not code (2026-10-08)
 powerups  the first word of each record (RpClump heap pointer)"""
import os, struct, sys

def words(d):
    return [struct.unpack_from("<I", d, i)[0] for i in range(0, len(d) - 3, 4)]

def main():
    kind, fa, fb = sys.argv[1:4]
    try:
        a, b = open(fa, "rb").read(), open(fb, "rb").read()
    except OSError:      # e.g. runs=1 has no second control run
        print("n/a")
        return
    n = max(len(a), len(b))
    ign = set()
    if kind == "cars":
        for c in range(16):
            base = c * 0xD04
            ign.update(range(base + 0xB10, base + 0xB14))
            for m in (0x928, 0x968):
                for pad in (0x1C, 0x2C, 0x3C):
                    ign.update(range(base + m + pad, base + m + pad + 4))
    elif kind == "powerups":
        for r in range(25):
            ign.update(range(r * 0x50, r * 0x50 + 4))
    elif kind == "vehicles":
        for v in range(4):
            ign.update(range(v * 0x2AC + 0x220, v * 0x2AC + 0x22C))
        wa, wb = words(a), words(b)
        for i, (x, y) in enumerate(zip(wa, wb)):
            if x != y and 0x01000000 <= x < 0x20000000 and 0x01000000 <= y < 0x20000000 and abs(x - y) < 0x100000:   # heap pointers; the shift is the size of the overlay section (+ a few bytes), floats never live in this range
                ign.update(range(i * 4, i * 4 + 4))
    if os.environ.get("CMP_DELTA"):      # a is the control, b the linked exe (tools/delink/state_compare.sh): .data moved by CMP_DELTA, so data pointers differ by exactly that; heap pointers by the size difference of the images
        dl = int(os.environ["CMP_DELTA"], 16)
        for i, (x, y) in enumerate(zip(words(a), words(b))):
            if x != y and ((0x5dd000 <= x < 0x9f9000 and y - x == dl) or (0x01000000 <= x < 0x20000000 and 0x01000000 <= y < 0x20000000 and abs(x - y) < 0x1000000)):
                ign.update(range(i * 4, i * 4 + 4))
    if len(sys.argv) >= 6:      # noise mask: the bytes in which two control runs of the same exe differ are not evidence against the overlay (course 2, 5, 6 are not deterministic under load)
        try:
            m1, m2 = open(sys.argv[4], "rb").read(), open(sys.argv[5], "rb").read()
            ign.update(i for i in range(min(len(m1), len(m2))) if m1[i:i + 1] != m2[i:i + 1])
        except OSError:
            pass
    print(sum(1 for i in range(n) if a[i:i + 1] != b[i:i + 1] and i not in ign))

main()
