#!/usr/bin/env python3
"""Byte compare the autocompiled functions (tools/autocompile.py output: <dir>/ok_NNN.c chunks) with the original code and list the ones that are byte identical (relocation fields masked).
Usage: automatch.py <autocompile dir> <out.csv> [jobs=6]     csv: chunk,symbol,va,status (status MATCH or the asmdiff class)
Chunks are independent compiles (one persistent compiler container, several at the same time)."""
import glob, os, re, sys
from concurrent.futures import ThreadPoolExecutor
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import cmpfn
R = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "src")
sym = {}
for l in open(os.path.join(R, "symbols.txt")):
    p = l.split()
    if len(p) == 2:
        try: sym.setdefault(p[0], int(p[1], 16))
        except ValueError: pass
d, out = sys.argv[1], sys.argv[2]
ap = os.path.join(d, "addrs.txt")
if os.path.exists(ap):          # addresses from the decompilation headers (see autocompile.py), not the first symbol of that name
    sym = {l.split()[0]: int(l.split()[1], 16) for l in open(ap) if len(l.split()) == 2}
jobs = int(sys.argv[3]) if len(sys.argv) > 3 else 6
fsz = {}
_fp = os.path.join(R, "..", "output", "fnsizes.txt")
if os.path.exists(_fp):
    for l_ in open(_fp):
        p_ = l_.split()
        if len(p_) == 3: fsz[int(p_[0], 16)] = int(p_[2])
def is_thunk(va):
    """a `jmp rel32` thunk: Ghidra decompiles a thunk as its TARGET (the body of the other function), so a match against the thunk bytes means nothing"""
    return cmpfn.exe_bytes(va, 1)[:1] == b"\xe9" and fsz.get(va, 99) <= 5
def one(f):
    text = open(f, errors="replace").read()
    names = re.findall(r"^[A-Za-z_][\w\s\*]*?\b(\w+)\([^;{]*\)\s*\n\{", text, re.M)
    items = [(n, sym[n]) for n in names if n in sym]
    r = cmpfn.check(f, items, os.environ.get("AM_FLAGS", "/O2").split())
    rows = []
    for n, va in items:
        ok, det = r.get(n, (False, "missing"))
        if is_thunk(va):
            rows.append((os.path.basename(f), n, va, "THUNK", "", "")); continue
        m = re.match(r"\[(\w+) ([0-9.]+)\] ?(.*?) ours ", det)
        rows.append((os.path.basename(f), n, va, "MATCH" if ok else (m.group(1) if m else det[:30].replace(",", " ")), m.group(2) if m else "", (m.group(3) if m else "").replace(",", ";")[:150]))
    return rows
files = sorted(glob.glob(os.path.join(d, "ok_*.c")))
with ThreadPoolExecutor(jobs) as ex:
    allrows = [r for rows in ex.map(one, files) for r in rows]
with open(out, "w") as o:
    for f, n, va, st, sc, why in allrows: o.write("%s,%s,%08x,%s,%s,%s\n" % (f, n, va, st, sc, why))
import collections
print(len(allrows), "functions;", dict(collections.Counter(r[3] for r in allrows)))
