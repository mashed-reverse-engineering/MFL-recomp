#!/usr/bin/env python3
"""Compare a relinked image (mkobj.py object + link.exe) with the original MFL.exe, section by section.
Every pointer site of output/delink_relocs.csv that mkobj relocated must hold (original value - old target section VA + new target section VA);
every other byte must be identical.  So a build at the original base must be byte identical, and a build at a shifted base differs exactly at the
relocated sites by the base delta (un-shifting those sites gives the original back).
Usage: check_roundtrip.py <linked.exe> [<more.exe> ...]"""
import csv, os, struct, sys
H = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(H, "..", ".."))
ORIG = os.path.normpath(os.path.join(G, "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe"))
def pe(path):
    d = open(path, "rb").read()
    e = struct.unpack_from("<I", d, 0x3C)[0]; ns = struct.unpack_from("<H", d, e + 6)[0]; osz = struct.unpack_from("<H", d, e + 20)[0]
    base = struct.unpack_from("<I", d, e + 24 + 28)[0]; entry = struct.unpack_from("<I", d, e + 24 + 16)[0]
    s = {}
    for i in range(ns):
        o = e + 24 + osz + i * 40
        nm = d[o:o + 8].rstrip(b"\0").decode(); vs, va, rs, rp = struct.unpack_from("<IIII", d, o + 8)
        s[nm] = (base + va, vs, d[rp:rp + min(vs, rs)])
    return base, entry, s
ob, oe, os_ = pe(ORIG)
# the sites mkobj relocated: same selection rules (kinds header / rsrc / rva are left alone, overlaps dropped in confidence order)
rows = sorted(csv.DictReader(open(os.path.join(G, "output", "delink_relocs.csv"))), key=lambda r: ["high", "medium", "low"].index(r["confidence"]))
sites, taken = {}, set()
for r in rows:
    s, t = int(r["site"], 16), int(r["target"], 16)
    if r["kind"] in ("header", "rsrc", "unknown", "rva", "const", "rel32") or any(s + k in taken for k in range(4)): continue
    taken.update(range(s, s + 4)); sites[s] = t
def osec(v, secs):
    for n, (va, vs, raw) in secs.items():
        if va <= v < va + vs: return n
for path in sys.argv[1:]:
    nb, ne, ns_ = pe(path)
    print("%s: base %08x entry %s" % (os.path.basename(path), nb, "same" if ne == oe else "DIFFERENT %x/%x" % (ne, oe)))
    ok = bad = other = 0
    for n, (va, vs, raw) in os_.items():
        if n == ".rsrc": continue
        if n not in ns_: print("  section %s missing" % n); continue
        nva, nvs, nraw = ns_[n]
        if nvs != vs or len(nraw) != len(raw): print("  %s size %x/%x raw %x/%x" % (n, nvs, vs, len(nraw), len(raw)))
        exp = bytearray(raw)
        for s, t in sites.items():
            if va <= s < va + len(raw):
                tn = osec(t, os_); delta = ns_[tn][0] - os_[tn][0]
                struct.pack_into("<I", exp, s - va, (t + delta) & 0xFFFFFFFF)
        diff = [i for i in range(min(len(exp), len(nraw))) if exp[i] != nraw[i]]
        nsites = sum(1 for s in sites if va <= s < va + len(raw))
        bsites = {s for s in sites if va <= s < va + len(raw) and nraw[s - va:s - va + 4] != exp[s - va:s - va + 4]}
        changed = sum(1 for s in sites if va <= s < va + len(raw) and nraw[s - va:s - va + 4] != raw[s - va:s - va + 4])
        stray = [i for i in diff if not any(va + i - k in sites for k in range(4))]
        print("  %-8s at %08x (orig %08x): %6d relocated sites, %6d changed vs original, %d wrong, %d other differing bytes%s" % (
            n, nva, va, nsites, changed, len(bsites), len(stray), (" first at %08x" % (va + stray[0])) if stray else ""))
