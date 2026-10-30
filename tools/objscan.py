#!/usr/bin/env python3
"""Object-level scan for raw image data addresses: an instruction operand in the image's data range (0x5dd000-0x9f9000) that carries no COFF relocation.
In a linked build (tools/delink/link_ours.sh) the data moves, so such an operand reads or writes the wrong memory (cl_weapon_shotgun_load_assets:
`slot_cursor < 0x6aac10` = the 2026-10-30 boot crash).  tools/rawaddr.py is the source level twin; it misses compares and offsets without a matching original relocation.
Usage: objscan.py <objs list: "src obj" per line (build_overlay.py OBJLIST=...)> [--quiet]   Exit 1 when a hit is not allowed.
Allowed: masks / colours below, and `src/objscan_allow.txt` lines `<source basename> <hex value>  # reason` (checked false positives)."""
import collections, os, re, subprocess, sys
from concurrent.futures import ThreadPoolExecutor
LO, HI = 0x5dd000, 0x9f9000
MASKS = {0x800000, 0x804000, 0x8080ff, 0x92312e, 0x62c3db}      # bit masks and colours that merely look like addresses
allow = set()
ap = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "src", "objscan_allow.txt")
if os.path.exists(ap):
    for l in open(ap):
        q = l.split("#")[0].split()
        if len(q) == 2: allow.add((q[0], int(q[1], 16)))

def scan(item):
    src, obj = item
    out = subprocess.run(["objdump", "-d", "-r", "-M", "intel", obj], capture_output=True, text=True).stdout.splitlines()
    hits = []
    for i, l in enumerate(out):
        m = re.match(r"\s+([0-9a-f]+):\t([0-9a-f ]+)\t(.*)", l)
        if not m: continue
        if i + 1 < len(out) and re.search(r"dir32|DISP32|IMAGE_REL|R_386", out[i + 1]) and not re.match(r"\s+[0-9a-f]+:\t", out[i + 1]): continue
        for v in re.findall(r"0x([0-9a-f]{6,8})\b", m.group(3)):
            a = int(v, 16)
            if LO <= a < HI and a not in MASKS and (os.path.basename(src), a) not in allow: hits.append((m.group(3).strip(), a))
    return src, hits

def main():
    lst = [l.split() for l in open(sys.argv[1]) if len(l.split()) >= 2]
    with ThreadPoolExecutor(4) as ex: res = [r for r in ex.map(scan, lst) if r[1]]
    for src, h in sorted(res, key=lambda r: -len(r[1])):
        if "--quiet" not in sys.argv: print("%s: %d  e.g. %s" % (os.path.basename(src), len(h), "; ".join("%s [0x%x]" % x for x in h[:2])))
    print("objscan: %d unrelocated image data addresses in %d files" % (sum(len(h) for _, h in res), len(res)))
    sys.exit(1 if res else 0)
main()
