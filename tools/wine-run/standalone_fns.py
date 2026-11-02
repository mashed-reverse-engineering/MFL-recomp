#!/usr/bin/env python3
"""standalone_fns.py <ours.exe> <ours.map> <out csv> [gdbcov names out]: every live function of a standalone link -> csv "name,va,size,layer,obj" (+ "name hexaddr" file for gdbcov).
Functions = map 'f' symbols in .text, minus labels (_LAB_/_caseD_/_switchdata/__rng_), int3 filled originals (dropped by a C replacement or a source lib) and the 5 byte `jmp` entry stubs of named
layer objects.  Layer: original VA of the name (src/symbols.txt, tools/delink/layers.py), for named layer objects the VA itself; names without a symbol take the majority layer of their object file."""
import collections, csv, os, re, struct, sys
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
sys.path.insert(0, G + "/tools/delink")
from layers import layer, item_at
exe, mp, out = sys.argv[1:4]
d = open(exe, "rb").read(); e = struct.unpack_from("<I", d, 0x3C)[0]; ns, osz = struct.unpack_from("<H", d, e + 6)[0], struct.unpack_from("<H", d, e + 20)[0]
secs = [struct.unpack_from("<IIII", d, e + 24 + osz + k * 40 + 8) for k in range(ns)]      # vsize, rva, rawsize, rawptr
def byte(va, n=1):
    for vs, rva, rs, rp in secs:
        if rva <= va - 0x400000 < rva + max(vs, rs): o = rp + va - 0x400000 - rva; return d[o:o + n]
    return b""
sym = {}
for l in open(G + "/src/symbols.txt"):
    p = l.split()
    if len(p) == 2: sym.setdefault(p[0], int(p[1], 16))
NAMED = {"app.obj", "RenderWare.obj", "Audio.obj", "D3DX.obj", "rwp.obj", "DirectShow.obj", "CRT.obj", "other.obj"}
JUNK = re.compile(r"^(_LAB_|_caseD_|_switchdata|__rng_|_?\$\$\$|_?PTR_|_?DAT_|_?s_|_?u_|_?thunk_)")
syms = []      # (va, name, obj)
sec_f = False
for l in open(mp):
    p = l.split()
    if len(p) >= 5 and p[0].startswith("0001:") and p[-2] == "f": syms.append((int(p[2], 16), p[1], p[-1]))
syms.sort()
real = [s for s in syms if not JUNK.match(s[1])]
addrs = sorted({s[0] for s in real})
def orig(n):
    n = re.sub(r"@\d+$", "", n).lstrip("_@")
    for c in (n, re.sub(r"_r$", "", n), re.sub(r"_orig$", "", n)):
        if c in sym: return sym[c]
    m = re.search(r"(?:gap|FUN|SUB)_([0-9a-f]{6,8})", n)      # generated names carry the original VA
    if m: return int(m.group(1), 16)
    m = re.search(r"_([0-9a-f]{6})$", n)      # trailing original VA (_lib_4f4320, *_cb_49fdc0)
    if m and 0x401000 <= int(m.group(1), 16) < 0x5dd000: return int(m.group(1), 16)
def lay(va):
    it = item_at(va); return layer(va, it[3], it[4]) if it else None
rows = {}
for va, n, obj in real:
    if va in rows: continue
    b = byte(va, 1)
    if not b or b == b"\xcc": continue
    if obj in NAMED and b == b"\xe9": continue
    nx = next((a for a in addrs if a > va), va + 16)
    sz = nx - va; body = byte(va, min(sz, 0x8000)); sz = len(body.rstrip(b"\xcc\x90")) or 1
    ov = va if obj in NAMED and va < 0x5dd000 else orig(n)
    rows[va] = [n, va, sz, lay(ov) if ov else None, obj]
unres = 0; prev = None      # unresolved names (not in symbols.txt): object majority, else the layer of the previous resolved function by address (the link groups the objects by layer)
maj = collections.defaultdict(collections.Counter)
for r in rows.values():
    if r[3]: maj[r[4]][r[3]] += 1
for va in sorted(rows):
    r = rows[va]
    if not r[3]:
        r[3] = maj[r[4]].most_common(1)[0][0] if maj[r[4]] else (prev or "unresolved"); unres += 1
    else: prev = r[3]
w = csv.writer(open(out, "w")); w.writerow(["name", "va", "size", "layer", "obj"])
for va in sorted(rows): r = rows[va]; w.writerow([r[0], "%08x" % va, r[2], r[3], r[4]])
if len(sys.argv) > 4: open(sys.argv[4], "w").write("".join("%s %08x\n" % (r[0], va) for va, r in sorted(rows.items())))
c = collections.Counter(r[3] for r in rows.values()); print(len(rows), "functions, unresolved", unres, dict(c))
