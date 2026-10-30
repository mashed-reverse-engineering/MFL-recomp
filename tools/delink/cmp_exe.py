#!/usr/bin/env python3
"""cmp_exe.py <a.exe> <b.exe>: section by section byte compare of two linked exes (names, sizes, number of differing bytes, first differences).
mkrange.py check: the exe linked from the per-layer objects must equal the one linked from mfl.obj in every section."""
import struct, sys
def secs(p):
    d = open(p, "rb").read(); e = struct.unpack_from("<I", d, 0x3C)[0]; n = struct.unpack_from("<H", d, e + 6)[0]; o = struct.unpack_from("<H", d, e + 20)[0]
    r = {}
    for i in range(n):
        h = e + 24 + o + 40 * i; vs, va, rs, rp = struct.unpack_from("<IIII", d, h + 8); r[d[h:h + 8].rstrip(b"\0").decode()] = (va, vs, d[rp:rp + min(rs, vs)])
    return r
a, b = secs(sys.argv[1]), secs(sys.argv[2]); bad = 0
for k in sorted(set(a) | set(b)):
    if k not in a or k not in b: print(k, "only in one"); bad += 1; continue
    (va, vs, x), (vb, ws, y) = a[k], b[k]
    diff = [i for i in range(min(len(x), len(y))) if x[i] != y[i]]
    print("%-8s va %x/%x size %x/%x differing bytes %d%s" % (k, va, vb, vs, ws, len(diff) + abs(len(x) - len(y)), " first at " + ", ".join("%x" % (va + i) for i in diff[:6]) if diff else ""))
    bad += bool(diff) or va != vb or vs != ws
sys.exit(1 if bad else 0)
