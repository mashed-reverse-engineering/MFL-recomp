#!/usr/bin/env python3
"""mapcheck: verify the pinned layout of a globgen check link.  Usage: mapcheck.py <workdir with gen output, g.exe, g.map>
1. every generated name: map address - original address is ONE constant per section (and the aliases/interior labels agree);
2. byte check of the initialised sections: app/gap items equal the original, pointer words excluded;
3. pointer words: linked word == map address of the intended symbol + addend.
Exit 0 only if everything is 0."""
import csv, os, re, struct, sys

G = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
W = sys.argv[1]


def load(path):
    d = open(path, "rb").read()
    pe = struct.unpack_from("<I", d, 0x3c)[0]
    base = struct.unpack_from("<I", d, pe + 52)[0]
    secs = []
    for i in range(struct.unpack_from("<H", d, pe + 6)[0]):
        o = pe + 24 + struct.unpack_from("<H", d, pe + 20)[0] + 40 * i
        vs, va, rs, rp = struct.unpack_from("<IIII", d, o + 8)
        secs.append((base + va, vs, d[rp:rp + rs]))

    def rd(a, n):
        for s, vs, raw in secs:
            if s <= a < s + max(vs, len(raw)):
                o = a - s
                return (raw[o:o + n] + bytes(n))[:n]
        return bytes(n)
    return rd


orig, new = load(os.path.join(G, "game", "MFL.exe")), load(os.path.join(W, "g.exe"))
mp = {}
for l in open(os.path.join(W, "g.map"), errors="replace"):
    m = re.match(r"\s*[0-9a-f]{4}:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})\s", l)
    if m: mp[m.group(1)] = int(m.group(2), 16)
rows = list(csv.DictReader(open(os.path.join(W, "globals_layout.csv"))))
delta, bad, missing = {}, 0, 0
for r in rows:
    a = mp.get("_" + r["name"])
    if a is None: missing += 1; continue
    dl = a - int(r["va"], 16)
    delta.setdefault(r["section"], {}).setdefault(dl, []).append(r["name"])
for s, v in delta.items():
    main = max(v, key=lambda k: len(v[k]))
    off = [(k, n[:3]) for k, n in v.items() if k != main]
    print("%s: delta %#x, %d names ok, %d mismatching (%s)" % (s, main, len(v[main]), sum(len(n) for k, n in v.items() if k != main), off[:3]))
    bad += sum(len(n) for k, n in v.items() if k != main)
print("names not in map:", missing)
D = {s: max(v, key=lambda k: len(v[k])) for s, v in delta.items()}
sec_of = lambda va: next((s for s, (lo, hi) in {"rdata": (0x5dd000, 0x5fa5c8), "data": (0x5fb000, 0x64d000), "bss": (0x64d000, 0x949d64)}.items() if lo <= va < hi), None)
# aliases
ab = 0
lay = {r["name"]: int(r["va"], 16) for r in rows}
for l in open(os.path.join(W, "aliases_gen.txt")):
    if l[0] == "#": continue
    n, o, p = l.split()
    a = mp.get("_" + n)
    if a is None: ab += 1; continue
    if a - (lay[p] + int(o)) != D[sec_of(lay[p])]: ab += 1
print("alias mismatches:", ab)
# pointers
ptrs = list(csv.DictReader(open(os.path.join(W, "ptrs.csv"))))
pw, pm = set(), 0
pw_sites = {int(p["site"], 16) for p in ptrs}
for p in ptrs:
    s = int(p["site"], 16)
    pw.update(range(s, s + 4))
    ns = s + D[sec_of(s)]
    sym = mp.get(p["link"])           # exact link name (decorated for stdcall/fastcall targets)
    if sym is None or struct.unpack("<I", new(ns, 4))[0] != sym + int(p["addend"]): pm += 1
print("pointer words: %d, wrong: %d" % (len(ptrs), pm))
# coverage (F4): every reloc site inside a generated non-pe, non-bss item is a pointer or listed in raw.csv
raws = {int(r["site"], 16) for r in csv.DictReader(open(os.path.join(W, "raw.csv")))}
cov = 0
spans = sorted((int(r["va"], 16), int(r["va"], 16) + int(r["size"])) for r in rows if r["kind"] != "pe" and r["section"] != "bss")
import bisect
st = [x[0] for x in spans]
for r in csv.DictReader(open(os.path.join(G, "output/delink_relocs.csv"))):
    if r["kind"] in ("const", "header", "rel32"): continue
    s = int(r["site"], 16)
    i = bisect.bisect_right(st, s) - 1
    if i >= 0 and s < spans[i][1] and s not in pw_sites and s not in raws: cov += 1
print("reloc sites neither pointer nor raw:", cov)
# bytes
nb = diff = 0
for r in rows:
    if r["kind"] == "pe" or r["section"] == "bss": continue
    va, n = int(r["va"], 16), int(r["size"])
    a, b = orig(va, n), new(va + D[r["section"]], n)
    nb += n
    for i in range(n):
        if a[i] != b[i] and va + i not in pw: diff += 1
print("bytes compared: %d, differences: %d" % (nb, diff))
# bss: all zero in the new exe
bz = sum(1 for r in rows if r["section"] == "bss" and r["kind"] != "lib" and any(new(int(r["va"], 16) + D["bss"], int(r["size"]))))
print("bss items not zero:", bz)
tot = bad + missing + ab + pm + diff + bz + cov
print("MAPCHECK", "OK" if tot == 0 else "FAIL (%d)" % tot)
sys.exit(1 if tot else 0)
