#!/usr/bin/env python3
"""pinall.py <statecmp out> <ref statehash.bin> <frame> <phase> <out pins> [exclude va-lo-hi ...]: pin every word that differs at frame 0 in the summary"""
import os, struct, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import statecmp
out, ref, frame, phase, dst = sys.argv[1], sys.argv[2], int(sys.argv[3]), int(sys.argv[4]), sys.argv[5]
ex = [tuple(int(x, 16) for x in r.split("-")) for r in sys.argv[6:]]
vas, sec = [], False
for l in open(out):
    if l.startswith("words that ever differ"):
        sec = True
        continue
    s = l.split()
    if sec and l.startswith("  ") and len(s) >= 6 and s[2] == "0":
        va = int(s[0], 16)
        if not any(lo <= va < hi for lo, hi in ex):
            vas.append((va, s[1]))
mem = None
for h, regs, pages in statecmp.records(ref):
    if mem is None:
        mem = bytearray(statecmp.PAGE * len(pages))
    for i, d in pages.items():
        mem[i * statecmp.PAGE:(i + 1) * statecmp.PAGE] = d
    if h["frame"] == frame:
        break
with open(dst, "w") as f:
    for va, nm in vas:
        f.write("%d:%x:%x %s | menu residue: differs between the two control runs at loading frame 0 (cc_ph3), value = reference run ph3_1 at loading frame 0\n" % (phase, va, struct.unpack_from("<I", mem, va - h["base"])[0], nm))
print(len(vas), "pins")
