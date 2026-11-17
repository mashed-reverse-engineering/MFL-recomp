#!/usr/bin/env python3
"""Globals generator (plan_standalone_exe.md S4, design: output/design_globals_generator.md).
Emits the game's app data (.rdata/.data/.bss minus library/import/CRT-table spans) as C definitions in PINNED layout: one TU per section, address
order, extent = gap to the next name.  Library spans are emitted as bytes plus pointers (g_libspan_<va>, variant L: the generator owns the whole data layout); linker generated PE spans are zero fillers (g_pespan_<va>).
Usage: globgen.py <outdir>      Output: rdata.c data.c bss.c globals_layout.csv aliases_gen.txt ptrs.csv raw.csv low_pointers.csv objcopy_<sec>.txt stubs.c globgen_report.txt"""
import bisect, collections, csv, os, re, struct, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import cdecls
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "delink"))
import layers      # W11 Z1: ownership item + layer of a .bss address (the split origbytes.py uses)

G = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
BASE = 0x400000
EXE = os.path.join(G, "game", "MFL.exe")
d = open(EXE, "rb").read()
pe = struct.unpack_from("<I", d, 0x3c)[0]
nsec = struct.unpack_from("<H", d, pe + 6)[0]
SECS = {}      # name -> (va, vsize, raw bytes)
for i in range(nsec):
    o = pe + 24 + struct.unpack_from("<H", d, pe + 20)[0] + 40 * i
    nm = d[o:o + 8].rstrip(b"\0").decode()
    vs, va, rs, rp = struct.unpack_from("<IIII", d, o + 8)
    SECS[nm] = (BASE + va, vs, d[rp:rp + rs])


def rd(va, n):
    for nm, (a, vs, raw) in SECS.items():
        if a <= va < a + vs:
            o = va - a
            return (raw[o:o + n] + bytes(max(0, n - max(0, len(raw) - o))))[:n]
    raise KeyError(hex(va))


def u32(va): return struct.unpack("<I", rd(va, 4))[0]


# --- sections of the generator -----------------------------------------------------------------------------------------------------------
SECT = {"rdata": (0x5dd000, 0x5dd000 + 0x1d5c8, ".rdata"), "data": (0x5fb000, 0x64d000, ".data"), "bss": (0x64d000, 0x949d64, ".bss")}
# W12 Z2: the CRT initialiser tables (.CRT$XCA..XTZ of the original link: __xc_a .. __xt_z, walked by _cinit / doexit through those names) hold only
# function pointers and zero markers: no library bytes, emitted as _PVFV arrays named by their markers (the PTR_* labels inside stay aliases).
# Z2T_PUTBACK=1 (zneg.sh crtinit): the bad case, the table stays a library blob.
CRT_TABLES = (0x5fb000, 0x5fb074)
CRT_MARK = re.compile(r"__x[cipt]_[az]$")


def pe_spans():
    """rdata spans derived from the PE: debug directory + raw data, import directory, thunk tables, DLL names, hint/name entries."""
    oh = pe + 24
    dd = lambda i: struct.unpack_from("<II", d, oh + 96 + 8 * i)
    out = []
    r, s = dd(6)
    if r:
        out.append((BASE + r, BASE + r + s))
        for k in range(s // 28):
            e = BASE + r + 28 * k
            sz, raw = u32(e + 16), u32(e + 20)
            if raw: out.append((BASE + raw, BASE + raw + sz))
    r, s = dd(12)
    out.append((BASE + r, BASE + r + s))                   # IAT
    r, s = dd(1)
    imp = BASE + r
    out.append((imp, imp + s))
    while u32(imp + 12):
        oft, nm, ft = u32(imp), u32(imp + 12), u32(imp + 16)
        n = BASE + nm
        ln = rd(n, 64).index(0) + 1
        out.append((n, n + ln + (ln & 1)))
        t = BASE + (oft or ft)
        start = t
        while u32(t):
            v = u32(t)
            if not v & 0x80000000:
                h = BASE + v
                l = rd(h + 2, 256).index(0) + 1
                out.append((h, h + 2 + l + ((2 + l) & 1)))
            t += 4
        out.append((start, t + 4))
        imp += 20
    return out


def merge(rs):
    rs = sorted(rs)
    o = []
    for a, b in rs:
        if o and a <= o[-1][1]: o[-1] = (o[-1][0], max(o[-1][1], b))
        else: o.append((a, b))
    return o


def lib_spans():
    """-> {sec: [(lo, hi, cls)]}; cls 'pe' = linker generated (IAT, import/debug directories): zero filler; cls 'lib' = library bytes, emitted like app data (variant L).
    COMDAT rows owned 'shared' (constants, string pool) are app data and cut out of the library block (F3)."""
    pe = merge(pe_spans())
    pe = merge(pe + [(y, x2) for (x, y), (x2, y2) in zip(pe, pe[1:]) if 0 < x2 - y < 16])    # tiny holes between import tables are DLL name strings of ordinal-only imports
    sp = {"rdata": [(0x5e2670, 0x5f8e30)], "data": [CRT_TABLES, (0x62c5d8, 0x64c274)] if os.environ.get("Z2T_PUTBACK") else [(0x62c5d8, 0x64c274)], "bss": [(0x771ac0, 0x771f78), (0x7d1fa0, 0x7daa1c), (0x7dc298, 0x7dc2c4)]}
    rows, shared = [], []
    for r in csv.DictReader(open(os.path.join(G, "output/delink_sdk/ownership.csv"))):
        lo, hi = int(r["lo"], 16), int(r["hi"], 16)
        if r["kind"] in ("bss", "common") and r["owner"] in ("crt", "rw"): rows.append((lo, hi))
        if r["kind"] in ("rdata", "data") and r["owner"] == "shared": shared.append((lo, hi))
    sp["bss"] += [r for r in rows if r[0] >= 0x64d000]
    st = _selftables()      # B5: a split member's own SEH / jump tables (shared rows pointing into its code) are lib bytes, dead (zero fill) like the rest of its data
    for k_ in ("rdata", "data"): sp[k_] += [(a, b) for a, b in st if SECT[k_][0] <= a < SECT[k_][1]]

    def cut(spans, holes):
        for h0, h1 in holes:
            n = []
            for a, b in spans:
                if h1 <= a or b <= h0: n.append((a, b)); continue
                if a < h0: n.append((a, h0))
                if h1 < b: n.append((h1, b))
            spans = n
        return spans
    out = {}
    for k, v in sp.items():
        v = [(max(a, SECT[k][0]), min(b, SECT[k][1])) for a, b in v if b > SECT[k][0] and a < SECT[k][1]]
        v = cut(merge(v), pe + (cut(merge(shared), st) if k != "bss" else []))
        out[k] = sorted([(a, b, "lib") for a, b in v] + ([(max(a, SECT[k][0]), min(b, SECT[k][1]), "pe") for a, b in pe if b > SECT[k][0] and a < SECT[k][1]] if k == "rdata" else []))
    return out


# --- names ----------------------------------------------------------------------------------------------------------------------------------
# dead lib data (zero fill): the source-built lib has its own data.  (section, lo, hi): qhull 0x63a610-0x649138; zlib/PNG (A5): rtpng.lib's .rdata (png, pngerror, deflate, crc32, trees, infblock, inftrees) and
# .data (pngwrite .. infutil, without the rtpng.obj wrapper data at 0x6373b4); only rows of kind lib inside are zeroed (the libpng strings are app rows)
# W11 Z3 (LIBSRC lua): the Lua 4.0 data the source-built Lua objects bring themselves (relocation targets of Lua code only; qhull's 0x62d994 sites are filled code too):
# .rdata 0x5e9c88-0x5ea034 (lua_ident "$Lua: Lua 4.0 Copyright", type / token / event name tables, luaL_reg lists, lmathlib constants), .data 0x62d894-0x62d914
# (lauxlib / ldo strings) and 0x62d91c-0x62e514 (Lua strings).  Not Lua: the rwFILTER / rpSTRING strings 0x5e9a58-0x5e9c80 (app table 0x62d81c), RenderWare
# 0x5ea038- ("Core built at", hex digits) and 0x62d914-0x62d91c, 0x62e514- (RenderWare / Audio / CRT rows).  Evidence: agents/l3_w11/refs.py, luamap.py, notes "Z3".
# Z3_PUTBACK=1 (zneg.sh lua): the bad case, the Lua rows stay library bytes.
LIBSRC_DEAD_RANGES = [("rdata", 0x5e9c88, 0x5ea034), ("data", 0x62d894, 0x62d914), ("data", 0x62d91c, 0x62e514)] * ("lua" in os.environ.get("LIBSRC", "").split() and not os.environ.get("Z3_PUTBACK")) +[("data", 0x63a610, 0x649138)] * ("qhull" in os.environ.get("LIBSRC", "")) + [("rdata", 0x5f4700, 0x5f4880), ("rdata", 0x5f48b8, 0x5f5a18), ("data", 0x6373bc, 0x63a194)] * ("zlib" in os.environ.get("LIBSRC", "").split())
CRT_SPLIT = os.environ.get("CRT_SPLIT")      # B5 LIB_CRT=libcmt: dir of plan_split.py crt output; the data (rdata / data / bss) of its split members is dead here (libcmt objects bring their own), the rest of the CRT data keeps its bytes and gets the lib's names
def _selftables():
    f = os.path.join(CRT_SPLIT or "", "selftables.txt")
    return [(int(l.split()[0], 16), int(l.split()[1], 16)) for l in open(f) if l.strip()] if CRT_SPLIT and os.path.exists(f) else []
def _crt_dead():
    if not CRT_SPLIT: return []
    import csv as _c
    mem = {l.strip() for l in open(os.path.join(CRT_SPLIT, "members.txt"))}; out = []
    _ci = os.path.join(CRT_SPLIT, "items.csv")      # B7: ownership + plan_split's anchored data rows
    for r in _c.DictReader(open(_ci if os.path.exists(_ci) else os.path.join(G, "output", "delink_sdk", "ownership_items.csv"))):
        if r["member"] in mem and r["kind"] != "code" and r["owner"] == "crt":
            lo, hi = int(r["lo"], 16), int(r["hi"], 16)
            if r["kind"] == "common" and any(rd(lo, hi - lo)): continue      # a COMMON the image holds initialised by an unsplit member (crt0dat's __FPinit = &_fpmath, set up by _cinit): keep bytes + name, the member's zero COMMON loses to it (dead = _fpmath never ran = black screen)
            for k_, (a_, b_, _n) in SECT.items():
                if a_ <= lo < b_: out.append((k_, lo, min(hi, b_)))
    for lo, hi in _selftables():
        for k_, (a_, b_, _n) in SECT.items():
            if a_ <= lo < b_: out.append((k_, lo, min(hi, b_)))
    return out
LIBSRC_DEAD_RANGES = LIBSRC_DEAD_RANGES + _crt_dead()
# W13 Z6 (plan_z6_d3dx_2026-11-17.md section 5): dead D3DX library rows, tools/delink/d3dx_dead.txt "<va> <size> <name> <step>"; zero fill, range
# edges stay item edges.  Z6_PUTBACK=<step>[,<step>] (zneg.sh d3dx1 ..): the bad case, that step's rows stay library bytes.
_Z6P = set(filter(None, os.environ.get("Z6_PUTBACK", "").split(",")))
_d3f = os.path.join(G, "tools", "delink", "d3dx_dead.txt")
for _l in (open(_d3f) if os.path.exists(_d3f) else []):
    _p = _l.split()
    if not _p or _l[0] == "#": continue
    if _p[3] in _Z6P: print("Z6_PUTBACK: %s +%s %s (step %s) stays library bytes" % tuple(_p[:4])); continue
    _a = int(_p[0], 16); LIBSRC_DEAD_RANGES.append((next(k for k, (x, y, _n) in SECT.items() if x <= _a < y), _a, _a + int(_p[1])))
LIBSRC_DEAD = {x for _, a, b in LIBSRC_DEAD_RANGES for x in (a, b)}      # range edges stay item edges
def libsrc_dead(sec, it): return any(s_ == sec and a <= it["va"] and it["va"] + it["ext"] <= b + (3 if sec == "bss" and it.get("folded") else 0) for s_, a, b in LIBSRC_DEAD_RANGES)      # Z1b: a dead .bss row keeps the <4 B alignment padding folded behind it (stdargv, mbctype: 261 + 3 B)


# W11 Z1: the library .bss rows of these layers are not byte blobs (g_libspan_<va>) but named zero definitions, kind `libz` (no image content):
# one definition per name (names.get), the unnamed bytes behind a name belong to it up to the next name or the end of the owning object's .bss;
# an unnamed head is named after the owning object (<object>_bss[_<hex offset in it>]).  Types: tools/globals/z1_types.txt, else a sized array /
# scalar under the name (listed in z1_rows.csv).  Every definition carries a size assert.  Z1_PUTBACK=1 (zneg.sh bss): the first such row stays a blob.
Z1_LAYERS = None      # Z1b: every layer (Z1a was {"RenderWare"}); the bss gap rows too (z1_gapname)
def z1_owner(a):
    """-> (member, member lo) when the .bss byte a belongs to a Z1 layer (origbytes.py's split: owner crt/rw items by layer()), else None;
    ("", None) = no owner item (padding between objects, the CRT span's own bytes)"""
    if any(s_ == "bss" and x <= a < y for s_, x, y in LIBSRC_DEAD_RANGES): return None
    it = layers.item_at(a)
    if it and it[3] in ("crt", "rw", "crt?", "rw?"): return (it[4], it[0]) if Z1_LAYERS is None or layers.layer(a, it[3], it[4]) in Z1_LAYERS else None
    L = "CRT" if 0x771ac0 <= a < 0x771f78 and not it else layers.other_layer(a, "data")
    return ("", None) if Z1_LAYERS is None or L in Z1_LAYERS else None
def z1_gapname(a):
    """an unnamed .bss piece with no owner and no Z1 row in front: <layer>_bss_<va> (no better evidence; listed in z1_rows.csv)"""
    L = "CRT" if 0x771ac0 <= a < 0x771f78 else layers.other_layer(a, "data")
    return re.sub(r"\W", "_", L.lower()) + "_bss_%x" % a
def z1_cuts(a, b):
    """ownership item edges inside [a, b) -> sub-pieces"""
    xs = sorted({a, b} | {x for lo_, hi_, k_, o_, m_ in layers.items if k_ in ("bss", "common") for x in (lo_, hi_) if a < x < b})
    return list(zip(xs, xs[1:]))
def z1_hex(n): return re.search(r"_[0-9a-f]{6,8}$", n) is not None      # a Ghidra / lane label named by its address (g_rw_7d2814)
def z1_name(nm):
    """readable name first, then <prefix>_<hex> names, g_data_<hex> last"""
    return sorted(nm, key=lambda n: (z1_hex(n), n.startswith("g_data_")))[0]
def z1_member_name(member, off):
    base = re.sub(r"\W", "_", member.replace("\\", "/").split("/")[-1].lower().replace(".obj", ""))
    return base + "_bss" + ("_%x" % off if off else "")
Z1_TYPES = {}      # name -> (C type, declarator suffix, size, alignment, evidence); struct definitions in z1_types.h (pasted into bss.c)
_zt = os.path.join(os.path.dirname(os.path.abspath(__file__)), "z1_types.txt")
if os.path.exists(_zt):
    for l in open(_zt):
        if l.strip() and l[0] != "#":
            p = [x.strip() for x in l.split("|")]
            Z1_TYPES[p[0]] = (p[1], p[2], int(p[3]), int(p[4]), p[5])
# W13 Z4: library .rdata / .data rows as named, typed definitions (kind `libt`; the bytes are the image's, the form is typed C) or dead rows
# (type `dead`: zero fill, nothing in the link reads them).  tools/globals/z4_types.txt, one definition per line:
#   <va> | <size> | <name> | <type> | <group> | <evidence>
# type: base [N] | {field; field; ...}[N] | dead;  field: base[N] [name];  base: f4 f8 u1 u2 u4 i4 ptr fptr guid str wstr
# (str / wstr at the top level take the whole size).  The entries must tile whole library items (a head + __t split pair counts as two items).
# Z4_PUTBACK=<group>[,<group>] (zneg.sh audio / rwp / dshow): the bad case, that group's rows stay library blobs.
Z4 = {}      # sec -> {va: entry}
Z4_PUTBACK = set(filter(None, os.environ.get("Z4_PUTBACK", "").split(",")))
Z4_BASE = {"f4": ("float", 4), "f8": ("double", 8), "u1": ("unsigned char", 1), "u2": ("unsigned short", 2), "u4": ("unsigned int", 4),
           "i4": ("int", 4), "ptr": ("void *", 4), "fptr": ("z4_fn", 4), "guid": ("struct mfl_guid", 16), "str": ("char", 1), "wstr": ("unsigned short", 2),
           "vu2": ("unsigned short", 2), "vu4": ("unsigned int", 4)}      # vu2 / vu4: value-only words (a relocation site inside is NOT relocated: the image value)
def z4_field(s, whole=None):
    """'base[N] name' -> (base, n or None, name or None, size)"""
    m = re.fullmatch(r"(\w+)(?:\[(\d+)\])?(?:\s+(\w+))?", s.strip())
    if not m or m.group(1) not in Z4_BASE: raise ValueError("z4 field %r" % s)
    n = int(m.group(2)) if m.group(2) else (whole // Z4_BASE[m.group(1)][1] if whole and m.group(1) in ("str", "wstr") else None)
    return m.group(1), n, m.group(3), Z4_BASE[m.group(1)][1] * (n or 1)
def z4_parse(t, size):
    """-> dict(fields=[...] | None for a plain base, base, n, struct) ; checks the size"""
    t = t.strip()
    if t == "dead": return dict(dead=True)
    m = re.fullmatch(r"\{(.*)\}(?:\[(\d+)\])?", t)
    if m:
        fs = [z4_field(x) for x in m.group(1).split(";") if x.strip()]
        n = int(m.group(2)) if m.group(2) else None
        sz = sum(f[3] for f in fs) * (n or 1)
        r = dict(fields=fs, n=n)
    else:
        b, n, _, sz = z4_field(t, size)
        r = dict(fields=None, base=b, n=n)
    if sz != size: raise ValueError("z4 type %r is %d B, the row %d B" % (t, sz, size))
    return r
_z4f = os.path.join(os.path.dirname(os.path.abspath(__file__)), "z4_types.txt")
if os.path.exists(_z4f):
    for _l in open(_z4f):
        if not _l.strip() or _l[0] == "#": continue
        _p = [x.strip() for x in _l.split("|")]
        _va, _sz = int(_p[0], 16), int(_p[1])
        _sec = next(k for k, (a, b, _n) in SECT.items() if a <= _va < b)
        _e = dict(va=_va, size=_sz, name=_p[2], type=_p[3], group=_p[4], evidence=_p[5] if len(_p) > 5 else "", sec=_sec, **z4_parse(_p[3], _sz))
        if _va in Z4.setdefault(_sec, {}): sys.exit("globgen Z4: %08x twice in z4_types.txt" % _va)
        Z4[_sec][_va] = _e


def load_names():
    names = collections.defaultdict(list)
    seen = {}      # a name used at several addresses (the _imp__ thunk labels) keeps its first one
    for f in ("src/symbols.txt", "src/aliases.txt"):
        for l in open(os.path.join(G, f)):
            p = l.split()
            if len(p) >= 2 and l[0] != "#":
                try: a = int(p[1], 16)
                except ValueError: continue
                if 0x401000 <= a < 0x949d64 and re.fullmatch(r"[A-Za-z_]\w*", p[0]) and p[0] not in names[a] and seen.setdefault(p[0], a) == a: names[a].append(p[0])
    if os.environ.get("LIBSRC"):      # A4: names the source-built libs link against (CRT data, qh_qh): tools/delink_sdk/libsrc_symbols.txt "<C name> <va>"
        for l in open(os.path.join(G, "tools", "delink_sdk", "libsrc_symbols.txt")):
            p = l.split()
            if len(p) >= 2 and l[0] != "#" and p[0] not in names[int(p[1], 16)]: names[int(p[1], 16)].append(p[0])
    if CRT_SPLIT:      # names the split libcmt members link against: located CRT data of the image (ownership.py lib_names), not in a dead range
        import csv as _c
        cr = [(int(r["lo"], 16), int(r["hi"], 16)) for r in _c.DictReader(open(os.path.join(G, "output", "delink_sdk", "ownership_items.csv"))) if r["owner"].startswith("rw") and r["kind"] != "code"]      # lib_names also holds RenderWare names: those would only add item borders (+12 B RenderWare bss)
        for l in open(os.path.join(G, "output", "delink_sdk", "lib_names.txt")):
            p = l.split()
            if len(p) < 2: continue
            a = int(p[1], 16)
            if any(x <= a < y for x, y in cr): continue
            if SECT["rdata"][0] <= a < SECT["bss"][1] and re.fullmatch(r"[A-Za-z_]\w*", p[0]) and not any(s_ <= a < e_ for _, s_, e_ in LIBSRC_DEAD_RANGES) and p[0] not in names[a] and seen.setdefault(p[0], a) == a: names[a].append(p[0])
    if CRT_SPLIT:      # the split members define these symbols themselves (_iob, __d_inf, ...): a name here would win the link and bring the zero-filled dead row
        _cd = list(_crt_dead())      # once: calling _crt_dead() per name re-read the CRT csv ~60k times (15 min of every libcmt link, found 2026-11-03)
        for a in [a for a in names if any(s_ <= a < e_ for k_, s_, e_ in _cd)]: names[a] = []
    return {a: v for a, v in names.items() if v}


def used_idents():
    s = set()
    for root, _, fs in os.walk(os.path.join(G, "src")):
        for f in fs:
            if f.endswith(".c") or f.endswith(".h"):
                s.update(re.findall(r"[A-Za-z_]\w*", open(os.path.join(root, f), errors="replace").read()))
    return s


ALLOW = os.path.join(G, "src", "globals_reloc_allow.txt")      # reviewed low-confidence relocation sites (first token = site, hex)


def c_convs():
    """F1: name -> (conv, [param sizes]) for every __stdcall/__fastcall function our C declares or defines (decorated link name _f@N / @f@N)."""
    def split(s):
        out, d, cur = [], 0, ""
        for c in s:
            d += (c == "(") - (c == ")")
            if c == "," and d == 0: out.append(cur); cur = ""
            else: cur += c
        if cur.strip(): out.append(cur)
        return [p.strip() for p in out]
    sz = lambda p: 4 if "*" in p or "(" in p else 8 if re.search(r"\b(double|__int64)\b|long long", p) else 4
    res = {}
    for root, _, fs in os.walk(os.path.join(G, "src")):
        for f in fs:
            if not f.endswith((".c", ".h")): continue
            t = open(os.path.join(root, f), errors="replace").read()
            for m in re.finditer(r"\b(__stdcall|__fastcall)\s+(\w+)\s*\(", t):
                i = j = m.end(); dd = 1
                while dd and j < len(t): dd += (t[j] == "(") - (t[j] == ")"); j += 1
                ps = split(t[i:j - 1])
                if ps == ["void"]: ps = []
                res.setdefault(m.group(2), set()).add((m.group(1), tuple(sz(p) for p in ps)))
    return {k: sorted(v)[0] for k, v in res.items()}, {k for k, v in res.items() if len(v) > 1}


def c_defined():
    """names of functions our C has a definition (with body) for"""
    pat = re.compile(r"^[A-Za-z_][\w \t*]*?\b(\w+)\s*\([^;{}]*\)\s*\{", re.M)
    s = set()
    for root, _, fs in os.walk(os.path.join(G, "src")):
        for f in fs:
            if f.endswith(".c"): s.update(pat.findall(open(os.path.join(root, f), errors="replace").read()))
    return s


def linkname(nm, conv):
    if not conv: return "_" + nm
    return ("_%s@%d" if conv[0] == "__stdcall" else "@%s@%d") % (nm, sum(conv[1]))


def main(out):
    os.makedirs(out, exist_ok=True)
    spans = lib_spans()
    names = load_names()
    used = used_idents()
    CONV, conv_conflict = c_convs()
    cdefs = c_defined()
    conv_decl_only = sorted(n for n in CONV if n not in cdefs)      # declared __stdcall but not defined by our C: the image object's undecorated legacy symbol is right
    CONV = {n: v for n, v in CONV.items() if n in cdefs}
    cdec = cdecls.scan({n for v in names.values() for n in v if n in used})        # T2: declared C types of the names
    allow = set()
    if os.path.exists(ALLOW):
        for l in open(ALLOW):
            p = l.split()
            if p and l[0] != "#": allow.add(int(p[0], 16))
    # T-E6: value-only words that hold a real code address (hand classified, evidence in the file): they become `symbol + offset` pointers
    CODEW = os.path.join(G, "src", "globals_code_words.txt")
    if os.path.exists(CODEW):
        for l in open(CODEW):
            p = l.split()
            if p and l[0] != "#": allow.add(int(p[0], 16))
    fn = []
    for l in open(os.path.join(G, "output/fnsizes.txt")):
        p = l.split()
        if len(p) >= 3: fn.append((int(p[0], 16), int(p[2]), p[1]))
    fn.sort()
    fn_starts = [x[0] for x in fn]
    relocs = {}
    for r in csv.DictReader(open(os.path.join(G, "output/delink_relocs.csv"))):
        if r["kind"] in ("const", "header", "rel32"): continue
        relocs[int(r["site"], 16)] = (int(r["target"], 16), r["kind"], r["confidence"], r["why"])
    for env_, f_ in (("DROP_DSHOW", "dshow_dead.txt"), ("DROP_RW", "rw_dead.txt")):      # A7 / S3: pointers into dead code (DirectShow movie code, RenderWare/D3DX pool) become 0 (vtables, EH tables, XCU entries; _initterm skips NULL).  DROP_DSHOW is set by link_ours.sh in every link (W12 DS2); globals/check.sh leaves it unset (no dead-code model)
        if not os.environ.get(env_): continue
        dead = sorted((int(l.split()[0], 16), int(l.split()[0], 16) + int(l.split()[1])) for l in open(os.path.join(G, "tools/delink", f_)) if l.strip() and not l.startswith("#"))
        dst = [x[0] for x in dead]; nz = 0
        for s_ in sorted(relocs):
            t_ = relocs[s_][0]; j_ = bisect.bisect_right(dst, t_) - 1
            if j_ >= 0 and t_ < dead[j_][1] and SECT["rdata"][0] <= s_ < SECT["bss"][1]:
                for nm_, (a_, vs_, raw_) in SECS.items():
                    if a_ <= s_ < a_ + vs_:
                        raw_ = bytearray(raw_); raw_[s_ - a_:s_ - a_ + 4] = bytes(4); SECS[nm_] = (a_, vs_, bytes(raw_)); break
                del relocs[s_]; nz += 1
        print("%s: %d pointers into dead code zeroed" % (env_, nz))

    # items per section
    items = {}      # sec -> list of dict(va, ext, names, kind app/gap/lib/pe, ...)
    carved = 0
    for sec, (lo, hi, _) in SECT.items():
        sp = spans[sec]
        bounds = {lo, hi} | {x for a, b, c in sp for x in (a, b)} | {a for a in names if lo <= a < hi and (os.environ.get("Z2T_PUTBACK") or not CRT_TABLES[0] <= a < CRT_TABLES[1] or any(CRT_MARK.match(n) for n in names[a]))} | {x for x in LIBSRC_DEAD | set(CRT_TABLES) if lo <= x < hi}
        bs = sorted(bounds)
        its = []
        for a0, b0 in zip(bs, bs[1:]):
            cls0 = next((c for x, y, c in sp if x <= a0 < y), None)
            nm0 = names.get(a0, [])
            if cls0 == "lib" and any(n in used for n in nm0): cls0 = None; carved += 1      # F3: a name our C uses is app data, also inside a library span
            for a, b in (z1_cuts(a0, b0) if cls0 == "lib" and sec == "bss" else [(a0, b0)]):      # Z1: a .bss library piece is cut at its owners' edges
                cls, nm = cls0, (nm0 if a == a0 else [])
                own = z1_owner(a) if cls == "lib" and sec == "bss" else None
                if own and own[0] == "" and Z1_LAYERS is not None and (nm or not (its and its[-1]["kind"] == "libz" and its[-1]["va"] + its[-1]["ext"] == a)): own = None      # Z1a: padding joins only an unnamed tail of a Z1 row
                if not own and cls is None and not nm and sec == "bss" and Z1_LAYERS is None: own = ("", None)      # Z1b: a bss gap row (no name, no owner) is a Z1 row too
                if own:      # Z1: named zero definition
                    p_ = its[-1] if its and its[-1]["kind"] == "libz" and its[-1]["va"] + its[-1]["ext"] == a else None
                    if nm and p_ and p_["readable"] and p_["member"] == own[0] and all(z1_hex(n) for n in nm) and a < p_["va"] + Z1_TYPES.get(p_["name"], ("", "", 1 << 30))[2]:
                        p_["ext"] += b - a; continue      # a <prefix>_<hex> access label inside a readably named object (up to its z1_types size): interior label (alias)
                    if nm: its.append(dict(va=a, ext=b - a, kind="libz", names=nm, name=z1_name(nm), member=own[0], mlo=own[1], readable=not z1_hex(z1_name(nm)))); continue
                    if its and its[-1]["kind"] == "libz" and its[-1]["va"] + its[-1]["ext"] == a and own[0] in (its[-1]["member"], ""): its[-1]["ext"] += b - a; continue
                    its.append(dict(va=a, ext=b - a, kind="libz", names=[], name=z1_member_name(own[0], a - own[1]) if own[0] else z1_gapname(a), member=own[0], mlo=own[1], readable=False)); continue
                if cls:
                    if its and its[-1]["kind"] == cls and its[-1]["va"] + its[-1]["ext"] == a and a not in LIBSRC_DEAD:      # LIBSRC_DEAD: the edges of the dead qhull data stay item edges
                        its[-1]["ext"] += b - a
                        continue
                    its.append(dict(va=a, ext=b - a, kind=cls, names=[], name=("g_libspan_%08x" if cls == "lib" else "g_pespan_%08x") % a))
                    continue
                prim = next((n for n in nm if n in used), nm[0] if nm else None)
                its.append(dict(va=a, ext=b - a, kind="app" if nm else "gap", names=nm, name=prim or "g_gap_%08x" % a))
        items[sec] = its
    if os.environ.get("Z1_PUTBACK"):      # zneg.sh bss: the bad case, chosen at run time (the first Z1 row)
        z = next((it for it in items["bss"] if it["kind"] == "libz"), None)
        if z: z.update(kind="lib", names=[], name="g_libspan_%08x" % z["va"]); print("Z1_PUTBACK: %08x +%d stays a library blob" % (z["va"], z["ext"]))
    FILL = ("lib", "pe")

    # alignment fold: interior labels
    misaligned = split = splitbad = absorbed = 0
    absmix = collections.Counter()      # bytes of one class folded into an item of another class (alignment limit of cl: strings cannot start at odd addresses)
    pe_nonzero = []         # non-zero bytes absorbed into a zero pe filler (must stay empty)
    for sec, its in items.items():
        res = []
        pre = []
        for it in its:       # a library item that starts at an unaligned address gives only its head (up to the next 4-boundary) to the fold below, the rest stays a library item
            if it["kind"] in ("lib", "libz") and it["va"] % 4 and it["ext"] > 4:      # Z1: a libz item the same way, its rest is named after the owning object
                h = 4 - it["va"] % 4
                pre += [dict(it, ext=h), dict(it, va=it["va"] + h, ext=it["ext"] - h, names=[], name="g_libspan_%08x" % (it["va"] + h) if it["kind"] == "lib" else z1_member_name(it["member"], it["va"] + h - it["mlo"]) if it["member"] else z1_gapname(it["va"] + h))]
            else: pre.append(it)
        for it in pre:
            if it["kind"] == "pe": res.append(it); continue
            a, e = it["va"], it["ext"]
            ok = e == 1 or a % 4 == 0           # VC7: a short is 4-aligned too (probed), only char has alignment 1
            if not ok and e == 2:               # two chars (name, name__1) keep the pinned layout without a parent
                res.append(dict(it, ext=1))
                res.append(dict(it, va=a + 1, ext=1, names=[], name=it["name"] + "__1", kind="gap" if it["kind"] == "gap" else it["kind"]))
                continue
            if not ok:
                # merge back to the nearest preceding item with a 4-aligned start (all items between become interior labels), or into a lib filler
                j = len(res) - 1
                while j >= 0 and res[j]["kind"] != "pe" and res[j]["va"] % 4 != 0: j -= 1
                if j >= 0:
                    par = res[j]
                    for r in res[j + 1:] + [it]:
                        par.setdefault("folded", []).append(r["va"])
                        par["ext"] += r["ext"]
                        if r["kind"] != par["kind"]: absorbed += r["ext"]; absmix[r["kind"] + " into " + par["kind"]] += r["ext"]
                        if par["kind"] == "pe" and any(rd(r["va"], r["ext"])): pe_nonzero.append(r["va"])
                    del res[j + 1:]
                    continue
                misaligned += 1
                it["misaligned"] = True
            res.append(it)
        # VC7: arrays/aggregates >= 64 bytes are 8-aligned relative to the section start (probed): at rel offset 4 mod 8 the item is split into
        # a 4 byte head (keeps the name) and a tail <name>__t at the 8-aligned address
        out2 = []
        for it in res:
            if it["ext"] >= 64 and (it["va"] - SECT[sec][0]) % 8 == 4:
                a = it["va"]
                if any(a < x < a + 4 for x in relocs):
                    splitbad += 1
                else:
                    split += 1
                    out2.append(dict(it, ext=4))
                    out2.append(dict(it, va=a + 4, ext=it["ext"] - 4, names=[], name=it["name"] + "__t", folded=[]))
                    continue
            out2.append(it)
        items[sec] = out2
    # W13 Z4: library items replaced by the typed / dead definitions of z4_types.txt (entries must tile whole library items)
    z4used, z4n = set(), collections.Counter()
    byname = {n: a for a, ns in names.items() for n in ns}
    for sec, its in items.items():
        ents = Z4.get(sec, {})
        if not ents: continue
        out3, i = [], 0
        while i < len(its):
            it = its[i]
            if it["kind"] != "lib" or it["va"] not in ents or ents[it["va"]]["group"] in Z4_PUTBACK:
                out3.append(it); i += 1; continue
            pos, end = it["va"], it["va"] + it["ext"]; i += 1; new = []; rest = None
            while True:
                en = ents.get(pos)
                if new and en is None and pos % 4 == 0 and not any(pos < x < end for x in ents):      # the entries end inside a longer library item (check build without the CRT split): the rest stays a library blob
                    rest = dict(it, va=pos, ext=end - pos, names=[], name="g_libspan_%08x" % pos, folded=[]); break
                if en is None or en["group"] in Z4_PUTBACK: sys.exit("globgen Z4: no entry at %08x inside the library item run from %08x (entries must tile it)" % (pos, it["va"]))
                new.append(en); pos += en["size"]
                while pos > end:      # the entry runs on into the next item: a library item that starts there
                    if i >= len(its) or its[i]["va"] != end or its[i]["kind"] != "lib": sys.exit("globgen Z4: entry %08x +%d runs past the library items (to %08x)" % (en["va"], en["size"], end))
                    end += its[i]["ext"]; i += 1
                if pos == end: break
            for en in new:
                if en["name"] in byname and byname[en["name"]] != en["va"]: sys.exit("globgen Z4: name %s is a label at %08x, the entry is at %08x" % (en["name"], byname[en["name"]], en["va"]))
                if not en.get("dead") and ((en["size"] >= 64 and (en["va"] - SECT[sec][0]) % 8) or (en.get("base") == "f8" and en["va"] % 8)):
                    sys.exit("globgen Z4: %08x +%d %s: cl places it 8-aligned (>= 64 B aggregate or double): split it" % (en["va"], en["size"], en["name"]))
                z4used.add((sec, en["va"])); z4n["dead" if en.get("dead") else "libt"] += en["size"]
                out3.append(dict(va=en["va"], ext=en["size"], kind="lib" if en.get("dead") else "libt", z4dead=bool(en.get("dead")), names=[], name=en["name"], z4=en, folded=[]))
            if rest: out3.append(rest)
        items[sec] = out3
    z4miss = [(s, a) for s, e in Z4.items() for a in e if (s, a) not in z4used and e[a]["group"] not in Z4_PUTBACK]
    if z4miss: sys.exit("globgen Z4: %d entries of z4_types.txt match no library item: %s" % (len(z4miss), " ".join("%08x" % a for s, a in z4miss[:8])))
    for g in sorted(Z4_PUTBACK): print("Z4_PUTBACK: group %s: %s stay library blobs" % (g, " ".join("%08x+%d" % (a, e[a]["size"]) for s, e in Z4.items() for a in sorted(e) if e[a]["group"] == g)))
    # every name inside an item other than the item's own definition becomes an alias (objcopy): interior labels, second names, library labels
    nm_all = sorted(a for a in names if SECT["rdata"][0] <= a < SECT["bss"][1])
    aliases = []
    nint = nsame = 0
    for sec, its in items.items():
        for it in its:
            if it["kind"] == "pe": continue
            i0 = bisect.bisect_left(nm_all, it["va"])
            while i0 < len(nm_all) and nm_all[i0] < it["va"] + it["ext"]:
                a = nm_all[i0]; i0 += 1
                for n in names[a]:
                    if n == it["name"]: continue
                    aliases.append((n, a - it["va"], it["name"]))
                    if a == it["va"]: nsame += 1
                    else: nint += 1

    # lookup tables
    allapp = [(it["va"], it["ext"], it, sec) for sec, its in items.items() for it in its]
    allapp.sort(key=lambda x: x[0])
    starts = [x[0] for x in allapp]
    secof = {}
    for sec, its in items.items():
        for it in its: secof[id(it)] = sec

    stat = collections.Counter()
    ext_syms = {}         # extern name -> (va of the label)  (functions + pe labels)
    rawsites = {}         # site -> reason: reloc sites that stay raw numbers (F4: nothing is dropped silently)
    emitted = set()
    lowrows = []

    def resolve(t):
        """-> (expr_kind, name, off, item) ; kinds: 'def' (generated), 'ext' (code/lib symbol), 'raw'"""
        i = bisect.bisect_right(starts, t) - 1
        if i >= 0 and SECT["rdata"][0] <= t < SECT["bss"][1]:
            va, ext, it, sec = allapp[i]
            if va <= t < va + ext:
                if it["kind"] not in FILL: return ("def", it["name"], t - va, it)
                j = bisect.bisect_right(nm_all, t) - 1
                if j >= 0 and nm_all[j] >= va and it["kind"] == "lib":
                    a = nm_all[j]; return ("ext", names[a][0], t - a, None)
                if it["kind"] == "lib": return ("def", it["name"], t - va, it)
                if j >= 0 and nm_all[j] >= va:
                    a = nm_all[j]; return ("ext", names[a][0], t - a, None)
                return ("ext", it["name"], t - va, None)
        k = bisect.bisect_right(fn_starts, t) - 1
        if k >= 0 and fn[k][0] <= t < fn[k][0] + max(fn[k][1], 1) and re.fullmatch(r"[A-Za-z_]\w*", fn[k][2]): return ("ext", fn[k][2], t - fn[k][0], None)
        if t in names: return ("ext", names[t][0], 0, None)
        if 0x401000 <= t < 0x5dc451: return ("ext", "func_0x%08x" % t, 0, None)      # T-E6: code address without a symbol (label inside a body): mkobj defines func_0x<va> at that address
        return ("raw", None, 0, None)

    ptrs = []             # site, symbol, link symbol, addend, original value, conf
    def item_ptrs(it, sec, valonly=()):
        a, e = it["va"], it["ext"]
        out = []
        for s in range(a, a + e - 3):
            r = relocs.get(s)
            if r is None: continue
            if s in valonly: stat["Z4 value-only word (not relocated)"] += 1; rawsites[s] = "z4 value-only"; continue      # W13 Z4: a vu2 / vu4 field: the image value, never a pointer
            v = u32(s)
            if v != r[0]: stat["reloc value mismatch"] += 1; rawsites[s] = "value mismatch"; continue
            if any(s < o + 4 and o < s + 4 for o, *_ in out): stat["overlapping reloc"] += 1; rawsites[s] = "overlap"; continue
            k, nm, off, tit = resolve(v)
            if r[2] == "low":
                nb = lambda x: relocs.get(x, (0, 0, "-"))[2]
                lowrows.append((s, it["name"], v, k, nm, off, r[1], r[3], nb(s - 4), nb(s + 4), s in allow))
                if s not in allow: stat["low raw (not allowed)"] += 1; rawsites[s] = "low"; continue
                stat["low allowed"] += 1
            stat["ptr " + k] += 1; stat["ptr conf " + r[2]] += 1
            if k == "raw": rawsites[s] = "unresolved"; continue
            out.append((s, k, nm, off, tit))
            emitted.add(s)
            ptrs.append((s, nm, linkname(nm, CONV.get(nm)) if k == "ext" else "_" + nm, off, v, r[2]))
            if k == "ext": ext_syms[nm] = v - off
        return out

    pos = {it["name"]: i for its in items.values() for i, it in enumerate(its)}
    PRINT = set(range(0x20, 0x7f)) | {10, 13, 9}

    def cstr(b):
        o = []
        for c in b:
            if c == 34: o.append('\\"')
            elif c == 92: o.append("\\\\")
            elif c == 63: o.append("\\?")
            elif c == 10: o.append("\\n")
            elif c == 13: o.append("\\r")
            elif c == 9: o.append("\\t")
            elif 32 <= c < 127: o.append(chr(c))
            else: o.append("\\%03o" % c)
        return '"' + "".join(o) + '"'

    fwd = {}     # forward targets (same TU, defined later): VC7 emits a definition at its FIRST REFERENCE, so pointers go through <name>__fwd, an alias added by objcopy

    def pexpr(sec, p, cur):
        s, k, nm, off, tit = p
        ref = nm
        if k == "def" and secof[id(tit)] == sec:
            if pos[nm] < cur: ref = "&" + nm
            else: ref = nm + "__fwd"; fwd[nm] = 1
        return "(void *)%s" % ref if off == 0 else "(char *)%s + %d" % (ref, off)

    z1src = {}      # Z1 definition name -> where its type came from / why it fell back to a sized array
    tsrc, tdown = {}, {}       # item name -> evidence source of a typed scalar / reason of a downgrade (types_chosen.csv)
    gfloat = set()      # T2 secondary evidence: Ghidra data type float (output/decls.h) and the float overrides of names/MFL_global_types.txt
    for l in open(os.path.join(G, "output/decls.h"), errors="replace"):
        m = re.match(r"/\*([0-9a-f]{8})\*/ extern float \w+;", l)
        if m: gfloat.add(int(m.group(1), 16))
    for l in open(os.path.join(G, "names/MFL_global_types.txt")):
        m = re.match(r"\w*?_([0-9a-f]{8}) float$", l)
        if m: gfloat.add(int(m.group(1), 16))

    def typed_scalar(it, e, b):
        """T2: scalar of the type our C declares for the name (all declarations agree, size == extent); floats only if %.{6..9}g round-trips the exact bits"""
        ds = [d for n in it["names"] for d in cdec.get(n, [])]
        cls = cdecls.scalar_class(ds) if ds else ("f4" if e == 4 and it["va"] in gfloat else None)
        if cls is None:
            if ds: stat["T2 downgrade: declarations disagree or not a scalar"] += 1; tdown[it["name"]] = "declarations disagree or not a scalar"
            return None
        base = cls.lstrip("u")
        if int(base[1]) != e:
            stat["T2 downgrade: C type size != extent"] += 1; tdown[it["name"]] = "C type size != extent"; return None
        tsrc[it["name"]] = "C" if ds else "ghidra-float"
        v = int.from_bytes(b, "little")
        if base == "f4":
            if v == 0: stat["T2 float"] += 1; return "float", "0"
            x = struct.unpack("<f", b)[0]
            if x != x or x in (float("inf"), float("-inf")) or (v >> 23) & 0xff == 0 or v == 0x80000000:
                stat["T2 downgrade: float not finite/normal"] += 1; return None
            for n in range(6, 10):
                t = "%.*g" % (n, x)
                if struct.pack("<f", float(t)) == b: break
            else:
                stat["T2 downgrade: float no exact decimal"] += 1; return None
            if not re.search(r"[.e]", t): t += ".0"
            stat["T2 float"] += 1
            return "float", t + "f"
        ty = {"i1": "char", "i2": "short", "i4": "int"}[base]
        if cls[0] == "u": ty = "unsigned " + ty
        else:
            sv = v - (1 << 8 * e) if v >> (8 * e - 1) else v
            stat["T2 " + ty] += 1
            return ty, ("(-2147483647-1)" if sv == -2147483648 else "%d" % sv)
        stat["T2 " + ty] += 1
        return ty, "%d" % v

    def z4_value(base, b, o, s, pp, sec, cur, nm):
        """initialiser of one base value at byte offset o of the item (s = its address); pp: {offset: pointer} of the item"""
        sz = Z4_BASE[base][1]
        if base in ("ptr", "fptr"):
            if o in pp:
                x = pexpr(sec, pp[o], cur); return x if base == "ptr" else "(z4_fn)" + x if x.startswith("(void *)") else "(z4_fn)(%s)" % x
            if not any(b[o:o + 4]): return "0"
            sys.exit("globgen Z4: %s +%x: a %s word 0x%x without a relocation (type it u4)" % (nm, o, base, int.from_bytes(b[o:o + 4], "little")))
        if any(q in pp for q in range(o - 3, o + sz)): sys.exit("globgen Z4: %s +%x: a relocated word inside a %s field (type it ptr / fptr)" % (nm, o, base))
        v = int.from_bytes(b[o:o + sz], "little")
        if base in ("f4", "f8"):
            fmt, rng, bits = ("<f", range(6, 10), 23) if base == "f4" else ("<d", range(15, 18), 52)
            ex = (v >> bits) & (0xff if base == "f4" else 0x7ff)
            if v == 0: return "0.0f" if base == "f4" else "0.0"
            if v == 1 << (8 * sz - 1): return "-0.0f" if base == "f4" else "-0.0"
            if ex in (0, 0xff if base == "f4" else 0x7ff): sys.exit("globgen Z4: %s +%x: %s 0x%x is not a finite normal number (type it u4)" % (nm, o, base, v))
            x = struct.unpack(fmt, b[o:o + sz])[0]
            for k in rng:
                t = "%.*g" % (k, x)
                if struct.pack(fmt, float(t)) == b[o:o + sz]: break
            else: sys.exit("globgen Z4: %s +%x: no exact decimal" % (nm, o))
            if not re.search(r"[.e]", t): t += ".0"
            return t + ("f" if base == "f4" else "")
        if base == "guid": return "{0x%08x, 0x%04x, 0x%04x, {%s}}" % (v & 0xffffffff, (v >> 32) & 0xffff, (v >> 48) & 0xffff, ", ".join("0x%02x" % c for c in b[o + 8:o + 16]))
        if base == "i4": return "%d" % (v - (1 << 32) if v >> 31 else v) if v != 0x80000000 else "(-2147483647-1)"
        return "0x%x" % v
    def z4_init(base, n, b, o, pp, s, sec, cur, nm):
        """initialiser of base[n] (n None = scalar) at offset o"""
        if base in ("str", "wstr") and any(q in pp for q in range(o - 3, o + n * Z4_BASE[base][1])): sys.exit("globgen Z4: %s +%x: a relocated word inside a %s field" % (nm, o, base))
        if base == "str":
            return cstr(b[o:o + n].rstrip(b"\0"))
        if base == "wstr":
            q = [int.from_bytes(b[o + 2 * k:o + 2 * k + 2], "little") for k in range(n)]
            while q and q[-1] == 0: q.pop()
            return "{" + ", ".join(("'%s'" % ("\\'" if c == 39 else "\\\\" if c == 92 else chr(c))) if 32 <= c < 127 else "%d" % c for c in q) + "}" if q else "{0}"
        if n is None: return z4_value(base, b, o, s, pp, sec, cur, nm)
        w = Z4_BASE[base][1]
        return "{" + ", ".join(z4_value(base, b, o + k * w, s, pp, sec, cur, nm) for k in range(n)) + "}"
    def render_z4(it, sec, cur):
        """W13 Z4: the typed definition of a z4_types.txt entry, same bytes as the image row"""
        en, a, e, nm = it["z4"], it["va"], it["ext"], it["name"]
        b = rd(a, e)
        vo, o = set(), 0      # sites covered by vu2 / vu4 fields
        for base, n, fn, fsz in ((en["fields"] or [(en["base"], en["n"], None, e)]) * ((en.get("n") or 1) if en["fields"] else 1)):
            if base in ("vu2", "vu4"): vo |= set(range(a + o, a + o + fsz))
            o += fsz
        pp = {p[0] - a: p for p in item_ptrs(it, sec, vo)}
        const = "const " if sec == "rdata" else ""
        stat["Z4 typed"] += 1
        if en["fields"] is None:      # `const void *` would be a pointer to const in .data: the object itself is const (void *const)
            return None, ("void *" + const.strip() if en["base"] == "ptr" and const else const + Z4_BASE[en["base"]][0]),"" if en["n"] is None else "[%d]" % en["n"], z4_init(en["base"], en["n"], b, 0, pp, a, sec, cur, nm)
        fs, recs = en["fields"], []
        rs = sum(f[3] for f in fs)
        for r in range(en["n"] or 1):
            o, vals = r * rs, []
            for k, (base, n, fn, fsz) in enumerate(fs):
                vals.append(z4_init(base, n, b, o, pp, a, sec, cur, nm)); o += fsz
            recs.append("{" + ", ".join(vals) + "}")
        mem = " ".join("%s %s%s;" % (Z4_BASE[base][0], fn or "f%x" % sum(f[3] for f in fs[:k]), "[%d]" % n if n is not None else "") for k, (base, n, fn, fsz) in enumerate(fs))
        sd = "#pragma pack(push,1)\nstruct %s_t { %s };\n#pragma pack(pop)\n" % (nm, mem)
        return sd, "%sstruct %s_t" % (const, nm), "" if en["n"] is None else "[%d]" % en["n"], recs[0] if en["n"] is None else "{" + ", ".join(recs) + "}"

    def render(it, sec, cur):
        """-> (struct_def or None, decl_type, declarator suffix, initialiser) for the definition"""
        a, e, nm = it["va"], it["ext"], it["name"]
        if it["kind"] == "libt": return render_z4(it, sec, cur)
        if it["kind"] == "libz" and nm in Z1_TYPES:      # Z1: the type of z1_types.txt only when its size is the extent and its alignment fits the address
            ty_, suf_, sz_, al_, ev_ = Z1_TYPES[nm]
            if sz_ == e and a % al_ == 0: z1src[nm] = "typed: " + ev_; return None, ty_, suf_, "{0}"
            z1src[nm] = "fallback: z1_types.txt %s%s is %d B align %d, extent %d at %08x" % (ty_, suf_, sz_, al_, e, a)
        b = rd(a, e) if sec != "bss" else bytes(e)
        ps = item_ptrs(it, sec) if sec != "bss" else []
        if it["kind"] == "app" and CRT_TABLES[0] <= a < CRT_TABLES[1] and e % 4 == 0:      # Z2: initialiser table -> _PVFV array (every word a pointer or 0)
            pp = {p[0] - a: p for p in ps}
            if all(i in pp or not any(b[i:i + 4]) for i in range(0, e, 4)):
                stat["Z2 _PVFV table"] += 1
                fx = lambda x: "(_PVFV)" + x[8:] if x.startswith("(void *)") else "(_PVFV)(%s)" % x
                return None, "_PVFV", "[%d]" % (e // 4), "{" + ", ".join(fx(pexpr(sec, pp[i], cur)) if i in pp else "0" for i in range(0, e, 4)) + "}"
        const = "const " if sec == "rdata" else ""
        zero = not any(b) and not ps
        if ps:
            mem, init, i, k = [], [], 0, 0
            pp = {p[0] - a: p for p in ps}
            while i < e:
                if i in pp:
                    p = pp[i]
                    mem.append("void *m%d;" % k); init.append(pexpr(sec, p, cur)); i += 4
                else:
                    j = i
                    while j < e and j not in pp: j += 1
                    run = b[i:j]
                    if len(run) % 4 == 0:
                        mem.append("unsigned int m%d[%d];" % (k, len(run) // 4)); init.append("{" + ",".join("0x%x" % x for x in struct.unpack("<%dI" % (len(run) // 4), run)) + "}")
                    else:
                        mem.append("unsigned char m%d[%d];" % (k, len(run))); init.append("{" + ",".join("%d" % x for x in run) + "}")
                    i = j
                k += 1
            sd = "#pragma pack(push,1)\nstruct t_%s { %s };\n#pragma pack(pop)\n" % (nm, " ".join(mem))
            stat["typed struct"] += 1
            return sd, "%sstruct t_%s" % (const, nm), "", "{" + ", ".join(init) + "}"
        if e in (1, 2, 4) and not ps:
            ts = typed_scalar(it, e, b)
            if ts:
                return None, const + ts[0], "", ts[1]
        if e in (1, 2, 4):
            t = {1: "unsigned char", 2: "unsigned short", 4: "unsigned int"}[e]
            stat["typed scalar"] += 1
            return None, const + t, "", "0x%x" % int.from_bytes(b, "little") if not zero else "0"
        if zero:
            stat["zero blob"] += 1
            return None, const + "unsigned char", "[%d]" % e, "{0}"
        if e >= 5 and e <= 2000 and b[-1] == 0 and all(c in PRINT for c in b[:-1].rstrip(b"\0")) and b[0] != 0 and b"\0" not in b[:-1].rstrip(b"\0"):
            stat["string"] += 1
            return None, const + "char", "[%d]" % e, cstr(b.rstrip(b"\0"))
        if e % 4 == 0:
            stat["word array"] += 1
            return None, const + "unsigned int", "[%d]" % (e // 4), "{" + ",".join("0x%x" % x for x in struct.unpack("<%dI" % (e // 4), b)) + "}"
        stat["byte array"] += 1
        return None, const + "unsigned char", "[%d]" % e, "{" + ",".join("%d" % x for x in b) + "}"

    # --- emit ------------------------------------------------------------------------------------------------------------------------------
    lay = open(os.path.join(out, "globals_layout.csv"), "w")
    lay.write("va,size,section,name,kind,folded_labels\n")
    z1rows = []
    _dn = collections.Counter(it["name"] for its in items.values() for it in its)
    assert not [n for n, c in _dn.items() if c > 1], "globgen: a definition name twice: %s" % [n for n, c in _dn.items() if c > 1][:5]      # Z1 object names must not collide
    cnt = collections.Counter()
    hdr = "/* Generated by tools/globals/globgen.py. Do not edit. Pinned layout: address order, extent = gap to next name.\n * Linker generated spans (zero fillers g_pespan_<va>) in %s:\n%s */\n"
    for sec, its in items.items():
        lo, hi, secname = SECT[sec]
        rendered = [(it, None if it["kind"] == "pe" else render(it, sec, i)) for i, it in enumerate(its)]
        pre = []
        extern_names = set()
        for it, r in rendered:
            if r is None: continue
            for m in re.finditer(r"\((?:void \*|char \*|_PVFV)\)\(?(\w+)", r[3]): extern_names.add(m.group(1))
        extern_names -= {it["name"] for it in its}
        for n in sorted(extern_names):
            if n in CONV: pre.append("extern void %s %s(%s);" % (CONV[n][0], n, ",".join("double" if x == 8 else "int" for x in CONV[n][1]) or "void"))   # F1: the prototype gives the decorated link name
            else: pre.append("extern char %s[];" % n)
        f = open(os.path.join(out, sec + ".c"), "w")
        f.write(hdr % (sec, "".join(" *   %08x-%08x\n" % (x, y) for x, y, c in spans[sec] if c == "pe")))
        if sec == "data": f.write('#pragma data_seg(".data")\ntypedef void (__cdecl *_PVFV)(void);      /* Z2: CRT initialiser table entry */\n')
        if any(it["kind"] == "libt" for it in its):      # W13 Z4: the types of the typed library rows
            f.write("typedef void (__cdecl *z4_fn)(void);      /* Z4: code pointer of a library table (vtable / method slot) */\n"
                    "struct mfl_guid { unsigned long d1; unsigned short d2; unsigned short d3; unsigned char d4[8]; };      /* Z4: a 16 byte GUID / UUID */\n")
        zh = os.path.join(os.path.dirname(os.path.abspath(__file__)), "z1_types.h")
        if sec == "bss" and os.path.exists(zh): f.write(open(zh).read() + "\n")      # Z1: the struct types of z1_types.txt (declarations only, no storage)
        f.write("\n".join(pre) + "\n\n")
        for it, r in rendered:
            if r is not None and it["kind"] == "lib" and (libsrc_dead(sec, it) or it.get("z4dead")):      # W13 Z4: a `dead` row of z4_types.txt the same way
                r = None; it["kind"] = "dead"      # LIBSRC=qhull (A4): the lib is compiled from source with its own data, its old data rows are zero fill
            if r is None:
                ext = it["ext"]
                f.write("/* PESPAN %08x +%x */\n%sunsigned char %s[%d]%s;\n" % (it["va"], ext, "const " if sec == "rdata" else "", it["name"], ext, " = {0}"))
                lay.write("%08x,%d,%s,%s,%s,\n" % (it["va"], ext, sec, it["name"], "dead" if it["kind"] == "dead" else "pe"))
                cnt[sec + " " + ("dead" if it["kind"] == "dead" else "pe") + " bytes"] += ext
                continue
            sd, ty, suf, ini = r
            if sd: f.write(sd)
            f.write("/*%08x*/ %s %s%s = %s;\n" % (it["va"], ty, it["name"], suf, ini))
            if it["kind"] == "libz":      # Z1: size assert (cl 13 has no _Static_assert: a negative array size fails the compile)
                f.write("typedef char z1_size_%s[sizeof(%s) == %d ? 1 : -1];\n" % (it["name"], it["name"], it["ext"]))
                z1rows.append((it["va"], it["ext"], it["name"], it.get("member", ""), "%s %s%s" % (ty, it["name"], suf), z1src.get(it["name"], "sized array / scalar (no type in z1_types.txt)")))
            if it["kind"] == "libt":      # W13 Z4: size assert per typed library definition
                f.write("typedef char z4_size_%s[sizeof(%s) == %d ? 1 : -1];\n" % (it["name"], it["name"], it["ext"]))
            lay.write("%08x,%d,%s,%s,%s,%s\n" % (it["va"], it["ext"], sec, it["name"], it["kind"], " ".join("%x" % x for x in it.get("folded", []))))
            cnt[sec + " " + it["kind"]] += 1
            cnt[sec + " " + it["kind"] + " bytes"] += it["ext"]
        f.close()
    lay.close()
    with open(os.path.join(out, "z1_rows.csv"), "w") as f:      # Z1: every named zero definition, its type and the type's source (sized-array fall-backs listed)
        f.write("va,size,name,member,decl,type_source\n")
        for r in z1rows: f.write("%08x,%d,%s,%s,%s,%s\n" % (r[0], r[1], r[2], r[3], r[4], r[5].replace(",", ";")))
    # F4: every reloc site inside a generated item is either a pointer (ptrs.csv) or listed in raw.csv with a reason; count the rest
    for s in sorted(relocs):
        i = bisect.bisect_right(starts, s) - 1
        if i < 0 or not (SECT["rdata"][0] <= s < SECT["bss"][1]): continue
        va, ext, it, sec = allapp[i]
        if s >= va + ext or it["kind"] == "pe" or sec == "bss": continue
        if s in emitted or s in rawsites: continue
        rawsites[s] = "crosses item end" if s + 4 > va + ext else "unclassified"
        stat["reloc " + rawsites[s]] += 1
    aliases += [(n + "__fwd", 0, n) for n in sorted(fwd)]
    with open(os.path.join(out, "aliases_gen.txt"), "w") as f:
        f.write("# name offset parent  (offset 0 = second name of the same address)\n")
        for n, o, p in aliases: f.write("%s %d %s\n" % (n, o, p))
    with open(os.path.join(out, "ptrs.csv"), "w") as f:
        f.write("site,symbol,link,addend,orig,conf\n")
        for s, nm, ln, off, v, c in ptrs: f.write("%08x,%s,%s,%d,%08x,%s\n" % (s, nm, ln, off, v, c))
    with open(os.path.join(out, "raw.csv"), "w") as f:
        f.write("site,reason\n")
        for s, r in sorted(rawsites.items()): f.write("%08x,%s\n" % (s, r))
    with open(os.path.join(out, "low_pointers.csv"), "w") as f:
        f.write("site,item,value,kind,symbol,off,reloc_kind,why,prev_conf,next_conf,allowed\n")
        for r in lowrows: f.write(",".join(("%08x" % x if i in (0, 2) else str(x)) for i, x in enumerate(r)).replace("\n", " ") + "\n")
    # objcopy aliases for the check build: aliases + pe labels / other excluded targets referenced by pointers, as section offsets
    lay_by_name = {}
    for sec, its in items.items():
        for it in its: lay_by_name[it["name"]] = (sec, it["va"])
    oc = collections.defaultdict(list)
    done = set()
    for n, o, p in aliases:
        sec, va = lay_by_name[p]
        oc[sec].append("--add-symbol _%s=%s:0x%x,global" % (n, SECT[sec][2], va + o - SECT[sec][0])); done.add(n)
    for nm, va in ext_syms.items():
        if nm in done: continue
        for sec, (lo, hi, sn) in SECT.items():
            if lo <= va < hi: oc[sec].append("--add-symbol _%s=%s:0x%x,global" % (nm, sn, va - lo))
    # SSE tables in .rdata are read with movapd: the generated .rdata (original start 0x5dd000, 16-aligned) must start 0 mod 16 whatever size the linker's IAT in front of it has
    oc["rdata"].append("--set-section-alignment .rdata=16")
    for sec, ls in oc.items():
        open(os.path.join(out, "objcopy_%s.txt" % sec), "w").write("\n".join(ls) + "\n")
    codefns = sorted(n for n, va in ext_syms.items() if not any(lo <= va < hi for lo, hi, _ in SECT.values()))
    with open(os.path.join(out, "stubs.c"), "w") as f:
        f.write("/* check build only: stand-ins for pointer targets in code (same calling convention as our C: decorated link names) */\n")
        for n in codefns:
            if n in CONV: f.write("void %s %s(%s) {}\n" % (CONV[n][0], n, ",".join("%s a%d" % ("double" if x == 8 else "int", i) for i, x in enumerate(CONV[n][1])) or "void"))
            else: f.write("void %s(void) {}\n" % n)
    # F1 verification against the original code: stdcall `ret N` with N = bytes of parameters (fastcall: bytes minus the two register arguments)
    bad, ret_imm_unknown, tailjmp = [], [], []
    for n in codefns:
        va = ext_syms[n]
        k = bisect.bisect_left(fn_starts, va)
        if k >= len(fn) or fn[k][0] != va: continue
        code = rd(va, fn[k][1])
        rets = {struct.unpack_from("<H", code, i + 1)[0] for i in range(len(code) - 2) if code[i] == 0xc2}
        if n in CONV:
            c, ps = CONV[n]
            want = sum(ps) if c == "__stdcall" else max(0, sum(ps) - 8)
            if not rets: tailjmp.append(n)          # COM adjustor thunks end in jmp: nothing to compare
            elif want not in rets: bad.append("%s %s %d (ret %s)" % (n, c, sum(ps), sorted(rets)))
        elif rets and all(r % 4 == 0 and 0 < r < 128 for r in rets) and n in cdefs: ret_imm_unknown.append("%s ret %s" % (n, sorted(rets)))
    with open(os.path.join(out, "types_chosen.csv"), "w") as f:
        f.write("name,source,reason\n")
        for n, v in sorted(tsrc.items()): f.write("%s,%s,\n" % (n, v))
        for n, v in sorted(tdown.items()): f.write("%s,default,%s\n" % (n, v))
    rep = open(os.path.join(out, "globgen_report.txt"), "w")
    for k, v in sorted(cnt.items()): rep.write("%s: %d\n" % (k, v))
    for k, v in sorted(stat.items()): rep.write("%s: %d\n" % (k, v))
    rep.write("carved out of library spans (names our C uses): %d\naliases at an offset inside an item (interior and library labels): %d\naliases at the item start (second names): %d\nmisaligned items (no parent): %d\nbytes folded into an item of another class: %d %s\nitems split 4+tail (>=64 B at 4 mod 8): %d (unsplittable: %d)\ncode stubs: %d\nnon-zero bytes absorbed into pe fillers: %s\n" % (carved, nint, nsame, misaligned, absorbed, dict(absmix), split, splitbad, len(codefns), [hex(x) for x in pe_nonzero]))
    rep.write("W13 Z4 (z4_types.txt): typed library bytes %d, dead rows %d B, put back: %s\n" % (z4n["libt"], z4n["dead"], ",".join(sorted(Z4_PUTBACK)) or "-"))
    rep.write("reloc sites staying raw: %d (raw.csv); by reason: %s\n" % (len(rawsites), dict(collections.Counter(rawsites.values()))))
    rep.write("function targets with decorated names (F1): %d, ret-N verification failures: %d %s (tail-jump thunks not comparable: %d); targets our C DEFINES as cdecl although the original returns with ret N: %d %s; names with conflicting conv: %s; stdcall declared only (legacy, undecorated): %d\n" % (sum(1 for n in codefns if n in CONV), len(bad), bad, len(tailjmp), len(ret_imm_unknown), ret_imm_unknown[:10], sorted(conv_conflict)[:10], len(conv_decl_only)))
    rep.close()
    print(open(os.path.join(out, "globgen_report.txt")).read())


if __name__ == "__main__":
    main(sys.argv[1])
