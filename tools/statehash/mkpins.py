#!/usr/bin/env python3
"""mkpins.py <ref statehash.bin> <frame> <phase> <ptrwords> <va-lo>-<va-hi> ... > pins.txt
pin lines (phase:va:value) with the values of the reference run at <frame> for every dword in the ranges (pointer words skipped).
run.sh (STATEHASH=1 PINFILE=pins.txt) gives them to the proxy, which writes them when the race phase first becomes <phase>."""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import statecmp, struct

ref, frame, phase, ptr = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), sys.argv[4]
ptrw = statecmp.read_words(ptr)
mem = None
for h, regs, pages in statecmp.records(ref):
    if mem is None:
        mem = bytearray(statecmp.PAGE * len(pages))
    for i, d in pages.items():
        mem[i * statecmp.PAGE:(i + 1) * statecmp.PAGE] = d
    if h["frame"] == frame:
        break
for r in sys.argv[5:]:
    lo, hi = (int(x, 16) for x in r.split("-"))
    for va in range(lo, hi, 4):
        if va not in ptrw:
            print("%d:%x:%x" % (phase, va, struct.unpack_from("<I", mem, va - h["base"])[0]))
