#!/usr/bin/env python3
"""Globals generator (plan_standalone_exe.md S4, design: output/design_globals_generator.md).
Emits the game's app data (.rdata/.data/.bss minus library/import/CRT-table spans) as C definitions in PINNED layout: one TU per section, address
order, extent = gap to the next name.  Library spans are emitted as bytes plus pointers (g_libspan_<va>, variant L: the generator owns the whole data layout); linker generated PE spans are zero fillers (g_pespan_<va>).
Usage: globgen.py <outdir>      Output: rdata.c data.c bss.c globals_layout.csv aliases_gen.txt ptrs.csv raw.csv low_pointers.csv objcopy_<sec>.txt stubs.c globgen_report.txt"""
import bisect, collections, csv, os, re, struct, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import cdecls

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
    sp = {"rdata": [(0x5e2670, 0x5f8e30)], "data": [(0x5fb000, 0x5fb074), (0x62c5d8, 0x64c274)], "bss": [(0x771ac0, 0x771f78), (0x7d1fa0, 0x7daa1c), (0x7dc298, 0x7dc2c4)]}
    rows, shared = [], []
    for r in csv.DictReader(open(os.path.join(G, "output/delink_sdk/ownership.csv"))):
        lo, hi = int(r["lo"], 16), int(r["hi"], 16)
        if r["kind"] in ("bss", "common") and r["owner"] in ("crt", "rw"): rows.append((lo, hi))
        if r["kind"] in ("rdata", "data") and r["owner"] == "shared": shared.append((lo, hi))
    sp["bss"] += [r for r in rows if r[0] >= 0x64d000]

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
        v = cut(merge(v), pe + (merge(shared) if k != "bss" else []))
        out[k] = sorted([(a, b, "lib") for a, b in v] + ([(max(a, SECT[k][0]), min(b, SECT[k][1]), "pe") for a, b in pe if b > SECT[k][0] and a < SECT[k][1]] if k == "rdata" else []))
    return out


# --- names ----------------------------------------------------------------------------------------------------------------------------------
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

    # items per section
    items = {}      # sec -> list of dict(va, ext, names, kind app/gap/lib/pe, ...)
    carved = 0
    for sec, (lo, hi, _) in SECT.items():
        sp = spans[sec]
        bounds = {lo, hi} | {x for a, b, c in sp for x in (a, b)} | {a for a in names if lo <= a < hi}
        bs = sorted(bounds)
        its = []
        for a, b in zip(bs, bs[1:]):
            cls = next((c for x, y, c in sp if x <= a < y), None)
            nm = names.get(a, [])
            if cls == "lib" and any(n in used for n in nm): cls = None; carved += 1      # F3: a name our C uses is app data, also inside a library span
            if cls:
                if its and its[-1]["kind"] == cls and its[-1]["va"] + its[-1]["ext"] == a:
                    its[-1]["ext"] += b - a
                    continue
                its.append(dict(va=a, ext=b - a, kind=cls, names=[], name=("g_libspan_%08x" if cls == "lib" else "g_pespan_%08x") % a))
                continue
            prim = next((n for n in nm if n in used), nm[0] if nm else None)
            its.append(dict(va=a, ext=b - a, kind="app" if nm else "gap", names=nm, name=prim or "g_gap_%08x" % a))
        items[sec] = its
    FILL = ("lib", "pe")

    # alignment fold: interior labels
    misaligned = split = splitbad = absorbed = 0
    absmix = collections.Counter()      # bytes of one class folded into an item of another class (alignment limit of cl: strings cannot start at odd addresses)
    pe_nonzero = []         # non-zero bytes absorbed into a zero pe filler (must stay empty)
    for sec, its in items.items():
        res = []
        pre = []
        for it in its:       # a library item that starts at an unaligned address gives only its head (up to the next 4-boundary) to the fold below, the rest stays a library item
            if it["kind"] == "lib" and it["va"] % 4 and it["ext"] > 4:
                h = 4 - it["va"] % 4
                pre += [dict(it, ext=h), dict(it, va=it["va"] + h, ext=it["ext"] - h, name="g_libspan_%08x" % (it["va"] + h))]
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
        return ("raw", None, 0, None)

    ptrs = []             # site, symbol, link symbol, addend, original value, conf
    def item_ptrs(it, sec):
        a, e = it["va"], it["ext"]
        out = []
        for s in range(a, a + e - 3):
            r = relocs.get(s)
            if r is None: continue
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

    def render(it, sec, cur):
        """-> (struct_def or None, decl_type, declarator suffix, initialiser) for the definition"""
        a, e, nm = it["va"], it["ext"], it["name"]
        b = rd(a, e) if sec != "bss" else bytes(e)
        ps = item_ptrs(it, sec) if sec != "bss" else []
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
    cnt = collections.Counter()
    hdr = "/* Generated by tools/globals/globgen.py. Do not edit. Pinned layout: address order, extent = gap to next name.\n * Linker generated spans (zero fillers g_pespan_<va>) in %s:\n%s */\n"
    for sec, its in items.items():
        lo, hi, secname = SECT[sec]
        rendered = [(it, None if it["kind"] == "pe" else render(it, sec, i)) for i, it in enumerate(its)]
        pre = []
        extern_names = set()
        for it, r in rendered:
            if r is None: continue
            for m in re.finditer(r"\((?:void \*|char \*)\)(\w+)", r[3]): extern_names.add(m.group(1))
        extern_names -= {it["name"] for it in its}
        for n in sorted(extern_names):
            if n in CONV: pre.append("extern void %s %s(%s);" % (CONV[n][0], n, ",".join("double" if x == 8 else "int" for x in CONV[n][1]) or "void"))   # F1: the prototype gives the decorated link name
            else: pre.append("extern char %s[];" % n)
        f = open(os.path.join(out, sec + ".c"), "w")
        f.write(hdr % (sec, "".join(" *   %08x-%08x\n" % (x, y) for x, y, c in spans[sec] if c == "pe")))
        if sec == "data": f.write('#pragma data_seg(".data")\n')
        f.write("\n".join(pre) + "\n\n")
        for it, r in rendered:
            if r is None:
                ext = it["ext"]
                f.write("/* PESPAN %08x +%x */\n%sunsigned char %s[%d]%s;\n" % (it["va"], ext, "const " if sec == "rdata" else "", it["name"], ext, " = {0}"))
                lay.write("%08x,%d,%s,%s,pe,\n" % (it["va"], ext, sec, it["name"]))
                cnt[sec + " pe bytes"] += ext
                continue
            sd, ty, suf, ini = r
            if sd: f.write(sd)
            f.write("/*%08x*/ %s %s%s = %s;\n" % (it["va"], ty, it["name"], suf, ini))
            lay.write("%08x,%d,%s,%s,%s,%s\n" % (it["va"], it["ext"], sec, it["name"], it["kind"], " ".join("%x" % x for x in it.get("folded", []))))
            cnt[sec + " " + it["kind"]] += 1
            cnt[sec + " " + it["kind"] + " bytes"] += it["ext"]
        f.close()
    lay.close()
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
    rep.write("reloc sites staying raw: %d (raw.csv); by reason: %s\n" % (len(rawsites), dict(collections.Counter(rawsites.values()))))
    rep.write("function targets with decorated names (F1): %d, ret-N verification failures: %d %s (tail-jump thunks not comparable: %d); targets our C DEFINES as cdecl although the original returns with ret N: %d %s; names with conflicting conv: %s; stdcall declared only (legacy, undecorated): %d\n" % (sum(1 for n in codefns if n in CONV), len(bad), bad, len(tailjmp), len(ret_imm_unknown), ret_imm_unknown[:10], sorted(conv_conflict)[:10], len(conv_decl_only)))
    rep.close()
    print(open(os.path.join(out, "globgen_report.txt")).read())


if __name__ == "__main__":
    main(sys.argv[1])
