#!/usr/bin/env python3
"""Brief G: split the code of the image object (mkobj.py output of a GEN_DATA/--drop-app link) into one COFF object per layer.
Usage: mkrange.py <mfl.obj> <out dir> [--rewrite]
Writes <out>/<layer>.obj (RenderWare, qhull, rwp, zlib_png_tiff, D3DX, CRT, Lua, DirectShow, other, app) and <out>/rest.obj (everything of mfl.obj that is not code:
_rwdseg, .rdata filler, weak externals).  Code is cut into BLOCKS = maximal address runs of one layer (tools/delink/layers.py, the metric's classification); every block is
a section `.text$NNNNN` (NNNNN = address order; link.exe sorts the contributions of one output section by the suffix, so the pinned layout stays) with a 1 byte alignment
(the first block keeps the page alignment) in the object of its layer, and an external symbol `__rng_<va>` at its start.  `_rwcseg` (the 0x451 byte RW tail segment) is one block.
Everything that pointed into code now points at `__rng_<va of block>` with the offset as addend: DIR32 words (code, data), REL32 redirect/cross-section branches.  Every other rel32
branch (call/jmp/jcc: Ghidra flow refs, plus the E8/E9/0F8x scan of the code Ghidra gave no sized function) that crosses a block gets a REL32 as well, so a block can move or be
swapped for another implementation of the same layer without patching bytes.  Short branches across blocks cannot be relocated: listed in <out>/short_branches.txt.
The linked exe must be byte identical in .text to the one made from mfl.obj itself (check_range.sh); our own objects must not use a plain `.text` section name that would sort in
front of `.text$NNNNN` (link_ours.sh renames it to `.text$zz`)."""
import bisect, collections, csv, os, re, struct, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import layers as L

if sys.argv[1] == "--rename-text":      # mkrange.py --rename-text <objs...>: section `.text` -> `.text$zz` in the section headers (8 bytes, in place; objcopy would also change debug section flags)
    for p in sys.argv[2:]:
        b = bytearray(open(p, "rb").read())
        for i in range(struct.unpack_from("<H", b, 2)[0]):
            if b[20 + 40 * i:28 + 40 * i] == b".text\0\0\0": b[20 + 40 * i:28 + 40 * i] = b".text$zz"
        sp, sn = struct.unpack_from("<II", b, 8); k = 0
        while k < sn:                      # the section symbol (and COMDAT symbol) of the section has the section's name
            o = sp + 18 * k
            if b[o:o + 8] == b".text\0\0\0": b[o:o + 8] = b".text$zz"
            k += 1 + b[o + 17]
        open(p, "wb").write(b)
    sys.exit(0)
inp, outd = sys.argv[1], sys.argv[2]
d = open(inp, "rb").read()
ns, symptr, nsyms = struct.unpack_from("<H", d, 2)[0], *struct.unpack_from("<II", d, 8)
strt = symptr + 18 * nsyms
def sname(b):
    if b[:4] == b"\0\0\0\0":
        o = strt + struct.unpack_from("<I", b, 4)[0]; return d[o:d.index(b"\0", o)].decode()
    return b.rstrip(b"\0").decode()
secs = []
for i in range(ns):
    o = 20 + 40 * i
    nm = d[o:o + 8]; vs, va, rs, rp, relp, lnp, nr, nl, fl = struct.unpack_from("<IIIIIIHHI", d, o + 8)
    nm = sname(nm) if not nm.startswith(b"/") else None
    rels = []
    if fl & 0x01000000 and nr == 0xFFFF:
        nr = struct.unpack_from("<I", d, relp)[0] - 1; relp += 10
    for k in range(nr): rels.append(struct.unpack_from("<IIH", d, relp + 10 * k))
    secs.append(dict(name=nm, size=rs if rp else vs, data=bytearray(d[rp:rp + rs]) if rp else None, flags=fl, rels=rels, vs=vs))
syms = []              # raw index -> dict
i = 0
while i < nsyms:
    o = symptr + 18 * i
    nm = sname(d[o:o + 8]); val, sec, typ, cls, aux = struct.unpack_from("<IhHBB", d, o + 8)
    syms.append(dict(name=nm, val=val, sec=sec, typ=typ, cls=cls, aux=d[o + 18:o + 18 + 18 * aux], idx=i))
    for k in range(aux): syms.append(None)
    i += 1 + aux
byname = {s["name"]: s for s in syms if s and s["cls"] in (2, 105) and s["name"]}

# ---- address model: .text at 0x401000, _rwcseg at 0x5dc000 ----
CODE = {}
for si, s in enumerate(secs):
    if s["name"] == ".text": CODE[si] = L.TEXT; assert s["size"] == L.TSZ, hex(s["size"])
    elif s["name"] == "_rwcseg": CODE[si] = L.RWC; assert s["size"] == L.RSZ
assert len(CODE) == 2, "need .text and _rwcseg (a mkobj --drop-app object)"
runs = []       # (lo, hi, layer, code section index)
# A2 bisection: CUTS=0x4a0000,0x4a8000 splits a layer run at these addresses (function starts), LAYOUT=endva:lo-hi,lo-hi (hex VAs) moves only the runs that start inside a range behind all others
_cuts = sorted(int(x, 16) for x in os.environ.get("CUTS", "").split(",") if x)
for si, base in CODE.items():
    n = secs[si]["size"]; va = base
    while va < base + n:
        it = L.item_at(va)
        if it is None:
            nxt = min([x[0] for x in L.items if x[0] > va] + [base + n]); hi = min(nxt, base + n); lay = "other"
        else:
            hi = min(it[1], base + n); lay = L.layer(va, it[3], it[4])
        hi = min([hi] + [c for c in _cuts if c > va])
        if runs and runs[-1][2] == lay and runs[-1][1] == va and runs[-1][3] == si and va not in _cuts: runs[-1] = (runs[-1][0], hi, lay, si)
        else: runs.append((va, hi, lay, si))
        va = hi
blk_lo = [r[0] for r in runs]
def blk(va):
    i = bisect.bisect_right(blk_lo, va) - 1
    return i
def bname(i): return "__rng_%08x" % runs[i][0]

# ---- block order in the output section (T-E6): default = address order; LAYOUT="L1,L2,..." (names of tools/delink/layers.py LAYERS, layer-grouped, listed layers first in that
# order, the others behind in the default order) or LAYOUT=rev (layers in reverse order of first appearance) puts whole layers behind each other, so the two-layout test can move/swap them ----
_ENDVA = [tuple(int(y, 16) for y in x.split("-")) for x in os.environ["LAYOUT"][6:].split(",")] if os.environ.get("LAYOUT", "").startswith("endva:") else None
_lay_order = [x for x in os.environ.get("LAYOUT", "").split(",") if x and not x.startswith("end:") and not _ENDVA]
if _lay_order == ["rev"]: _lay_order = list(reversed(list(dict.fromkeys(r[2] for r in runs if secs[r[3]]["name"] != "_rwcseg"))))
_END = os.environ.get("LAYOUT", "").startswith("end:")      # LAYOUT=end:L1,L2: only these layers move behind all the others (everything else keeps its address order)
if _END: _lay_order = []; _endset = os.environ["LAYOUT"][4:].split(",")
_rank = (lambda bi: int(any(a <= runs[bi][0] < b for a, b in _ENDVA))) if _ENDVA else (lambda bi: int(runs[bi][2] in _endset)) if _END else lambda bi: (_lay_order.index(runs[bi][2]) if runs[bi][2] in _lay_order else len(_lay_order)) if _lay_order else 0
_ord = sorted((bi for bi in range(len(runs)) if secs[runs[bi][3]]["name"] != "_rwcseg"), key=lambda bi: (_rank(bi), bi))
POS = {bi: k for k, bi in enumerate(_ord)}
# ---- output objects ----
class Obj:
    def __init__(s, path): s.path, s.secs, s.sym, s.symi, s.strtab = path, [], bytearray(), {}, bytearray(4)
    def n8(s, nm, sec=False):
        b = nm.encode()
        if len(b) <= 8: return b.ljust(8, b"\0")
        o = len(s.strtab); s.strtab += b + b"\0"
        return (b"/%d" % o).ljust(8, b"\0") if sec else b"\0\0\0\0" + struct.pack("<I", o)
    def defsym(s, nm, secno, val, typ=0x20, cls=2, aux=b""):
        s.symi[nm] = len(s.sym) // 18; s.sym += s.n8(nm) + struct.pack("<IhHBB", val, secno, typ, cls, len(aux) // 18) + aux
    def ref(s, nm, rk=None):
        if nm in s.symi: return s.symi[nm]
        o = byname.get(nm)
        if o is not None and o["cls"] == 105:       # weak external: copy with its (undefined) target
            tgt = syms[struct.unpack_from("<I", o["aux"], 0)[0]]["name"]; ti = s.ref(tgt)
            s.symi[nm] = len(s.sym) // 18; s.sym += s.n8(nm) + struct.pack("<IhHBB", 0, 0, 0, 105, 1) + struct.pack("<II", ti, struct.unpack_from("<I", o["aux"], 4)[0]).ljust(18, b"\0")
            return s.symi[nm]
        s.symi[nm] = len(s.sym) // 18; s.sym += s.n8(nm) + struct.pack("<IhHBB", 0, 0, 0x20, 2, 0); return s.symi[nm]
    def write(s):
        body, shdr, p = bytearray(), bytearray(), 20 + 40 * len(s.secs)
        for sc in s.secs:
            raw = bytes(sc["data"]) if sc["data"] is not None else b""
            rp = p if raw else 0; p += len(raw); body += raw
            rl = b"".join(struct.pack("<IIH", off, si, ty) for off, si, ty in sorted(sc["rels"]))
            nr, fl = len(sc["rels"]), sc["flags"]
            if nr >= 0xFFFF: rl = struct.pack("<IIH", nr + 1, 0, 0) + rl; fl |= 0x01000000; nr = 0xFFFF
            rptr = p if sc["rels"] else 0; p += len(rl); body += rl
            shdr += s.n8(sc["name"], True) + struct.pack("<IIIIIIHHI", 0, 0, sc["size"], rp, rptr, 0, nr, 0, fl)
        struct.pack_into("<I", s.strtab, 0, len(s.strtab))
        open(s.path, "wb").write(struct.pack("<HHIIIHH", 0x14C, len(s.secs), 0, 20 + 40 * len(s.secs) + len(body), len(s.sym) // 18, 0, 0) + shdr + body + s.sym + s.strtab)

os.makedirs(outd, exist_ok=True)
objs = {}
def obj_for(lay):
    if lay not in objs: objs[lay] = Obj(os.path.join(outd, re.sub(r"\W+", "_", lay) + ".obj"))
    return objs[lay]
rest = Obj(os.path.join(outd, "rest.obj"))
# block sections
bsec = []        # block index -> (obj, section number 1-based)
first = True
for bi, (lo, hi, lay, si) in enumerate(runs):
    o = obj_for(lay); osec = secs[si]
    if osec["name"] == "_rwcseg": nm, fl = "_rwcseg", osec["flags"]
    else:
        nm = ".text$%05d" % POS[bi]; fl = (osec["flags"] & ~0x00F00000) | 0x00100000        # 1 byte alignment ...
        if POS[bi] == 0: fl = (fl & ~0x00F00000) | 0x00D00000                               # ... except the first block, page aligned like .text
    b0 = lo - CODE[si]
    o.secs.append(dict(name=nm, size=hi - lo, data=osec["data"][b0:hi - CODE[si]], flags=fl, rels=[]))
    bsec.append((o, len(o.secs)))
    o.defsym(bname(bi), len(o.secs), 0, 0x20)
# remainder: non-code sections
remap = {}
for si, s in enumerate(secs):
    if si in CODE: continue
    rest.secs.append(dict(name=s["name"], size=s["size"], data=s["data"], flags=s["flags"], rels=[])); remap[si] = len(rest.secs)
    rest.defsym(s["name"], len(rest.secs), 0, 0, 3, b"\0" * 18)
    rest.symi[("sec", si)] = rest.symi.pop(s["name"])
    rest.defsym("__rngsec_" + s["name"], len(rest.secs), 0, 0x20)       # code (other objects) reaches this section through an external alias
# ---- relocations ----
stat = collections.Counter()
covered = collections.defaultdict(set)         # block -> offsets covered by an existing reloc (4 bytes)
def retarget(site_obj_sec, off, symidx, ty, site_va, blkdata):
    """append relocation; returns nothing. site_obj_sec: section dict of the new object; blkdata: bytearray to patch addend into"""
    sm = syms[symidx]
    if sm["cls"] == 3 and sm["sec"] - 1 in CODE:
        c = sm["sec"] - 1; A = struct.unpack_from("<i", blkdata, off)[0]; t = CODE[c] + A
        tb = blk(t)
        if not runs[tb][0] <= t <= runs[tb][1]: raise SystemExit("reloc target %x outside blocks" % t)
        struct.pack_into("<i", blkdata, off, t - runs[tb][0]); return ("rng", bname(tb)), t
    if sm["cls"] == 3: return ("sec", sm["sec"] - 1), None
    return ("sym", sm["name"]), None
def sym_index(o, key):
    if key[0] == "rng": return o.ref(key[1])
    if key[0] == "sym": return o.ref(key[1])
    if key[0] == "sec":
        return o.symi[key] if key in o.symi else o.ref("__rngsec_" + secs[key[1]]["name"])
for si, s in enumerate(secs):
    for off, symidx, ty in s["rels"]:
        assert ty in (6, 7, 0x14), ty
        if si in CODE:
            site = CODE[si] + off; bi = blk(site); o, sn = bsec[bi]; sc = o.secs[sn - 1]; boff = site - runs[bi][0]
            key, t = retarget(sc, boff, symidx, ty, site, sc["data"])
            sc["rels"].append((boff, sym_index(o, key), ty)); covered[bi].update(range(boff - 3, boff + 4))
        else:
            sc = rest.secs[remap[si] - 1]
            key, t = retarget(sc, off, symidx, ty, None, sc["data"])
            sc["rels"].append((off, sym_index(rest, key), ty))
        stat["relocations"] += 1
# ---- external symbols ----
for sm in syms:
    if not sm or sm["cls"] != 2 or sm["sec"] <= 0: continue
    c = sm["sec"] - 1
    if c in CODE:
        va = CODE[c] + sm["val"]; bi = blk(va); o, sn = bsec[bi]
        if va == runs[bi][1] and bi + 1 < len(runs) and runs[bi + 1][3] == c: bi += 1; o, sn = bsec[bi]       # a symbol at the end of a block belongs to the next
        o.defsym(sm["name"], sn, va - runs[bi][0], sm["typ"]); stat["code symbols"] += 1
    else:
        rest.defsym(sm["name"], remap[c], sm["val"], sm["typ"])
for sm in syms:
    if sm and sm["cls"] == 105: rest.ref(sm["name"])
# ---- branches that leave a block ----
fs = sorted((L.H(l.split()[0]), int(l.split()[2])) for l in open(os.path.join(L.G, "output", "fnsizes.txt")))
fva = [x[0] for x in fs]
def in_sized(v):
    j = bisect.bisect_right(fva, v) - 1
    return j >= 0 and v < fs[j][0] + fs[j][1]
def code_at(va):      # current bytes (filled / patched) of a code address
    for c, base in CODE.items():
        if base <= va < base + secs[c]["size"]: return c, va - base
    return None, None
flow = set()
for r in csv.reader(open(os.path.join(L.G, "output", "delink", "ghidra_refs.csv"))):
    if len(r) >= 3 and r[2] in ("UNCONDITIONAL_CALL", "UNCONDITIONAL_JUMP", "CONDITIONAL_JUMP"):
        try: flow.add((int(r[0], 16), int(r[1], 16)))
        except ValueError: pass
cand = set(flow)
for l in open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "extra_branches.txt")):      # call sites Ghidra has no flow ref for
    if l.strip() and not l.startswith("#"): cand.add((L.H(l.split()[0]), L.H(l.split()[1])))
for c, base in CODE.items():       # byte scan of code outside every sized function (undecoded gaps); inside sized bodies only Ghidra's flow refs count (immediates look like E8 xx xx xx xx)
    b = secs[c]["data"]
    for i in range(len(b) - 6):
        k = 1 if b[i] in (0xE8, 0xE9) else 2 if b[i] == 0x0F and 0x80 <= b[i + 1] <= 0x8F else 0
        if k and not in_sized(base + i): cand.add((base + i, (base + i + k + 4 + struct.unpack_from("<i", b, i + k)[0]) & 0xFFFFFFFF))
NONBR = {int(l.split()[0], 16) for l in open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "nonbranch_sites.txt")) if l.strip() and not l.startswith("#")}      # byte patterns that only look like a branch
short = []
for site, t in sorted(cand):
    if site in NONBR: continue
    c, off = code_at(site)
    if c is None or code_at(t)[0] is None: continue
    bs, bt = blk(site), blk(t)
    if bs == bt: continue
    b = secs[c]["data"]
    if b[off] in (0xE8, 0xE9): fo = off + 1
    elif b[off] == 0x0F and 0x80 <= b[off + 1] <= 0x8F: fo = off + 2
    else:
        if b[off] != 0xCC: short.append((site, t))
        continue
    if (CODE[c] + fo + 4 + struct.unpack_from("<i", b, fo)[0]) & 0xFFFFFFFF != t: stat["branch decode mismatch (patched / data)"] += 1; continue
    o, sn = bsec[bs]; sc = o.secs[sn - 1]; boff = fo - (runs[bs][0] - CODE[c])
    if boff in covered[bs]: stat["branch already relocated"] += 1; continue
    struct.pack_into("<i", sc["data"], boff, t - runs[bt][0]); sc["rels"].append((boff, o.ref(bname(bt)), 0x14)); covered[bs].update(range(boff, boff + 4)); stat["rel32 branches across blocks"] += 1
open(os.path.join(outd, "short_branches.txt"), "w").write("".join("%08x -> %08x\n" % x for x in short))
stat["short branches across blocks (not relocatable)"] = len(short)
for o in list(objs.values()) + [rest]: o.write()
print("mkrange: %d blocks in %d layer objects (+ rest.obj)" % (len(runs), len(objs)))
per = collections.Counter(); sz = collections.Counter()
for lo, hi, lay, si in runs: per[lay] += 1; sz[lay] += hi - lo
for lay in sorted(per): print("  %-14s %4d blocks %8d bytes -> %s" % (lay, per[lay], sz[lay], os.path.basename(objs[lay].path)))
for k, v in sorted(stat.items()): print("  %-48s %d" % (k, v))
