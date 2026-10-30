#!/usr/bin/env python3
"""Make a relinked image (mkobj.py + link.exe) loadable: the object carries the import tables and resources only as raw bytes, so copy the
original's import / IAT / resource data directory entries (they hold RVAs, which do not change: link.exe keeps the original section RVAs) and append
the original .rsrc section at its original RVA (all of them moved by the same delta when mkobj.py --pad shifted the sections).  Usage: pefix.py <linked.exe> [<original MFL.exe>]"""
import os, struct, sys
H = os.path.dirname(os.path.abspath(__file__))
out = sys.argv[1]
orig = sys.argv[2] if len(sys.argv) > 2 else os.path.normpath(os.path.join(H, "..", "..", "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe"))
o, n = open(orig, "rb").read(), bytearray(open(out, "rb").read())
def hdr(d):
    e = struct.unpack_from("<I", d, 0x3C)[0]
    return e, struct.unpack_from("<H", d, e + 6)[0], struct.unpack_from("<H", d, e + 20)[0]
oe, ons, oosz = hdr(o); ne, nns, nosz = hdr(n)
def secva(d, e, ns, osz, name):
    for i in range(ns):
        so = e + 24 + osz + i * 40
        if d[so:so + 8].rstrip(b"\0") == name: return struct.unpack_from("<I", d, so + 12)[0]
delta = secva(n, ne, nns, nosz, b".rdata") - secva(o, oe, ons, oosz, b".rdata")   # mkobj --pad moves the data sections (and .rsrc behind them) by the same amount
NORSRC = bool(os.environ.get("NO_RSRC"))               # NO_RSRC=1: mfl.res was on the link line (S3), the linker made .rsrc: keep its directory, append nothing
for i in (1, 2, 12):                                   # import, resource, IAT directories (a linker built import table, mkobj --imports-as-symbols, is kept)
    if NORSRC and i == 2: continue
    if i != 2 and struct.unpack_from("<I", n, ne + 24 + 96 + 8 * i)[0]: continue
    va, sz = struct.unpack_from("<II", o, oe + 24 + 96 + 8 * i)
    struct.pack_into("<II", n, ne + 24 + 96 + 8 * i, va + delta if va else 0, sz)
for i in range(nns):                                   # link.exe makes the mkobj --pad section read/write: clear that, so it really traps
    so = ne + 24 + nosz + i * 40
    if n[so:so + 8].rstrip(b"\0") == b".pad": struct.pack_into("<I", n, so + 36, 0x00000080)
for i in range(ons):
    so = oe + 24 + oosz + i * 40
    if NORSRC or o[so:so + 8].rstrip(b"\0") != b".rsrc": continue
    vs, va, rs, rp = struct.unpack_from("<IIII", o, so + 8)
    rsrc = bytearray(o[rp:rp + rs])
    def walk(off):                                     # resource data entries hold RVAs
        cnt = sum(struct.unpack_from("<HH", rsrc, off + 12))
        for k in range(cnt):
            x = struct.unpack_from("<I", rsrc, off + 16 + 8 * k + 4)[0]
            if x & 0x80000000: walk(x & 0x7FFFFFFF)
            else: struct.pack_into("<I", rsrc, x, struct.unpack_from("<I", rsrc, x)[0] + rdelta)
    fa = struct.unpack_from("<I", n, ne + 24 + 36)[0]; sa = struct.unpack_from("<I", n, ne + 24 + 32)[0]
    last = ne + 24 + nosz + (nns - 1) * 40
    lvs, lva = struct.unpack_from("<II", n, last + 8)
    rdelta = max(delta, (lva + lvs + sa - 1) // sa * sa - va)       # behind the last section when the image grew more than .rdata moved (M2 lib split)
    rdelta += int(os.environ.get("RSRC_PAD", "0"), 16)        # E2 experiment: move only the image END (the .rsrc section), code and data stay
    if rdelta: walk(0)
    va += rdelta
    struct.pack_into("<I", n, ne + 24 + 96 + 8 * 2, va)
    pos = (len(n) + fa - 1) // fa * fa
    n.extend(b"\0" * (pos - len(n))); n.extend(rsrc)
    sh = bytearray(o[so:so + 40]); struct.pack_into("<I", sh, 20, pos); struct.pack_into("<I", sh, 12, va)
    new = ne + 24 + nosz + nns * 40
    assert new + 40 <= struct.unpack_from("<I", n, ne + 24 + 60)[0], "no room for another section header"
    n[new:new + 40] = sh
    struct.pack_into("<H", n, ne + 6, nns + 1)
    struct.pack_into("<I", n, ne + 24 + 56, (va + vs + sa - 1) // sa * sa)       # SizeOfImage
    struct.pack_into("<I", n, ne + 24 + 8, struct.unpack_from("<I", n, ne + 24 + 8)[0] + rs)   # SizeOfInitializedData
os.unlink(out)                                          # link.exe ran as root in the container: replace the file instead of writing into it
open(out, "wb").write(n)
print("fixed", out)
