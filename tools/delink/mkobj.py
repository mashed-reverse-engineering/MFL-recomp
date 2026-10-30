#!/usr/bin/env python3
"""Delinker step 1 round trip: write the whole MFL.exe image as ONE relocatable COFF object: the sections as raw bytes (.text, _rwcseg, .rdata,
.data + .bss for its uninitialised tail, _rwdseg), one IMAGE_REL_I386_DIR32 relocation per pointer site of output/delink_relocs.csv (against the
target's section symbol, the addend stored in place), one external symbol per function (output/fnsizes.txt) and _mfl_entry at the entry point.
The VC7 link.exe can then place it at any base; check_roundtrip.py compares the result with the original.
Usage: mkobj.py <out.obj> [--min-conf high|medium|low] [--drop-kinds header,rsrc,unknown] [--exe <patched copy of MFL.exe, e.g. prepare.py's>]
       [--skip <file of hex sites>] [--rename <file: name new_name>] [--objs <list of our .obj files (last word per line)>] [--grow-text <hex size>] [--pad <hex size> [--pad-code]: no access section in front, e.g. 5f8000 = the whole old address range 0x401000-0x9f9000 traps at base 0x400000]"""
import bisect, collections, csv, os, re, struct, sys
sys.argv, _a = sys.argv[:2], sys.argv[2:]
H = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(H, "..", ".."))
OUT = sys.argv[1]
opt = dict(zip([x for x in _a if x != "--pad-code"][::2], [x for x in _a if x != "--pad-code"][1::2]))
MINC = {"high": 0, "medium": 1, "low": 2}[opt.get("--min-conf", "low")]
DROP = set(opt.get("--drop-kinds", "header,rsrc,unknown,const").split(","))
EXE = opt.get("--exe") or os.path.normpath(os.path.join(G, "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe"))
d = open(EXE, "rb").read()
e = struct.unpack_from("<I", d, 0x3C)[0]; ns = struct.unpack_from("<H", d, e + 6)[0]; osz = struct.unpack_from("<H", d, e + 20)[0]
base = struct.unpack_from("<I", d, e + 24 + 28)[0]; entry = base + struct.unpack_from("<I", d, e + 24 + 16)[0]
ALIGN4K = 0x00D00000
secs = []                      # name, va, bytes (None = uninitialised), size, flags
PAD = int(opt.get("--pad", "0"), 16)
if PAD and "--pad-code" in _a:  # a zero filled code section first: link.exe places it in front of .text, so code AND data move up by PAD (pefix makes it no access)
    secs.append([".pad", 0xF0000000, bytes(PAD), PAD, 0x60000020 | ALIGN4K])
elif PAD:                      # a no access section in front: the image moves up by PAD inside the same base, the old address range becomes a trap
    secs.append([".pad", 0xF0000000, None, PAD, 0x80 | ALIGN4K])   # link.exe puts code first, so the pad lands between _rwcseg and .rdata: data moves, code stays
for i in range(ns):
    o = e + 24 + osz + i * 40
    nm = d[o:o + 8].rstrip(b"\0").decode(); vs, va, rs, rp = struct.unpack_from("<IIII", d, o + 8); fl, = struct.unpack_from("<I", d, o + 36)
    if nm == ".rsrc": continue
    n = min(vs, rs)
    if vs > rs and "--objs" in opt:      # with other objects the tail must stay glued to .data (arrays straddle the file boundary; our .data would land in between): zero filled
        secs.append([nm, base + va, d[rp:rp + n] + bytes(vs - n), vs, (fl & 0xFE000060) | ALIGN4K]); continue
    secs.append([nm, base + va, d[rp:rp + n], n, (fl & 0xFE000060) | ALIGN4K])
    if vs > rs:                # the uninitialised tail of .data becomes .bss (the linker merges it back behind .data)
        secs.append([".bss", base + va + rs, None, vs - rs, 0xC0000080 | ALIGN4K])
if "--grow-text" in opt:     # test layout: a zero filled .text$zz contribution behind .text moves _rwcseg and the data, .text stays (like linking our objects)
    secs.append([".text$zz", 0xF1000000, bytes(int(opt["--grow-text"], 16)), int(opt["--grow-text"], 16), 0x60000020 | 0x00500000])
def sec_index(v):
    for i, s in enumerate(secs):
        if s[1] <= v < s[1] + s[3]: return i
    return None
# relocations
data = [bytearray(s[2]) if s[2] is not None else None for s in secs]
rel = collections.defaultdict(list)
stat = collections.Counter()
taken = {}
SKIP = {int(l.split()[0], 16) for l in open(opt["--skip"]) if l.strip()} if "--skip" in opt else set()     # sites to leave alone (bisecting)
IMPUSED = set()
IMPSYM = {}      # --imports-as-symbols <dir of the <dll>.def files of tools/mk_importlibs.py>: IAT slot VA -> `_Name@N`; the 856 import sites then reference `__imp__Name@N` (the import libs define it), the original IAT and import tables are zero filled and the linker builds the import table
if "--imports-as-symbols" in opt:
    _dec = {}
    for _f in os.listdir(opt["--imports-as-symbols"]):
        if _f.endswith(".def"):
            for _l in open(os.path.join(opt["--imports-as-symbols"], _f)):
                _m = re.match(r"(\w+)@(\d+)$", _l.strip())
                if _m: _dec[_m.group(1)] = _l.strip()
                _m = re.match(r"(Ordinal_\d+) @", _l)
                if _m: _dec[_m.group(1)] = _m.group(1)
    def _rd0(va):
        si_ = sec_index(va); return struct.unpack_from("<I", secs[si_][2], va - secs[si_][1])[0]
    def _zero(va, n):
        si_ = sec_index(va); data[si_][va - secs[si_][1]:va - secs[si_][1] + n] = bytes(n)
    def _cstr(va):
        si_ = sec_index(va); o_ = va - secs[si_][1]; return secs[si_][2].index(0, o_) - o_ + 1
    _imp = base + struct.unpack_from("<I", d, e + 24 + 96 + 8)[0]; _zl = [(_imp, struct.unpack_from("<I", d, e + 24 + 96 + 12)[0])]    # descriptor table
    while any(_rd0(_imp + k) for k in (0, 12, 16)):
        _t0 = _t = base + _rd0(_imp + (0 if _rd0(_imp) else 16)); _slot = base + _rd0(_imp + 16)
        _zl.append((base + _rd0(_imp + 12), _cstr(base + _rd0(_imp + 12))))      # DLL name
        while _rd0(_t):
            if _rd0(_t) & 0x80000000: IMPSYM[_slot] = "__imp__Ordinal_%d" % (_rd0(_t) & 0xFFFF); _t += 4; _slot += 4; continue
            _h = base + _rd0(_t) + 2; _nm = _cstr(_h) - 1
            IMPSYM[_slot] = "__imp__" + _dec[secs[sec_index(_h)][2][_h - secs[sec_index(_h)][1]:][:_nm].decode()]
            _zl.append((_h - 2, 2 + _nm + 1)); _t += 4; _slot += 4
        _zl.append((_t0, _t - _t0 + 4)); _imp += 20      # INT incl. terminator
    _zl.append((base + struct.unpack_from("<I", d, e + 24 + 96 + 8 * 12)[0], struct.unpack_from("<I", d, e + 24 + 96 + 8 * 12 + 4)[0]))      # IAT
    for _va, _n in _zl: _zero(_va, _n)
rows = sorted(csv.DictReader(open(os.path.join(G, "output", "delink_relocs.csv"))), key=lambda r: ["high", "medium", "low"].index(r["confidence"]))
for r in rows:
    site, t = int(r["site"], 16), int(r["target"], 16)
    if site in SKIP: stat["skipped (--skip)"] += 1; continue
    if ["high", "medium", "low"].index(r["confidence"]) > MINC or r["kind"] in DROP or r["kind"] == "rva": stat["skipped " + r["kind"] + "/" + r["confidence"]] += 1; continue
    si, ti = sec_index(site), sec_index(t)
    if si is None or ti is None or data[si] is None: stat["outside sections"] += 1; continue
    if IMPSYM and r["kind"] == "import":          # S2: the operand is the address of an IAT slot -> `__imp__Name@N`, resolved by the import libs
        assert t in IMPSYM, "import site %x: %x is no IAT slot" % (site, t)
        off = site - secs[si][1]
        if any(site + k in taken for k in range(4)): stat["overlap dropped"] += 1; continue
        for k in range(4): taken[site + k] = 1
        struct.pack_into("<I", data[si], off, 0); rel[si].append((off, IMPSYM[t], 6)); IMPUSED.add(IMPSYM[t]); stat["imports as symbols"] += 1; continue
    if any(site + k in taken for k in range(4)): stat["overlap dropped"] += 1; continue
    for k in range(4): taken[site + k] = 1
    off = site - secs[si][1]
    if r["kind"] == "rel32":          # call/jmp rel32 into another section (.text <-> _rwcseg): REL32 against the target section, addend in place
        struct.pack_into("<I", data[si], off, t - secs[ti][1]); rel[si].append((off, ti, 0x14)); stat["relocated"] += 1; stat["rel32 (cross section branches)"] += 1; continue
    if struct.unpack_from("<I", data[si], off)[0] != t: stat["site patched in --exe, left alone"] += 1; continue
    struct.pack_into("<I", data[si], off, t - secs[ti][1])           # addend relative to the target section
    rel[si].append((off, ti, 6))
    stat["relocated"] += 1
# import directory: its RVA fields (descriptor OriginalFirstThunk / Name / FirstThunk, thunk entries -> hint/name) as IMAGE_REL_I386_DIR32NB,
# so the tables stay valid when the sections move (--pad); pefix.py points the data directories at the new place
def rva_reloc(fva):
    si = sec_index(fva); off = fva - secs[si][1]
    r = struct.unpack_from("<I", data[si], off)[0]
    ti = sec_index(base + r)
    struct.pack_into("<I", data[si], off, base + r - secs[ti][1]); rel[si].append((off, ti, 7)); stat["rva fields (DIR32NB)"] += 1
imp = base + struct.unpack_from("<I", d, e + 24 + 96 + 8)[0]
def rd(va):
    si = sec_index(va); return struct.unpack_from("<I", data[si], va - secs[si][1])[0]
while not IMPSYM and any(rd(imp + k) for k in (0, 12, 16)):
    for k in (0, 16):
        t = base + rd(imp + k)
        if rd(imp + k):
            while rd(t):
                if not rd(t) & 0x80000000: rva_reloc(t)
                t += 4
    for k in (0, 12, 16):
        if rd(imp + k): rva_reloc(imp + k)
    imp += 20
# --objs <list>: link OUR objects in place of original functions (no overlay).  Every code symbol an object defines that is a Ghidra function in
# .text gets a 5 byte `jmp <decorated symbol>` (REL32) at the original entry (the original body stays, under name_orig); every symbol the objects
# leave undefined is defined here by its decorated name at the address the overlay would use (symbols.txt + aliases.txt, imports at their IAT slot).
def undecorate(n):
    if n.startswith("@"): return n[1:].split("@")[0]
    if n.startswith("_"): n = n[1:]
    return n.split("@")[0] if re.match(r"^\w+@\d+$", n) else n
def coff_syms(path):
    d_ = open(path, "rb").read(); ns_, so_, nsy_ = struct.unpack_from("<H", d_, 2)[0], *struct.unpack_from("<II", d_, 8)
    st_ = so_ + nsy_ * 18; secfl = [struct.unpack_from("<I", d_, 20 + struct.unpack_from("<H", d_, 16)[0] + k * 40 + 36)[0] for k in range(ns_)]
    i_ = 0
    while i_ < nsy_:
        o_ = so_ + i_ * 18; raw = d_[o_:o_ + 8]
        nm_ = d_[st_ + struct.unpack_from("<I", raw, 4)[0]:d_.index(b"\0", st_ + struct.unpack_from("<I", raw, 4)[0])].decode() if raw[:4] == b"\0\0\0\0" else raw.rstrip(b"\0").decode()
        val_, sec_, typ_, cls_, aux_ = struct.unpack_from("<IhHBB", d_, o_ + 8)
        if cls_ == 2: yield nm_, sec_, val_, bool(sec_ > 0 and secfl[sec_ - 1] & 0x20)
        i_ += 1 + aux_
symmap = {}
for f_ in ("src/symbols.txt", "src/aliases.txt", "output/delink_sdk/lib_names.txt"):     # lib_names: located library functions / data by their lib names (ownership.py)
    if os.path.exists(os.path.join(G, f_)):
        for l in open(os.path.join(G, f_)):
            q_ = l.split()
            if len(q_) >= 2 and not q_[0].startswith("#"): symmap.setdefault(q_[0], int(q_[1], 16))
imports = {}
def rdo(va):           # original bytes (data[] already holds the relocation addends)
    si_ = sec_index(va); return struct.unpack_from("<I", secs[si_][2], va - secs[si_][1])[0]
imp = base + struct.unpack_from("<I", d, e + 24 + 96 + 8)[0]
while any(rdo(imp + k) for k in (0, 12, 16)):
    t_, slot_ = base + rdo(imp + (0 if rdo(imp) else 16)), base + rdo(imp + 16)
    while rdo(t_):
        if not rdo(t_) & 0x80000000:
            h_ = base + rdo(t_) + 2; si_ = sec_index(h_); o_ = h_ - secs[si_][1]
            imports[secs[si_][2][o_:secs[si_][2].index(0, o_)].decode()] = slot_
        t_ += 4; slot_ += 4
    imp += 20
objdef, objundef, aliases, undef_syms, missing = {}, set(), [], [], set()
undef_syms += sorted(IMPUSED)
commons = set()
memdefs = collections.defaultdict(set)          # lib member key (objs line "lib:<key> <obj>") -> external names it defines
# --drop-members <file of member keys> [--drop-items <csv>]: split those library members out of the image WITH their data (tools/delink_sdk/plan_split.py
# picks them): their code / data bytes (ownership_items.csv) are filled (int3 / 0), relocations inside them vanish, and every live reference into
# them becomes a symbol reference (function start or external data symbol + offset; rel32 calls / jumps from output/delink/ghidra_refs.csv);
# the member objects (lib:<key> lines of --objs) supply the code and data.  References that have no symbol are listed as blockers.
DROP = []
redir_at = {}          # address -> name of every function redirected to our objects
DSEC = set()           # --drop-app: indices of the sections that leave the image (.rdata .data .bss); the generated data objects define them
if "--drop-members" in opt:
    _dm = {l.strip() for l in open(opt["--drop-members"]) if l.strip()}
    DROP = sorted(((int(r["lo"], 16), int(r["hi"], 16), r) for r in csv.DictReader(open(opt.get("--drop-items", os.path.join(G, "output", "delink_sdk", "ownership_items.csv"))))
                   if r["member"] in _dm and not r["owner"].endswith("?") and r["owner"] != "shared" and (not r["owner"].endswith("~") or os.environ.get("VARIANTS") == "1")), key=lambda x: x[:2])
_DL = [x[0] for x in DROP]
def dropped(a):
    i = bisect.bisect_right(_DL, a) - 1
    return DROP[i] if i >= 0 and a < DROP[i][1] else None
REDIR = {l.split()[0] for l in open(opt["--redirect-only"]) if l.strip()} if "--redirect-only" in opt else None      # bisecting: only these functions get the jmp
if "--objs" in opt:
    for l in open(opt["--objs"]):
        if not l.strip(): continue
        for nm_, sec_, val_, code_ in coff_syms(l.split()[-1]):
            if l.startswith("lib:") and (sec_ > 0 or val_ > 0): memdefs[l.split()[0][4:]].add(nm_)
            if sec_ > 0: objdef.setdefault(nm_, code_)
            elif sec_ == 0 and val_ == 0: objundef.add(nm_)
            elif sec_ == 0: commons.add(nm_)            # COMMON (uninitialised, size in value): the linker allocates it unless someone defines it
    if "--drop-app" in opt:         # the generated data objects reference functions / labels of the image by name (pointer words): define those too
        _gd, _gu = set(), set()
        for x_ in ("rdata", "data", "bss"):
            for nm_, sec_, val_, code_ in coff_syms(os.path.join(opt["--drop-app"], x_ + ".obj")):
                (_gu if sec_ == 0 and val_ == 0 else _gd).add(nm_)
        objundef |= _gu - _gd
    tsec = 0 if not PAD or "--pad-code" not in _a else 1
    redirected = set()
    for nm_, code_ in objdef.items():
        u = undecorate(nm_)
        a = symmap.get(u)
        if not code_ or a is None or sec_index(a) != tsec or dropped(a): continue
        if REDIR is not None and u not in REDIR: continue
        off = a - secs[tsec][1]
        rel[tsec] = [r_ for r_ in rel[tsec] if not off - 3 <= r_[0] < off + 5]       # pointer fields inside the 5 patched bytes are gone with them
        data[tsec][off:off + 5] = b"\xe9\0\0\0\0"
        rel[tsec].append((off + 1, nm_, 0x14)); undef_syms.append(nm_); redirected.add(u); redir_at[a] = u
    stat["functions redirected to our objects"] = len(redirected)
    byu = {undecorate(n_): n_ for n_ in objdef}
    for fk, ty_ in (("--hook-calls", 0x14), ("--dwords", 0x6)):      # harness lists "<VA>=<symbol>": retarget a 5 byte E8 call (REL32) / store the symbol's address in a 4 byte operand (DIR32)
        if fk not in opt: continue
        for l in open(opt[fk]):
            if not l.strip() or l.startswith("#"): continue
            va_s, nm_ = l.split()[0].split("="); va = int(va_s, 16); si_ = sec_index(va); off = va - secs[si_][1]
            if ty_ == 0x14: assert data[si_][off] == 0xE8, "hook-call %s: no call there" % va_s; off += 1
            rel[si_] = [r_ for r_ in rel[si_] if not off - 3 <= r_[0] <= off + 3]
            data[si_][off:off + 4] = bytes(4); rel[si_].append((off, byu[nm_], ty_)); undef_syms.append(byu[nm_]); stat["harness " + fk[2:]] += 1
    DROPNAMES = {}
    if DROP:
        for r in csv.DictReader(open(os.path.join(G, "output", "delink_sdk", "symbols.csv"))):
            if r["addr"] and r["binding"] in ("ext", "common") and dropped(int(r["addr"], 16)): DROPNAMES.setdefault(int(r["addr"], 16), r["symbol"])
        _nm = lambda n: n if n.startswith("?") else n.lstrip("_@").split("@")[0]
        _nd = {k: {_nm(n): n for n in v} for k, v in memdefs.items()}
        for lo_, hi_, r in DROP:                      # split functions: our C may call them by a Ghidra spelling (_strstr for strstr)
            if r["kind"] == "code":
                n_ = _nd.get(r["member"].replace(" ", "_"), {}).get(_nm(r["name"]))
                if n_: DROPNAMES.setdefault(lo_, n_)
    alt = []                         # /alternatename pairs: our C names an address by its Ghidra name, the split member defines it by the lib name
    for nm_ in sorted(objundef - set(objdef) - commons):
        if nm_.startswith("__imp_") and IMPSYM: continue          # S2: defined by the import libs
        if nm_.startswith("__imp_"):
            u = undecorate(nm_[6:])
            if u in imports: aliases.append((nm_, imports[u])); continue
        u = undecorate(nm_)
        m_ = re.match(r"func_0x([0-9a-f]{8})$", u)          # decompiler names for functions without a symbol
        if m_: aliases.append((nm_, int(m_.group(1), 16)))
        elif any(c_ in symmap and sec_index(symmap[c_]) is not None and not dropped(symmap[c_]) for c_ in (u, nm_, nm_.lstrip("_"))):
            aliases.append((nm_, next(symmap[c_] for c_ in (u, nm_, nm_.lstrip("_")) if c_ in symmap and sec_index(symmap[c_]) is not None and not dropped(symmap[c_]))))
        elif u in symmap and dropped(symmap[u]) and DROPNAMES.get(symmap[u]): alt.append((nm_, DROPNAMES[symmap[u]]))
        elif nm_ == "__fltused": aliases.append((nm_, secs[0 if "--drop-app" in opt else [s_[0] for s_ in secs].index(".data")][1]))     # only its existence matters (CRT float marker)
        else: missing.add(nm_)
    stat["object references via /alternatename"] = len(alt)
    undef_syms += [t_ for _, t_ in alt]       # weak externals (alias) are written behind the undefined symbols (VC7 link has no /alternatename option)
    stat["object references defined here"] = len(aliases); stat["object references not found"] = len(missing)
    if missing: print("not found:", " ".join(sorted(missing)[:40]), file=sys.stderr)
if DROP:
    norm = lambda n: n if n.startswith("?") else n.lstrip("_@").split("@")[0]
    ndefs = {k: {norm(n): n for n in v} for k, v in memdefs.items()}
    dsym = collections.defaultdict(list)
    _dmem = {x[2]["member"] for x in DROP}
    for r in csv.DictReader(open(os.path.join(G, "output", "delink_sdk", "symbols.csv"))):
        if r["addr"] and r["member"] in _dmem and r["binding"] in ("ext", "common"): dsym[r["member"]].append((int(r["addr"], 16), max(1, int(r["size"] or 1)), r["symbol"]))
    blockers = []
    def symfor(t):
        d_ = dropped(t); r = d_[2]; key = r["member"].replace(" ", "_")
        if r["kind"] == "code":
            n_ = ndefs.get(key, {}).get(norm(r["name"]))
            return (n_, t - d_[0]) if n_ and t == d_[0] else (None, "%s %s+0x%x" % ("static function" if t == d_[0] else "interior", r["name"], t - d_[0]))
        for a_, n_, nm_ in dsym[r["member"]]:
            if a_ <= t < a_ + n_ and nm_ in memdefs.get(key, ()): return nm_, t - a_
        return None, "static/interior data %s" % r["name"]
    for si in range(len(secs)):
        keep = []
        for off, ti, ty in rel[si]:
            site = secs[si][1] + off
            if dropped(site): stat["drop: relocations inside split bytes"] += 1; continue
            if ty == 6 and isinstance(ti, int):
                t = secs[ti][1] + struct.unpack_from("<i", data[si], off)[0]
                if dropped(t):
                    n_, a_ = symfor(t)
                    if n_:
                        struct.pack_into("<i", data[si], off, a_); keep.append((off, n_, 6)); undef_syms.append(n_); stat["drop: abs refs -> symbols"] += 1; continue
                    blockers.append("%08x -> %08x %s" % (site, t, a_))
            keep.append((off, ti, ty))
        rel[si] = keep
    brs = set()
    for r in csv.reader(open(os.path.join(G, "output", "delink", "ghidra_refs.csv"))):          # rel32 calls / jumps from live code into split code
        if len(r) < 3 or r[2] not in ("UNCONDITIONAL_CALL", "UNCONDITIONAL_JUMP", "CONDITIONAL_JUMP"): continue
        try: brs.add((int(r[0], 16), int(r[1], 16)))
        except ValueError: continue
    _st = {x[0] for x in DROP if x[2]["kind"] == "code"}; _tx = secs[0]; _b = _tx[2]   # + byte scan of code Ghidra never decoded: E8 / E9 / 0F 8x onto a split function start
    for i in range(len(_b) - 6):
        k = 1 if _b[i] in (0xE8, 0xE9) else 2 if _b[i] == 0x0F and 0x80 <= _b[i + 1] <= 0x8F else 0
        if k and (_tx[1] + i + k + 4 + struct.unpack_from("<i", _b, i + k)[0]) & 0xFFFFFFFF in _st: brs.add((_tx[1] + i, (_tx[1] + i + k + 4 + struct.unpack_from("<i", _b, i + k)[0]) & 0xFFFFFFFF))
    for site, t in sorted(brs):
        if not dropped(t) or dropped(site) or sec_index(site) is None or sec_index(site) != sec_index(t): continue
        si = sec_index(site); off = site - secs[si][1]; b = secs[si][2]
        fo = off + 1 if b[off] in (0xE8, 0xE9) else off + 2 if b[off] == 0x0F and 0x80 <= b[off + 1] <= 0x8F else None
        if fo is None: blockers.append("%08x -> %08x short branch into split code" % (site, t)); continue
        if bytes(data[si][off:fo + 4]) != b[off:fo + 4]: stat["drop: branch inside patched bytes (dead), skipped"] += 1; continue     # e.g. under a redirect jmp
        if (struct.unpack_from("<i", b, fo)[0] + secs[si][1] + fo + 4) & 0xFFFFFFFF != t: blockers.append("%08x -> %08x branch decode mismatch" % (site, t)); continue
        n_, a_ = symfor(t)
        if not n_: blockers.append("%08x -> %08x %s (rel)" % (site, t, a_)); continue
        struct.pack_into("<i", data[si], fo, a_); rel[si].append((fo, n_, 0x14)); undef_syms.append(n_); stat["drop: rel32 branches -> symbols"] += 1
    for lo, hi, r in DROP:                 # the split bytes leave the image: int3 in code, zero in data
        si = sec_index(lo)
        if si is None or data[si] is None: continue
        o_ = lo - secs[si][1]; n_ = min(hi, secs[si][1] + secs[si][3]) - lo
        data[si][o_:o_ + n_] = (b"\xcc" if r["kind"] == "code" else b"\0") * n_; stat["drop: bytes filled"] += n_
    stat["drop: blockers (live refs without symbol)"] = len(blockers)
    if blockers: open(OUT + ".blockers.txt", "w").write("\n".join(blockers) + "\n"); print("drop blockers: %d (%s.blockers.txt)" % (len(blockers), OUT), file=sys.stderr)

# --drop-app <gen dir> (S5, variant L): the image no longer carries app code that our objects replace, nor ANY .rdata/.data/.bss (tools/globals/globgen.py
# generates the whole data layout, library spans as bytes included).  (a) Code: every function redirected to our objects is filled with int3 behind its
# 5 byte entry jmp, unless live code reaches its interior (branch / absolute word), then it stays (legacy, listed in <out>.dropapp.csv); iterated to a fixed
# point because a kept body keeps its own references.  (b) Data: all three sections are removed from the object; every live relocation that pointed into them
# becomes `_<generated name> + offset` (globals_layout.csv rows tile the sections), relocations sited in them vanish (the generated pointers replace them).
DAPP = []
if "--drop-app" in opt:
    assert "--objs" in opt and IMPSYM, "--drop-app needs --objs and --imports-as-symbols"
    GD = opt["--drop-app"]
    lay = sorted(((int(r["va"], 16), int(r["va"], 16) + int(r["size"]), r["name"], r["kind"]) for r in csv.DictReader(open(os.path.join(GD, "globals_layout.csv")))))
    _lo = [x[0] for x in lay]
    DSEC = {i for i, s_ in enumerate(secs) if s_[0] in (".rdata", ".data", ".bss")}
    # The linker puts the IAT (.idata$5) at the start of .rdata, ahead of the generated rdata.  Keep the generated data at the original offset modulo 16 (SSE tables are
    # read with movapd): a filler contribution of the right size in front of it.  .data / .bss start page aligned and need nothing.
    _iat = struct.unpack_from("<I", d, e + 24 + 96 + 8 * 12 + 4)[0]; _padn = (-_iat) % 16
    if _padn: secs.append([".rdata", 0xF2000000, bytes(_padn), _padn, 0x40000040 | 0x00100000]); data.append(bytearray(_padn)); stat["drop-app: .rdata alignment filler"] = _padn
    fsz = {}
    for l in open(os.path.join(G, "output", "fnsizes.txt")):
        a_, n_, z_ = l.split(); fsz[int(a_, 16)] = int(z_)
    _fs = sorted(fsz); _tend = secs[tsec][1] + secs[tsec][3]
    # extent = up to the next listed function: out-of-line blocks, jump tables and padding behind the sized body belong to it (Ghidra sizes stop at the first ret); live refs into them make the function stay
    cand = {a_: min(_tend, _fs[bisect.bisect_right(_fs, a_)] if bisect.bisect_right(_fs, a_) < len(_fs) else _tend) for a_ in redir_at if fsz.get(a_, 0) >= 6 and sec_index(a_) == tsec}
    # Unlisted functions hide in these gaps (reached by raw function pointers, e.g. 0x500510 from a code_interior data word): an extension stays only if no word anywhere in the image
    # (any offset of the code sections, aligned words of the data sections) outside the function's own extent holds an address inside the gap.
    _w = []
    for s_ in secs:
        if s_[2] is None or s_[0] in (".pad",) or s_[0] == ".rdata" and s_[1] > 0xF0000000: continue
        _bb = s_[2]; _st = 1 if s_[0] in (".text", "_rwcseg") else 4
        for i in range(0, len(_bb) - 3, _st):
            v_ = _bb[i] | _bb[i + 1] << 8 | _bb[i + 2] << 16 | _bb[i + 3] << 24
            if 0x401000 <= v_ < _tend: _w.append((v_, s_[1] + i))
    _w.sort(); _wv = [x[0] for x in _w]
    for a_ in list(cand):
        g0, g1 = a_ + fsz[a_], cand[a_]
        if g1 > g0 and any(not a_ <= _w[k][1] < g1 for k in range(bisect.bisect_left(_wv, g0), bisect.bisect_left(_wv, g1))):
            cand[a_] = g0; stat["drop-app: extensions cancelled (word points into the gap)"] += 1
    _cl = sorted(cand)
    def cand_of(a):            # candidate function whose body (behind the entry jmp) contains a, or None
        i = bisect.bisect_right(_cl, a) - 1
        return _cl[i] if i >= 0 and _cl[i] < a < cand[_cl[i]] else None
    legacy = {}                # function start -> reason
    # branches into interiors: Ghidra flow refs + the E8 / E9 / 0F 8x byte scan (as in --drop-members)
    brs = set()
    for r in csv.reader(open(os.path.join(G, "output", "delink", "ghidra_refs.csv"))):
        if len(r) < 3 or r[2] not in ("UNCONDITIONAL_CALL", "UNCONDITIONAL_JUMP", "CONDITIONAL_JUMP"): continue
        try: brs.add((int(r[0], 16), int(r[1], 16)))
        except ValueError: continue
    _tx = secs[tsec]; _b = _tx[2]
    for i in range(len(_b) - 6):
        k = 1 if _b[i] in (0xE8, 0xE9) else 2 if _b[i] == 0x0F and 0x80 <= _b[i + 1] <= 0x8F else 0
        if k:
            s_ = _tx[1] + i; j_ = bisect.bisect_right(_fs, s_) - 1
            if j_ >= 0 and s_ < _fs[j_] + fsz[_fs[j_]] and (s_, (s_ + k + 4 + struct.unpack_from("<i", _b, i + k)[0]) & 0xFFFFFFFF) not in brs: continue     # inside a sized function body only Ghidra's own flow refs count (else tables / immediates that look like E8 xx xx xx xx)
            brs.add((s_, (s_ + k + 4 + struct.unpack_from("<i", _b, i + k)[0]) & 0xFFFFFFFF))
    brs = [(s_, t_, cand_of(t_)) for s_, t_ in sorted(brs)]; brs = [x for x in brs if x[2] is not None]
    absr = []                  # (site, target) of DIR32 words in kept sections that point into a candidate body
    for si in range(len(secs)):
        if si in DSEC: continue
        for off, ti, ty in rel[si]:
            if ty == 6 and isinstance(ti, int) and ti not in DSEC:
                t = secs[ti][1] + struct.unpack_from("<i", data[si], off)[0]
                if cand_of(t) is not None: absr.append((secs[si][1] + off, t, cand_of(t)))
    for r in csv.DictReader(open(os.path.join(GD, "raw.csv"))):      # reloc sites the generator keeps as raw words (value only, code_interior ...): they still hold the original address
        si_ = sec_index(int(r["site"], 16)); v_ = struct.unpack_from("<I", secs[si_][2], int(r["site"], 16) - secs[si_][1])[0]
        f_ = cand_of(v_)
        if f_ is not None: absr.append((int(r["site"], 16), v_, f_))
    for nm_, a_ in aliases:            # symbols our objects / the generated data name at an address inside a body (labels, unlisted functions): the symbol would point at int3
        f_ = cand_of(a_)
        if f_ is not None: absr.append((0, a_, f_))
    gp = [r for r in csv.DictReader(open(os.path.join(GD, "ptrs.csv")))]
    _by = {n_: a_ for a_, n_ in redir_at.items()}
    for r in gp:                                           # generated pointers `function + addend` (addend != 0): the symbol would resolve to our C
        n_ = r["symbol"]
        if r["addend"] != "0" and n_ in _by and _by[n_] in cand: legacy[_by[n_]] = "generated data points into it (%s+%s)" % (n_, r["addend"])
    changed = True
    while changed:
        changed = False
        for s_, t_, f in brs + absr:
            if f in legacy: continue
            sf = cand_of(s_)
            if sf is not None and sf not in legacy: continue          # sited in a body that is filled: dead
            legacy[f] = "interior ref from %08x to %08x" % (s_, t_); changed = True
    for a_ in sorted(cand):
        if a_ in legacy: continue
        hi = cand[a_]; off = a_ - secs[tsec][1]
        rel[tsec] = [r_ for r_ in rel[tsec] if not off + 5 <= r_[0] < hi - secs[tsec][1]]
        data[tsec][off + 5:hi - secs[tsec][1]] = b"\xcc" * (hi - a_ - 5); stat["drop-app: code bytes filled"] += hi - a_ - 5; stat["drop-app: functions dropped"] += 1
        DAPP.append((a_, hi - a_, redir_at[a_], "dropped", ""))
    for a_ in sorted(legacy): DAPP.append((a_, cand[a_] - a_, redir_at[a_], "legacy", legacy[a_]))
    stat["drop-app: functions kept (interior refs)"] = len(legacy)
    blockers = []
    for si in range(len(secs)):
        keep = []
        for off, ti, ty in rel[si]:
            if si in DSEC: stat["drop-app: relocations sited in dropped data"] += 1; continue
            if ty == 6 and isinstance(ti, int) and ti in DSEC:
                t = secs[ti][1] + struct.unpack_from("<i", data[si], off)[0]
                j = bisect.bisect_right(_lo, t) - 1
                if j < 0 or t >= lay[j][1] or lay[j][3] == "pe": blockers.append("%08x -> %08x (data, %s)" % (secs[si][1] + off, t, "no layout row" if j < 0 or t >= lay[j][1] else "pe span")); keep.append((off, ti, ty)); continue
                struct.pack_into("<i", data[si], off, t - lay[j][0]); keep.append((off, "_" + lay[j][2], 6)); undef_syms.append("_" + lay[j][2]); stat["drop-app: data refs -> generated symbols"] += 1; continue
            keep.append((off, ti, ty))
        rel[si] = keep
    stat["drop-app: blockers"] = len(blockers)
    open(OUT + ".dropapp.csv", "w").write("va,size,name,status,reason\n" + "".join("%08x,%d,%s,%s,%s\n" % x for x in DAPP))
    if blockers: open(OUT + ".blockers.txt", "w").write("\n".join(blockers) + "\n"); print("drop-app blockers: %d (%s.blockers.txt)" % (len(blockers), OUT), file=sys.stderr)
# symbols: section symbols first (index 2 * i: symbol + aux), then externals
strtab = bytearray(4)
def name8(s):
    b = s.encode()
    if len(b) <= 8: return b.ljust(8, b"\0")
    o = len(strtab); strtab.extend(b + b"\0"); return b"\0\0\0\0" + struct.pack("<I", o)
syms = bytearray()
KEEP = [i for i in range(len(secs)) if i not in DSEC]           # --drop-app removes the data sections; section symbol k sits at index 2 * k
NEWI = {i: k for k, i in enumerate(KEEP)}
for i, s in enumerate(secs):
    if i in DSEC: continue
    syms += name8(s[0]) + struct.pack("<IhHBB", 0, NEWI[i] + 1, 0, 3, 1)
    syms += struct.pack("<IHHIHBB", s[3], len(rel[i]) if len(rel[i]) < 0xFFFF else 0xFFFF, 0, 0, 0, 0, 0).ljust(18, b"\0")
seen = collections.Counter()
# names: src/symbols.txt (ExportSymbols.java: functions and data labels; the same table the overlay resolves with, so a name means the same
# address in both builds), then the newer output/fnsizes.txt names that symbols.txt does not have
ext = [("_mfl_entry", entry)]
known = set()
for l in open(os.path.join(G, "src", "symbols.txt")):
    n, a = l.split(); a = int(a, 16)
    if sec_index(a) is not None: ext.append(("_" + n, a)); known.add(n)
for l in open(os.path.join(G, "output", "fnsizes.txt")):
    a, n, _ = l.split()
    if n not in known: ext.append(("_" + n, int(a, 16)))
# --rename <file>: lines "name new_name" (e.g. "car_update car_update_orig"): the original keeps working under the new name, so a recompiled
# object that defines the old name replaces it at link time (references inside the image are section relative and still reach the original)
REN = dict(l.split()[:2] for l in open(opt["--rename"]) if l.strip() and not l.startswith("#")) if "--rename" in opt else {}
ext = [("_" + REN[n[1:]], a) if n[1:] in REN else (n, a) for n, a in ext]
ext = [(n + "_orig", a) if n in objdef else (n, a) for n, a in ext]       # the object's definition wins, the original stays reachable as name_orig
ext += [(n, a) for n, a in aliases if n not in {x for x, _ in ext}]
nsym_ext = 0
for n, a in ext:
    i = sec_index(a)
    if i is None or i in DSEC or (DROP and dropped(a)): continue           # .bss symbols too; nothing inside split bytes (globals without an initial value)
    seen[n] += 1
    if seen[n] > 1: n = "%s_%x" % (n, a)
    syms += name8(n) + struct.pack("<IhHBB", a - secs[i][1], NEWI[i] + 1, 0x20, 2, 0); nsym_ext += 1
symidx = {}
for n in sorted(set(undef_syms)):
    symidx[n] = len(syms) // 18; syms += name8(n) + struct.pack("<IhHBB", 0, 0, 0x20, 2, 0)
for n, t_ in (alt if "--objs" in opt else []):      # IMAGE_SYM_CLASS_WEAK_EXTERNAL, aux: TagIndex = target, IMAGE_WEAK_EXTERN_SEARCH_ALIAS
    syms += name8(n) + struct.pack("<IhHBB", 0, 0, 0, 105, 1) + struct.pack("<II", symidx[t_], 3).ljust(18, b"\0")
for i in KEEP:         # relocation counts in the section symbols' aux records (redirects added some)
    struct.pack_into("<H", syms, 18 * (2 * NEWI[i] + 1) + 4, min(len(rel[i]), 0xFFFF))
nsyms = len(syms) // 18
# layout: header, section table, per section raw data + relocations, symbols, strings
hdr_end = 20 + 40 * len(KEEP)
body, shdr, p = bytearray(), bytearray(), hdr_end
for i, s in enumerate(secs):
    if i in DSEC: continue
    raw = bytes(data[i]) if data[i] is not None else b""
    rp = p if raw else 0; p += len(raw); body += raw
    flags = s[4]
    rels = sorted(rel[i], key=lambda r_: r_[0])
    rl = b"".join(struct.pack("<IIH", off, symidx[ti] if isinstance(ti, str) else 2 * NEWI[ti], ty) for off, ti, ty in rels)
    nrel = len(rels)
    if nrel >= 0xFFFF:         # IMAGE_SCN_LNK_NRELOC_OVFL: the real count is in the VirtualAddress of the first relocation
        rl = struct.pack("<IIH", nrel + 1, 0, 0) + rl; flags |= 0x01000000; nrel = 0xFFFF
    rptr = p if rels else 0; p += len(rl); body += rl
    shdr += name8(s[0]) + struct.pack("<IIIIIIHHI", 0, 0, s[3], rp, rptr, 0, nrel, 0, flags)
struct.pack_into("<I", strtab, 0, len(strtab))
coff = struct.pack("<HHIIIHH", 0x14C, len(KEEP), 0, hdr_end + len(body), nsyms, 0, 0) + shdr + body + syms + strtab
open(OUT, "wb").write(coff)
print("%s: %d sections, %d relocations, %d symbols (%d named functions and data labels), %d bytes" % (OUT, len(secs), stat["relocated"], nsyms, nsym_ext, len(coff)))
for k, v in sorted(stat.items()):
    if k != "relocated": print("  %-28s %d" % (k, v))
