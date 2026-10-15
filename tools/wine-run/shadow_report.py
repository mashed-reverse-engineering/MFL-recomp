#!/usr/bin/env python3
"""shadow_report.py <shadowlog.bin> <shadow_names.txt> [--all]: per bridged function the result of the in-game shadow test (src/harness_shadow/shadow.c):
tested calls, return value differences and the data addresses that differ between the original and the C version (symbolised with src/symbols.txt)."""
import bisect, os, struct, sys
d = open(sys.argv[1], "rb").read()
names = [l.strip() for l in open(sys.argv[2]) if l.strip()]
syms = []
for l in open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "src", "symbols.txt")):
    p = l.split()
    if len(p) == 2:
        try: syms.append((int(p[1], 16), p[0]))
        except ValueError: pass
syms.sort(); keys = [s[0] for s in syms]
def sym(a):
    i = bisect.bisect_right(keys, a) - 1
    return "%s+%#x" % (syms[i][1], a - syms[i][0]) if i >= 0 and a - syms[i][0] < 0x10000 else "heap/stack"
SZ = 124
ok = bad = untested = 0
for i in range(len(d) // SZ):
    r = struct.unpack_from("<5i8I8I8I2I", d, i * SZ)
    fid, calls, tested, retd, nd = r[:5]
    if fid != i or i >= len(names) or calls == 0: continue
    addr, orig, ours = r[5:13], r[13:21], r[21:29]
    if tested == 0: untested += 1; continue
    if nd == 0 and retd == 0:
        ok += 1
        if "--all" in sys.argv: print("OK    %-45s calls %d tested %d" % (names[i], calls, tested))
        continue
    bad += 1
    print("DIFF  %-45s calls %d tested %d  return differs %d times (orig %#x ours %#x)  %d data words differ" % (names[i], calls, tested, retd, r[29], r[30], nd))
    for k in range(min(nd, 8)):
        print("        %08x %-40s orig %08x ours %08x" % (addr[k], sym(addr[k]), orig[k], ours[k]))
print("tested functions: %d identical, %d different; %d called but not tested" % (ok, bad, untested))
