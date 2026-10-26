"""diffemu core: build our function (COFF of the game compiler) and run it against the ORIGINAL bytes of MFL.exe in two Unicorn instances with identical
seeded memory, every callee / import stubbed.  Library code; driver = diffemu.py, calibration = calibrate.py, method = README "diffemu"."""
import array, csv, hashlib, os, random, re, struct, sys, time
HERE = os.path.dirname(os.path.abspath(__file__)); TOOLS = os.path.dirname(HERE); ROOT = os.path.dirname(TOOLS)
sys.path.insert(0, TOOLS)
import cmpfn
from unicorn import *
from unicorn.x86_const import *
import capstone, pefile
from capstone import x86_const as CX

EXE = cmpfn.EXE
pe = pefile.PE(EXE, fast_load=True); pe.parse_data_directories(directories=[1])
BASE = pe.OPTIONAL_HEADER.ImageBase
IMG = pe.get_memory_mapped_image(); IMGEND = BASE + ((len(IMG) + 0xfff) & ~0xfff)
WSEC = [(BASE + s.VirtualAddress, BASE + s.VirtualAddress + ((max(s.Misc_VirtualSize, s.SizeOfRawData) + 0xfff) & ~0xfff)) for s in pe.sections if s.Characteristics & 0x80000000]
STACK, STK = 0x60000000, 0x40000      # emulator memory map; everything else is lazy
OURS, SCR, ARGBUF, STUBS, IMPSTUB, RET = 0x70000000, 0x71000000, 0x72000000, 0x73000000, 0x74000000, 0x7f000000
FPSRC = STUBS + 0x8000
FPPAGE = 0x76000000      # x87 readout page (J-spill 2026-10-22): mapped on its own, never written by the code under test, never in a compared region (FPOUT used to be STUBS+0x8100: its fstp counted as a function write)
FPOUT = FPPAGE
LITSZ = 256
MAXINS = 300000
WIN = 0x200      # default size of the memory window behind a pointer argument


FRAMEWIN = 8      # words (32 bytes) compared behind every pointer argument that points into the stack
def fp_depth(uc, top):
    """values on the x87 stack = (8 - TOP) & 7.  ponytail: a pop of an empty stack (callee the stub does not model as a float returner) reads as stale entries holding register garbage: that is a harness model gap (float_returners), not a C difference; FPTAG of Unicorn is not usable for this (empty reads 01 or 11)"""
    return -top & 7
def u32(v): return v & 0xffffffff
def sha(*parts):
    h = hashlib.sha1()
    for p in parts: h.update(p if isinstance(p, bytes) else str(p).encode())
    return h.hexdigest()[:16]
_NAN = re.compile(rb"(?=[\x80-\xff][\x7f\xff])")
def defloat(b):
    """random dwords that are float NaN / Inf are cleared (x87 NaN payloads depend on operand order: a false DIFF on a commutative a + b)"""
    a = array.array("I", b); a = array.array("I", [v ^ 0x40000000 if v & 0x7f800000 == 0x7f800000 else v for v in a]); b = bytearray(a.tobytes())
    for m in _NAN.finditer(bytes(b)): b[m.start() + 1] ^= 0x40      # unaligned float reads (a pointer argument that is not 4 byte aligned) see NaNs too
    return bytes(b)
def isstack(v): return STACK <= v < STACK + STK
def isro(v): return BASE <= v < IMGEND and not any(a <= v < z for a, z in WSEC)


def _out(name):
    """output/<name>, or $MFL_OUTPUT_OVERRIDE/<name> when that exists (try regenerated data without touching the live files)"""
    o = os.environ.get("MFL_OUTPUT_OVERRIDE")
    if o and os.path.exists(os.path.join(o, name)): return os.path.join(o, name)
    return os.path.join(ROOT, "output", name)


# ---------------------------------------------------------------------------------------------------------- data tables
class Tables:
    def __init__(s):
        s.sym = cmpfn._symtab(); s.rev = {}
        for n, vs in s.sym.items():
            for v in vs: s.rev.setdefault(v, n)
        s.size = {}
        for l in open(os.path.join(ROOT, "output", "fnsizes.txt")):
            p = l.split()
            if len(p) == 3: s.size[int(p[0], 16)] = int(p[2])
        s.reg = {}      # va -> (register inputs, ret N, stack types)
        for r in csv.DictReader(open(os.path.join(ROOT, "output", "regsigs.csv"))):
            s.reg[int(r["addr"], 16)] = ([x for x in r["regs"].split("|") if x], int(r["ret_n"] or 0), [x for x in r["stack"].split(";") if x])
        try:      # register inputs of call targets INSIDE a multi-function gap_ item (tools/interior_reginputs.py; regsigs.csv has no row: course_city_cb7 lost `mov esi,1` before func_0x00455dd0)
            for r in csv.DictReader(open(_out("reginputs_interior.csv"))):
                a = int(r["addr"], 16)
                if a not in s.reg: s.reg[a] = ([x for x in r["regs"].split("|") if x], int(r["ret_n"] or 0), [])
        except OSError: pass
        s.sig = {}      # va -> (conv, stack dwords, return type)
        s.stale = set()      # sigs.txt carries a `reg_e??` parameter that output/regsigs.csv does not know: the decls.h parameter list has one parameter too many (the stack words are the real arguments)
        for r in csv.DictReader(open(os.path.join(ROOT, "output", "sigs_regfix.csv"))): s.stale.update([int(r["addr"], 16)] if int(r["addr"], 16) not in s.reg else [])      # reg_ parameters tools/sig_regcheck.py dropped from sigs.txt (decls.h, the Ghidra export, still lists them)
        for l in open(os.path.join(ROOT, "output", "sigs.txt")):
            p = l.rstrip("\n").split("|")
            if len(p) >= 7:
                if re.search(r"(?:^|;)[^;]*:reg_e\w\w:", p[5]) and int(p[1], 16) not in s.reg: s.stale.add(int(p[1], 16))
                n = sum(int(x.split(":")[-1]) for x in p[5].split(";") if "Stack" in x); s.sig[int(p[1], 16)] = (p[2], (n + 3) // 4, p[3])
        s.decl = {}     # va -> (return type, parameter text)
        for l in open(os.path.join(ROOT, "output", "decls.h")):
            m = re.match(r"/\*([0-9a-f]{8})\*/ extern (.*?)\b\w+\((.*)\);", l)
            if m: s.decl[int(m.group(1), 16)] = (m.group(2), m.group(3))
        try:      # names/MFL_lib_protos.txt (tools/lib_protos.py): real prototypes of Lua / RenderWare callees win over the K&R `()` of decls.h
            for l in open(os.path.join(ROOT, "names", "MFL_lib_protos.txt")):
                m = re.match(r"/\*([0-9a-f]{8})\*/ extern (.*?)\b\w+\((.*)\);", l)
                if m: s.decl[int(m.group(1), 16)] = (m.group(2), m.group(3))
        except OSError: pass
        s.convfix = {}  # function -> cdecl | stdcall: names/MFL_conv_fix.txt corrects the Ghidra convention of sigs.txt / decls.h (the build applies it; so must the callee stubs)
        for l in open(os.path.join(ROOT, "names", "MFL_conv_fix.txt")):
            q = l.split()
            if len(q) == 2 and not l.startswith("#"): s.convfix[q[0]] = q[1]
        s.gt = {}       # va -> stack dwords the ORIGINAL call sites pass (output/cdecl_args.txt with >= 2 agreeing sites, else output/call_pushmin.txt of >= 3 sites): arg count of a callee declared `()`
        for fn, ok in (("call_pushmin.txt", lambda q: int(q[2]) >= 3), ("cdecl_args.txt", lambda q: int(q[2]) >= 2)):      # cdecl_args with one site: a batched `add esp` of the next calls inflates it (career_record_ptr: 17)
            try:
                for l in open(os.path.join(ROOT, "output", fn)):
                    q = l.split()
                    if len(q) >= 3 and ok(q): s.gt[int(q[0], 16)] = int(q[1])
            except OSError: pass
        s.oslots = {}   # va -> (stack slots the ORIGINAL body reads / rewrites, ret N, tail-jump targets, w<N> rewritten-slot count): output/orig_stackargs.txt (tools/origargs.py)
        try:
            for l in open(_out("orig_stackargs.txt")):
                q = l.split()
                s.oslots[int(q[0], 16)] = (int(q[1]), int(q[2]), [] if q[3] == "-" else [int(x, 16) for x in q[3].split(",")], max([int(x[1:]) for x in q[4].split(",") if re.fullmatch(r"w\d+", x)] or [0]), bool({"dyn"} & set(q[4].split(","))), q[1] == "0" and q[2] == "0" and q[4] == "-" and q[3] == "-")      # [5] = exact0: the body provably never touches a stack slot (no dyn/addr/ijmp/w flag, no tail jump, plain ret): the call-site push count is compiler scheduling only (race_lose_banner_draw: intro_banner_get_x() is called with the next call's hoisted pushes live). Only `dyn` (untracked esp) makes the slot count unusable: `addr` (an argument's address is taken) keeps it a proven lower bound (ai_drive_a: 3 of 4 slots, the call without arguments was invisible while it was ignored)
        except OSError: pass
        s.proto = {}    # import name -> number of arguments
        for l in open(os.path.join(ROOT, "names", "MFL_import_protos.txt")):
            m = re.match(r"extern __declspec\(dllimport\) \w[\w ]*?(\w+)\((.*)\);", l)
            if m: s.proto[m.group(1)] = 0 if m.group(2).strip() in ("", "void") else m.group(2).count(",") + 1
        s.iat = {}
        for e in getattr(pe, "DIRECTORY_ENTRY_IMPORT", []):
            for i in e.imports: s.iat[i.name.decode() if i.name else "ord%d" % i.ordinal] = i.address
        s.structs = {}  # struct name -> size (names/MFL_structs*.txt)
        for f in sorted(os.listdir(os.path.join(ROOT, "names"))):
            if f.startswith("MFL_structs") and f.endswith(".txt"):
                for l in open(os.path.join(ROOT, "names", f)):
                    p = l.strip().split(",")
                    if p[0] == "struct" and len(p) >= 3:
                        try: s.structs[p[1]] = int(p[2], 0)
                        except ValueError: pass
        s.hash = sha(*[open(os.path.join(ROOT, "output", f), "rb").read() for f in ("regsigs.csv", "sigs.txt", "decls.h", "fnsizes.txt")],
                     *[open(_out(f), "rb").read() for f in ("orig_stackargs.txt", "reginputs_interior.csv") if os.path.exists(_out(f))],      # the callee tables below are part of the verdict cache key
                     open(os.path.join(ROOT, "names", "MFL_import_protos.txt"), "rb").read(), open(os.path.join(ROOT, "names", "MFL_conv_fix.txt"), "rb").read(), *[open(os.path.join(ROOT, "output", f), "rb").read() for f in ("call_pushmin.txt", "cdecl_args.txt")], open(os.path.join(ROOT, "names", "MFL_lib_protos.txt"), "rb").read() if os.path.exists(os.path.join(ROOT, "names", "MFL_lib_protos.txt")) else b"", *[open(os.path.join(ROOT, "src", f), "rb").read() for f in ("symbols.txt", "aliases.txt")],
                     *[open(p, "rb").read() for p in [os.path.join(os.environ.get("T2_OVERLAY_DIR", "/nonexistent"), "aliases.txt")] if os.path.exists(p)])

_T = []
def T():
    if not _T: _T.append(Tables())
    return _T[0]

_REAL = set()
def real_helpers():
    """pure code helpers (x87 / 64 bit arithmetic / stack probe) run for real, never stubbed"""
    if not _REAL:
        for n in ("_ftol", "_ftol2", "__ftol", "__allmul", "__alldiv", "__allrem", "__aulldiv", "__aullrem", "__aulldvrm", "__aullshr", "__allshl", "__allshr", "__chkstk", "_chkstk",
                  "_CIsin", "_CIcos", "_CIasin", "_CIacos", "_CIsqrt", "_CIpow_pentium4", "_CIatan_pentium4", "_CIexp_pentium4"):
            _REAL.update(T().sym.get(n, ()))
    return _REAL


# ---------------------------------------------------------------------------------------------------------- C signatures
SCAL = {"int": "i4", "uint": "i4", "long": "i4", "ulong": "i4", "dword": "i4", "undefined4": "i4", "short": "i2", "ushort": "i2", "word": "i2", "undefined2": "i2", "wchar16": "i2",
        "wchar_t": "i2", "char": "i1", "uchar": "i1", "byte": "i1", "bool": "i1", "undefined": "i1", "undefined1": "i1", "float": "f", "double": "d", "float10": "d", "unkbyte10": "d",
        "longlong": "ll", "ulonglong": "ll", "undefined8": "ll", "__int64": "ll", "void": "v"}
_TD = {}

def typedefs(d):
    if d not in _TD:
        m = {}
        for h in ("ghidra_types.h", "prelude.h"):
            p = os.path.join(d, h)
            if not os.path.exists(p): continue
            for l in open(p, errors="replace"):
                r = re.match(r"typedef\s+(?:struct\s+\w+\s+\w+\s*,\s*\*(\w+)|(.+?)\s+(\w+))\s*;", l.strip())
                if r: m[r.group(1) or r.group(3)] = "void *" if r.group(1) else r.group(2)
        _TD[d] = m
    return _TD[d]

def tclass(t, td, depth=0):
    """C type text -> (class, pointee): i1 i2 i4 p f d ll v, or ? (unsupported, e.g. struct by value)"""
    t = re.sub(r"\b(const|volatile|unsigned|signed|__\w+)\b", "", t).strip()
    if "*" in t: return "p", t[:t.index("*")].replace("struct", "").replace("enum", "").strip()
    if t.startswith("enum"): return "i4", ""
    if t.startswith(("struct", "union")): return "?", ""
    if t in SCAL: return SCAL[t], ""
    if t in td and depth < 8: return tclass(td[t], td, depth + 1)
    return ("i4", "") if t == "" or t[:1].isupper() else ("?", "")

class Sig:
    def __init__(s, ret, conv, params):
        s.ret, s.conv, s.params = ret, conv, params      # params [(class, pointee, text)]
        nreg = {"__thiscall": 1, "__fastcall": 2}.get(conv, 0)
        s.regargs, s.stackargs = params[:nreg], params[nreg:]
    def kind(s): return "".join(p[0][0] for p in s.params)

DEF = re.compile(r"^(?!extern|/\*|typedef|static)([A-Za-z_][\w \*]*?)\b(\w+)\((.*)\)$")      # as shadowgen2.py / shadow_cache.py

def defs(path):
    """{function: (definition text, signature match)} of a C file"""
    L = open(path, errors="replace").read().split("\n"); out = {}; i = 0
    for k in range(len(L) - 1, 0, -1):      # Ghidra layout `undefined4 __cdecl` / `name` / `(args)`: return type, name and parameter list may sit on separate lines; joined into one logical line (was: KeyError for ~60 t2_auto functions)
        a = L[k - 1]
        if re.match(r"^[A-Za-z_][\w \*]*[\w\*]$", a) and not re.match(r"^(extern|typedef|static|return|else|do|goto|case)\b", a):
            if re.match(r"^\s*\(", L[k]) or re.match(r"^[A-Za-z_]\w*\(", L[k]): L[k - 1:k + 1] = [a + (L[k].lstrip() if L[k].lstrip().startswith("(") else " " + L[k])]
    while i < len(L):
        n_ = 1      # rule `sigjoin`: a signature split over several lines (`f(int a,` / `int b)`) is joined into one logical line
        if re.match(r"^(?!extern|/\*|typedef|static)[A-Za-z_][\w \*]*?\b\w+\(", L[i]) and L[i].count("(") > L[i].count(")"):
            while i + n_ < len(L) and "".join(L[i:i + n_]).count("(") > "".join(L[i:i + n_]).count(")") and n_ < 12: n_ += 1
            if n_ > 1: L[i:i + n_] = [" ".join(x.strip() for x in L[i:i + n_])]
        m = DEF.match(L[i]); k = i + 1
        while m and k < len(L) and L[k].strip() == "": k += 1      # autocompile output has a blank line before the brace
        if m and k < len(L) and L[k].startswith("{"):
            j, d = k, 0
            while j < len(L):
                d += L[j].count("{") - L[j].count("}"); j += 1
                if d == 0: break
            out[m.group(2)] = ("\n".join(x.rstrip() for x in L[i:j]), m); i = j; continue
        i += 1
    return out

def parse_sig(m, td):
    """-> (Sig, None) or (None, reason)"""
    pre, ps = m.group(1), m.group(3).strip()
    conv = next((c for c in ("__stdcall", "__fastcall", "__thiscall") if c in pre), "__cdecl")
    ret = re.sub(r"\b__(stdcall|cdecl|fastcall|thiscall)\b", "", pre).strip()
    params = []
    if ps not in ("", "void"):
        if "(" in ps or "..." in ps or "[" in ps: return None, "parameters not expressible (function pointer / varargs / array)"
        for p in ps.split(","):
            p = p.strip(); r = re.match(r"^(.*?[\s\*])(\w+)$", p); ty = r.group(1) if r else p
            c, pt = tclass(ty, td)
            if c == "?": return None, "unsupported parameter type " + p
            params.append((c, pt, p))
    rc = "v" if ret in ("void", "undefined", "") else tclass(ret, td)[0]
    return Sig("i4" if rc == "?" else rc, conv, params), None


# ---------------------------------------------------------------------------------------------------------- COFF + relocation
class Coff:
    def __init__(s, path):
        d = s.d = open(path, "rb").read()
        machine, nsec, ts, symoff, nsym, opt, ch = struct.unpack_from("<HHIIIHH", d, 0)
        strtab = symoff + nsym * 18
        def nm(raw):
            if raw[:4] == b"\0\0\0\0":
                o = struct.unpack_from("<I", raw, 4)[0]; return d[strtab + o:d.index(b"\0", strtab + o)].decode()
            return raw.rstrip(b"\0").decode()
        s.secs = []
        for k in range(nsec):
            o = 20 + opt + k * 40
            rawsz, rawptr, relptr = struct.unpack_from("<III", d, o + 16)
            s.secs.append((rawsz, rawptr, relptr, struct.unpack_from("<H", d, o + 32)[0], struct.unpack_from("<I", d, o + 36)[0]))
        s.syms = {}; s.byname = {}; i = 0
        while i < nsym:
            o = symoff + i * 18
            n = nm(d[o:o + 8]); value, secnum, typ, cls, aux = struct.unpack_from("<IhHBB", d, o + 8)
            s.syms[i] = (n, value, secnum); s.byname.setdefault(n, (secnum, value)); i += 1 + aux

    def function(s, sym):
        f = next(((sn, v) for n, (sn, v) in s.byname.items() if sn > 0 and (n == "_" + sym or re.fullmatch(r"[_@]%s@\d+" % re.escape(sym), n))), None)
        if not f: raise KeyError("symbol %s not in object" % sym)
        sn, val = f; rawsz, rawptr, relptr, nrel, _ = s.secs[sn - 1]
        rel = []
        for k in range(nrel):
            rva, si, rt = struct.unpack_from("<IIH", s.d, relptr + k * 10)
            if rt in (6, 0x14) and val <= rva < rawsz: rel.append((rva - val, rt, si, struct.unpack_from("<i", s.d, rawptr + rva)[0]))
        return s.d[rawptr + val:rawptr + rawsz], rel, sn, val


class Unresolved(Exception): pass

_LS = {}
def libsyms():
    """name -> address of output/*_libsyms.csv (rw_libsyms, crt_libsyms ...)"""
    if not _LS:
        import glob
        for fn in sorted(glob.glob(os.path.join(ROOT, "output", "*_libsyms.csv"))):
            for l in open(fn):
                q = l.strip().split(",")
                if len(q) >= 2 and q[1] != "00000000":
                    try: _LS.setdefault(q[0], int(q[1], 16))
                    except ValueError: pass
        try:      # RW library functions the byte matcher found (output/rw_libmatch.csv `va,name,..`) that rw_libsyms.csv (relocation derived) lacks: _rwD3D9SetVertexShaderConstant
            for l in open(os.path.join(ROOT, "output", "rw_libmatch.csv")):
                q = l.split(",")
                if len(q) >= 2 and re.fullmatch(r"[0-9a-f]{8}", q[0]) and re.fullmatch(r"\w+", q[1]): _LS.setdefault("_" + q[1], int(q[0], 16))
        except (OSError, ValueError): pass
    return _LS

def impstub(name): return IMPSTUB + 16 * sorted(T().iat).index(name)

def our_code(obj, sym, base=OURS, extra=None, lits=None, lit_at=None):
    """-> (code relocated for OURS, literal / static blob for SCR, relocation targets): relocations resolved like cmpfn.reloc_targets (symbols.txt + aliases.txt, IAT, jump tables
    inside the function, literals / constants / file local data copied to SCR).  Raises Unresolved.
    cluster mode (cluster.py): base = where this function is placed, extra = {symbol: address} of the other functions placed next to it (they win over the symbol table), lits / lit_at = shared literal pool"""
    co = Coff(obj); tb = T(); own = lits is None
    code, rel, fsn, fval = co.function(sym)
    code = bytearray(code.rstrip(b"\xcc") if rel else code.rstrip(b"\x90\xcc"))
    if own: lits = bytearray(); lit_at = {}
    adp, tail = {}, bytearray()      # static naked F_r adapters of the same object (bridgegen.gen_adapter): code appended behind the function; they call the ORIGINAL callee address, which is stubbed (register inputs recorded like on the original side)
    for off, rt, si, add in rel:
        if off + 4 > len(code): continue
        n, sval, sn = co.syms[si]; un = cmpfn._undecorate(n)
        if n.startswith("__imp_"):
            nm = cmpfn._undecorate(n[6:]); t = tb.iat.get(nm) or tb.iat.get(nm.lstrip("_"))
            if t is None: raise Unresolved("import " + n)
        elif extra and un in extra: t = extra[un] + add
        elif un.endswith("_r") and sn > 0 and (co.secs[sn - 1][4] & 0x20) and not (sn == fsn and sval == fval):
            if un not in adp:
                tail += b"\0" * ((-len(tail)) % 16); adp[un] = base + ((len(code) + 15) & ~15) + len(tail)
                ac, _ = our_code(obj, un, adp[un], extra, lits, lit_at); tail += ac
            t = adp[un] + add
        elif sn == fsn and sn > 0: t = base + (sval - fval) + add
        elif sn > 0 and not (co.secs[sn - 1][4] & 0x20) and (n.startswith(("??_C@", "_real@", "__real@", "__xmm@", "$S")) or (un not in tb.sym and "_" + un not in tb.sym)):
            sz, sp = co.secs[sn - 1][0], co.secs[sn - 1][1]; key = (sn, sval + add)
            if key not in lit_at:
                lit_at[key] = SCR + len(lits); lits += (co.d[sp + sval + add:sp + min(sz, sval + add + LITSZ)] if sp else b"").ljust(LITSZ, b"\0")
            t = lit_at[key]
        else:
            vas = tb.sym.get(un) or tb.sym.get("_" + un)
            if vas: t = sorted(vas)[0] + add
            elif (fm := re.fullmatch(r"_?(?:func_0x|FUN_)([0-9a-f]{8})", n)): t = int(fm.group(1), 16) + add      # decompiler name of an unnamed function: the address is the name (build_overlay.py does the same); stubbed like every callee
            elif (lv := libsyms().get(n) or libsyms().get(un) or libsyms().get("_" + un)): t = lv + add      # SDK library / CRT symbol (output/*_libsyms.csv: derived from the relocations of byte matched library functions): the original's address
            elif un in tb.iat: t = impstub(un) + add      # direct `call _WaitForSingleObject@8` of an import: the same import stub the original's `call [IAT]` reaches (name + arguments traced identically on both sides)
            else: raise Unresolved("symbol " + n)
        struct.pack_into("<I", code, off, u32(t if rt == 6 else t - (base + off + 4)))
    if tail: code += b"\0" * ((-len(code)) % 16) + tail
    return bytes(code), bytes(lits)


# ---------------------------------------------------------------------------------------------------------- disassembly
CS = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); CS.detail = True

def scan(read, entry, lo, hi):
    """recursive descent from entry inside [lo, hi): (basic block leaders, external call / jmp targets).  read(addr, n) -> bytes"""
    lead = {entry}; todo = [entry]; seen = set(); ext = set()
    while todo:
        a = todo.pop()
        while lo <= a < hi and a not in seen:
            ins = next(CS.disasm(read(a, 16), a, 1), None)
            if ins is None: break
            seen.add(a); g = ins.groups; term = False; call = CX.X86_GRP_CALL in g
            if CX.X86_GRP_JUMP in g or call:
                op = ins.operands[0] if ins.operands else None
                if op is not None and op.type == CX.X86_OP_IMM:
                    t = op.imm & 0xffffffff
                    if lo <= t < hi:
                        if not call: lead.add(t); todo.append(t)
                    else: ext.add(t)
                elif op is not None and op.type == CX.X86_OP_MEM and not call and op.mem.index and op.mem.scale == 4 and op.mem.disp > 0:      # jump table
                    t = op.mem.disp & 0xffffffff
                    for k in range(256):
                        e = struct.unpack("<I", read(t + 4 * k, 4).ljust(4, b"\0"))[0]
                        if not (lo <= e < hi): break
                        lead.add(e); todo.append(e)
                if not call:
                    term = ins.mnemonic == "jmp"
                    if not term: lead.add(a + ins.size)
            elif CX.X86_GRP_RET in g or ins.mnemonic in ("int3", "hlt", "ud2"): term = True
            a += ins.size
            if term: break
    return lead, ext

def img_read(a, n): return IMG[a - BASE:a - BASE + n]


# ---------------------------------------------------------------------------------------------------------- stubs
_KEEP = {}
def keeps_reg(va, reg):
    """True when the original function at va provably never writes reg (ecx / edx), see tools/pseudofix.py may_write; any failure = False (the stub clobbers it as before)"""
    k = (va, reg)
    if k not in _KEEP:
        try:
            sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
            import pseudofix; _KEEP[k] = not pseudofix.may_write("%08x" % va, reg)
        except Exception: _KEEP[k] = False
    return _KEEP[k]
class Callee:
    def __init__(s, va, name, pop, nargs, regs, ret, known=True, infer=False): s.va, s.name, s.pop, s.nargs, s.regs, s.ret, s.known, s.infer = va, name, pop, nargs, regs, ret, known, infer; s.lim = []

SDK_SZ = {"RwV3d": 12, "RwV2d": 8, "RwMatrix": 64, "RwRGBA": 4, "RwSphere": 16, "RwBBox": 24, "RwLine": 24, "RwPlane": 16, "RwTexCoords": 8, "RwSurfaceProperties": 12}
def ptr_limits(params):
    """per argument dword: the size in words of the object a typed pointer parameter points to (`struct RwV3d *` = 3), else 0 = unknown / an array.  The frame window (FRAMEWIN) must not look behind
    such an object: RwV3dNormalize(&RStack_18, ..) only reads / writes 12 bytes, the 3 floats declared behind the vector are other locals (course_city_cb1 was a false 'frame object' DIFF)"""
    out = []
    for p in params:
        m = re.match(r"^(?:const\s+)?(?:struct\s+)?(\w+)\s*\*\s*\w*$", p.strip())
        sz = (SDK_SZ.get(m.group(1)) or T().structs.get(m.group(1))) if m else None
        out += [((sz + 3) // 4 if sz else 0)] * (2 if re.search(r"\bdouble\b", p) and "*" not in p else 1)
    return out

_AL = {}
def asm_limits(va, nargs):
    """per stack argument: the size in words that the ORIGINAL callee reads / writes through the pointer it loads from that argument slot (0 = unknown).  Linear scan of the callee body: `mov r,[esp+4+4k]`
    makes r an alias; every `[r+disp]` access (no index register) extends the object to disp+size; anything else that touches an alias (copy to memory, push, arithmetic, lea, passing it in
    eax / ecx / edx to a call, an indexed access) gives up for that argument (unknown = the full FRAMEWIN).  Used for callees without a typed prototype (grid_position -> dot_product_2d_clamped: reads
    2 words of the vector, the window compared 8)"""
    key = (va, nargs)
    if key in _AL: return _AL[key]
    BAD = 1 << 30; ext = [0] * nargs; al = {}; d = 0; ebp_d = None; out = [0] * nargs
    sz = T().size.get(va, 0x400); name = lambda r: CS.reg_name(r)
    try:
        jt = {}; reach = True      # alias registers at the branch targets: the code behind a `ret` / `jmp` starts with the aliases of the jump that reaches it (`mov eax,[esp+4]; test eax,eax; jne L; mov eax,imm; ret; L: mov ecx,[eax]`: particles_set_color read 1 word, the scan saw none)
        for ins in CS.disasm(img_read(va, sz), va):
            mn = ins.mnemonic; ops = ins.operands; mems = [o for o in ops if o.type == CX.X86_OP_MEM]
            if ins.address in jt: al = {**jt.pop(ins.address), **al} if reach else dict(jt.pop(ins.address)); reach = True
            if ops and ops[0].type == CX.X86_OP_IMM and (mn == "jmp" or CX.X86_GRP_JUMP in ins.groups) and ins.address < ops[0].imm < va + sz: jt[ops[0].imm] = {**jt.get(ops[0].imm, {}), **al}
            if mn in ("ret", "retn"): d = 0; reach = False; continue
            if mn in ("and", "leave") and ops and ops[0].type == CX.X86_OP_REG and name(ops[0].reg) == "esp": ext = [BAD] * nargs; break
            slot = None
            for o in mems:
                b = name(o.mem.base) if o.mem.base else ""; x = o.mem.index
                if b == "esp" or (b == "ebp" and ebp_d is not None):
                    off = o.mem.disp - (d if b == "esp" else ebp_d)
                    if x == 0 and off >= 4 and (off - 4) % 4 == 0 and (off - 4) // 4 < nargs: slot = (off - 4) // 4
                    continue
                if b in al:
                    k = al[b]
                    if x: ext[k] = BAD
                    else: ext[k] = max(ext[k], o.mem.disp + max(o.size, 1))
                elif x and name(x) in al: ext[al[name(x)]] = BAD
            # the alias registers that this instruction reads / writes outside memory operands
            rd, wr = ins.regs_access(); rn = {name(r) for r in rd}; wn = {name(r) for r in wr}
            membase = {name(o.mem.base) for o in mems if o.mem.base} | {name(o.mem.index) for o in mems if o.mem.index}
            if slot is not None:
                if mn == "mov" and ops[0].type == CX.X86_OP_REG and ops[0].size == 4 and ops[1].type == CX.X86_OP_MEM:
                    al[name(ops[0].reg)] = slot; continue
                if mn in ("fld", "fild", "fadd", "fmul", "fsub", "fdiv", "fcom", "fcomp", "fsubr", "fdivr", "cmp", "test"): pass      # value use of the argument (not a pointer use)
                else: ext[slot] = BAD
            direct = ops[0].type == CX.X86_OP_IMM if ops else False
            if mn == "call":
                if direct and BASE <= ops[0].imm < IMGEND:
                    try: d -= callee_info(ops[0].imm).pop
                    except Exception: pass
                elif not direct:      # indirect call (the pointer may be the call target or a register argument)
                    for r in [r for r in al if r in rn]: ext[al.pop(r)] = BAD
                for r in ("eax", "ecx", "edx"):
                    if r in al: ext[al[r]] = BAD
                    al.pop(r, None)
                continue
            if mn == "jmp" and direct and not (va <= ops[0].imm < va + sz): ext = [BAD] * nargs; break
            if mn == "jmp": reach = False
            if mn == "push": d += 4
            elif mn == "pop": d -= 4
            elif mn == "sub" and ops[0].type == CX.X86_OP_REG and name(ops[0].reg) == "esp" and ops[1].type == CX.X86_OP_IMM: d += ops[1].imm
            elif mn == "add" and ops[0].type == CX.X86_OP_REG and name(ops[0].reg) == "esp" and ops[1].type == CX.X86_OP_IMM: d -= ops[1].imm
            elif mn == "mov" and ops[0].type == CX.X86_OP_REG and name(ops[0].reg) == "ebp" and ops[1].type == CX.X86_OP_REG and name(ops[1].reg) == "esp": ebp_d = d
            for r in [r for r in al if r in rn and r not in membase]:      # an alias used outside a memory operand
                if mn in ("test", "cmp"): continue
                if mn == "mov" and ops[0].type == CX.X86_OP_REG and ops[1].type == CX.X86_OP_REG and name(ops[1].reg) == r: al[name(ops[0].reg)] = al[r]; continue
                ext[al[r]] = BAD
            for r in wn:      # a register overwritten: no longer the pointer (derived pointers gave up above)
                if r in al and r in rn and r not in membase and mn not in ("test", "cmp"): ext[al.pop(r)] = BAD
                elif r in al and not (mn == "mov" and ops[0].type == CX.X86_OP_REG and name(ops[0].reg) == r and ops[1].type == CX.X86_OP_REG and name(ops[1].reg) in al): 
                    if r in rn and r in membase or r not in rn: al.pop(r, None)
    except Exception: return _AL.setdefault(key, [0] * nargs)
    out = [0 if e >= BAD or e <= 0 else (e + 3) // 4 for e in ext]
    return _AL.setdefault(key, out)

_RP = {}
def orig_ret_pops(va):
    """set of `ret N` values (0 = plain ret) of the ORIGINAL function at va by recursive descent over its bytes; empty when no ret was reached"""
    if va not in _RP:
        lo, hi = va, va + T().size.get(va, 0x400); todo = [va]; seen = set(); pops = set()
        while todo:
            a = todo.pop()
            while lo <= a < hi and a not in seen:
                ins = next(CS.disasm(img_read(a, 16), a, 1), None)
                if ins is None: break
                seen.add(a); g = ins.groups; op = ins.operands[0] if ins.operands else None
                if CX.X86_GRP_RET in g: pops.add(op.imm if op is not None and op.type == CX.X86_OP_IMM else 0); break
                if CX.X86_GRP_JUMP in g:
                    if op is not None and op.type == CX.X86_OP_IMM: todo.append(op.imm & 0xffffffff)
                    if ins.mnemonic == "jmp": break
                a += ins.size
        _RP[va] = pops
    return _RP[va]

_FRET = {}
_FCONS = ("fsin", "fcos", "fptan", "frndint", "f2xm1", "ftst", "fst", "fstp", "fmul", "fadd", "fsub", "fsubr", "fdiv", "fdivr", "fcom", "fcomp", "fabs", "fchs", "fsqrt", "fist", "fistp", "fucom", "fucomp", "fimul")
def float_returners():
    """va -> number of ORIGINAL call sites whose first x87 instruction (<= 6 instructions, no call / jump between) consumes ST0 with a memory operand or alone (fstp [m], fmul [m], fchs ...):
    the callee returns a float in ST0 although Ghidra typed it int / undefined (course_angels_cb1: `call 4757e0; fstp [esp]`); without it the stub left no value for the fstp = a false x87 stack DIFF"""
    if not _FRET:
        _FRET[0] = 0
        for s in pe.sections:
            if not s.Characteristics & 0x20000000: continue
            lo = BASE + s.VirtualAddress; d = IMG[s.VirtualAddress:s.VirtualAddress + max(s.Misc_VirtualSize, s.SizeOfRawData)]
            for m in re.finditer(rb"\xe8", d):
                a = lo + m.start(); t = (a + 5 + struct.unpack_from("<i", d, m.start() + 1)[0]) & 0xffffffff if m.start() + 5 <= len(d) else 0
                if not (lo <= t < lo + len(d)): continue
                n = 0
                for ins in CS.disasm(d[m.start() + 5:m.start() + 5 + 40], a + 5):
                    n += 1
                    if n > 6 or ins.mnemonic in ("call", "ret", "retn") or ins.mnemonic.startswith("j"): break
                    if ins.mnemonic.startswith("f") and ins.mnemonic not in ("fnstsw", "fnstcw", "fldcw", "fwait", "fnclex", "fninit"):
                        if ins.mnemonic in _FCONS and ("st(" not in ins.op_str or (ins.mnemonic == "fstp" and ins.op_str == "st(0)")):      # `fstp st(0)` = the result is popped unused (rwp_objects_create_*_group: call 56bf10; fstp st(0))
                            _FRET[t] = _FRET.get(t, 0) + 1
                        break
        for va in [k for k, v in _FRET.items() if k and v == 1]:      # a single call site may consume a value that was on the stack BEFORE the call: the callee itself must end like a float returner
            ok = False; ins = list(CS.disasm(img_read(va, 0x300), va))
            for i, x in enumerate(ins):
                if x.mnemonic in ("ret", "retn") and any(q.mnemonic.startswith("f") and q.mnemonic not in ("fnstsw", "fstp", "fwait") or q.mnemonic == "call" for q in ins[max(0, i - 4):i]): ok = True
            if not ok: del _FRET[va]
    return _FRET

def callee_info(va):
    """convention of a callee: ret N (output/regsigs.csv), number of stack arguments (declaration > sigs.txt > regsigs types > ret N / 4), register inputs, float return"""
    tb = T(); name = tb.rev.get(va, "FUN_%08x" % va)
    regs, ret_n, stack = tb.reg.get(va, ([], None, [])); sg = tb.sig.get(va); dc = tb.decl.get(va)
    cv = tb.convfix.get(name); stdc = (cv == "stdcall") if cv else (sg and sg[0] == "__stdcall")
    pop = ret_n if ret_n is not None else (sg[1] * 4 if sg and stdc else 0)
    if ret_n is None and pop and not cv and orig_ret_pops(va) == {0}: pop = 0      # Ghidra says __stdcall, the original body only has plain `ret` (the caller cleans up: platform_init `add esp, 8` behind controller_dialog_run): the stub must not pop either, else the caller's frame is wrecked
    n = None; o = tb.oslots.get(va); os_n = o[0] if o and not o[4] else 0
    if regs and (dc or sg) and va in tb.reg and tb.reg[va][0]: n = len(stack) or pop // 4      # register callee: the declaration / signature list the register inputs as parameters too (`f(undefined4 p0, undefined4 p1)` for eax|edx): only the stack arguments count
    if n is not None and regs and va in tb.reg and os_n > n: n = os_n      # register callee whose regsigs.csv row lists no / fewer stack types than the body reads (rx_pipeline_set_render_callback: callback at [esp+4], read after a call): the boundary call compares those slots too
    if n is not None: pass
    elif va in tb.stale and sg: n = sg[1]
    elif dc and dc[1].strip() not in ("", "void"): n = sum(2 if re.search(r"\bdouble\b", p) and "*" not in p else 1 for p in dc[1].split(","))      # a double argument is two dwords
    elif sg and sg[0] != "unknown": n = sg[1]
    elif stack: n = len(stack)
    elif pop: n = pop // 4
    elif o and o[5] and o[0] == 0 and not regs: n = 0      # exact0 body (see oslots): no argument can be observed, whatever the call sites push
    elif (va in tb.gt or os_n) and not (regs and not stack): n = max(tb.gt.get(va, 0), os_n)      # `()` declaration: the count comes from the original call sites (and the stack slots the callee's own body reads, output/orig_stackargs.txt, when exact: no address taken / lost esp), so the boundary call compares its argument VALUES too
    if o and o[2] and o[3] and not (regs and va in tb.reg):      # tail-forwarding callee (rewrites a slot, then `jmp target`: clump_load_by_name(name) -> 4c3bf0(name', out, out)): the target reads the other slots of the CALLER's frame, so the boundary call compares the target's slot count (a declaration with fewer parameters hid `clump_load_by_name(name, 0, &rdata)`: oil_load_assets crashed the game, diffemu EQUAL)
        tn = max([o[0], o[3]] + [tb.oslots[t][0] for t in o[2] if t in tb.oslots])
        if tn > (n or 0): n = tn
    try:
        import gsfix; n = gsfix.ARITY.get(name, n)      # library callees whose Ghidra signature swallowed the caller's frame (RwStreamClose: 17 parameters, really 2): comparing 12 stack words compared frame garbage
    except ImportError: pass
    rt = (dc[0] if dc else (sg[2] if sg else "")).split()
    c = Callee(va, name, pop, min(n or 0, 12), [r for r in regs if r in REGS], "f" if any(x in ("float", "double", "float10", "RwReal") for x in rt) or (float_returners().get(va) and va not in real_helpers() and all(x in ("", "undefined", "undefined4", "void", "unknown", "__cdecl", "__stdcall") for x in rt)) else "i", known=n is not None or va not in tb.reg, infer=n is None and not pop)
    if dc and dc[1].strip() not in ("", "void") and not (regs and va in tb.reg) and re.fullmatch(r"_?(?:RwV[23]d(?!Transform)\w*(?<!s)|RwFrame(?:Translate|Scale|Rotate|SetIdentity|Transform))", name): c.lim = ptr_limits(split_params(dc[1]))      # single object functions only: vector / vertex ARRAY functions (RwV3dTransformVectors, RwIm3D*) really read behind the first element
    if not c.lim and not regs and c.nargs: c.lim = asm_limits(va, c.nargs)      # no typed prototype: the extent the original callee's asm reads through each pointer argument
    return c

def split_params(s):
    out, d, cur = [], 0, ""
    for ch in s:
        d += {"(": 1, ")": -1}.get(ch, 0)
        if ch == "," and d == 0: out.append(cur); cur = ""
        else: cur += ch
    return out + [cur] if cur.strip() else out

def region(v):
    """region class of an address, for messages: stack | arg window | literal | .rdata | .data | .bss | code | null | heap"""
    if v < 0x1000: return "null/small"
    if isstack(v): return "stack"
    if ARGBUF <= v < ARGBUF + 0x40000: return "arg window"
    if SCR <= v < SCR + 0x10000: return "literal (.rdata)"
    if OURS <= v < OURS + 0x10000: return "code"
    if BASE <= v < IMGEND:
        for s_ in pe.sections:
            if BASE + s_.VirtualAddress <= v < BASE + s_.VirtualAddress + max(s_.Misc_VirtualSize, s_.SizeOfRawData):
                n_ = s_.Name.rstrip(b"\0").decode(errors="replace"); return n_ if n_ not in (".text",) else "code"
        return "image"
    return "heap/wild"
def isro_ptr(v): return v >= 0x1000 and (isro(v) or SCR <= v < SCR + 0x10000)

CANARY = os.environ.get("DIFFEMU_CANARY", "1") != "0"
_CNX = {}
def canary_extents(va, nargs):
    """per stack argument: bytes the ORIGINAL callee at va reaches through the pointer if it stores through it (tools/stack_lint.extents: (read, write)); 0 = none / unknown"""
    key = (va, nargs)
    if key in _CNX: return _CNX[key]
    out = [0] * nargs
    try:
        sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
        import stack_lint
        for k, (rd, wr) in enumerate(stack_lint.extents(va, nargs)):
            if wr > 0: out[k] = max(rd, wr)
    except Exception: pass
    try:
        for l in open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "callee_extents.txt")):
            q = l.split()
            if len(q) >= 3 and not l.startswith("#") and int(q[0], 16) == va and int(q[1]) < nargs: out[int(q[1])] = max(out[int(q[1])], int(q[2]))
    except OSError: pass
    _CNX[key] = out; return out
_WT = {}
_MEMW = {"mov", "add", "sub", "and", "or", "xor", "inc", "dec", "neg", "not", "shl", "shr", "sar", "rol", "ror", "adc", "sbb", "xchg", "bts", "btr", "btc", "cmpxchg", "xadd"}
def writes_through(va, depth=4):
    """frozenset of the stack argument indexes k of the ORIGINAL function at va whose pointer value it (or a function it passes the pointer on to, or tail-jumps to, up to `depth` levels) stores through:
    linear sweep of the body, esp delta + register / stack slot tags `argument k` (mov r,[esp+..], push, lea/add/mov r,r), a store `mov [r+d], x` / `rep stos` with r tagged, a direct call whose own writes_through
    contains the pushed slot.  Flow insensitive (a tag survives a join): an over-approximation that only feeds the read-only-destination check of the callee stub (Machine._callee)"""
    key = (va, depth)
    if key in _WT: return _WT[key]
    _WT[key] = frozenset(); res = set()
    if depth <= 0: return _WT[key]
    lo, hi = va, va + min(T().size.get(va, 0x200) or 0x200, 0x600)
    tag, slots, off, bp, a, dmap = {}, {}, 0, None, lo, {}      # dmap: esp delta at the targets of the jumps seen so far (the sweep continues behind a `ret` with the delta of the branch target)
    R = {CX.X86_REG_EAX: "eax", CX.X86_REG_ECX: "ecx", CX.X86_REG_EDX: "edx", CX.X86_REG_EBX: "ebx", CX.X86_REG_ESI: "esi", CX.X86_REG_EDI: "edi", CX.X86_REG_EBP: "ebp", CX.X86_REG_ESP: "esp"}
    def slot_of(op):
        mm = op.mem
        if mm.index or mm.segment: return None
        b = R.get(mm.base)
        if b == "esp": return off + mm.disp
        if b == "ebp" and bp is not None: return bp + mm.disp
        return None
    while a < hi:
        ins = next(CS.disasm(img_read(a, 16), a, 1), None)
        if ins is None: a += 1; continue
        if ins.address in dmap: off = dmap[ins.address]
        a += ins.size; mn = ins.mnemonic; ops = ins.operands; done = False
        if (mn == "jmp" or mn.startswith("j")) and ops and ops[0].type == CX.X86_OP_IMM: dmap.setdefault(ops[0].imm & 0xffffffff, off)
        if mn.startswith("rep") or mn.startswith(("stos", "movs")):
            if ("stos" in mn or "movs" in mn) and tag.get("edi"): res.add(tag["edi"][1])
        for k_, op in enumerate(ops):      # a store through a tagged base register
            if op.type == CX.X86_OP_MEM and op.access & 2:
                b = R.get(op.mem.base)
                if b not in (None, "esp", "ebp") and tag.get(b) and tag[b][0] == "a": res.add(tag[b][1])
                elif b == "ebp" and bp is None and tag.get("ebp") and tag["ebp"][0] == "a": res.add(tag["ebp"][1])
        if mn == "push" and ops:
            off -= 4; op = ops[0]
            slots[off] = (tag.get(R.get(op.reg)) if op.type == CX.X86_OP_REG else (slots.get(slot_of(op)) or (("a", (slot_of(op) - 4) // 4) if (slot_of(op) or 0) >= 4 and slot_of(op) % 4 == 0 else None)) if op.type == CX.X86_OP_MEM else None); done = True
        elif mn == "pop" and ops and ops[0].type == CX.X86_OP_REG:
            tag[R.get(ops[0].reg)] = slots.get(off); off += 4; done = True
        elif mn == "mov" and len(ops) == 2 and ops[0].type == CX.X86_OP_REG and R.get(ops[0].reg):
            d_ = R[ops[0].reg]; s_ = ops[1]
            if d_ == "ebp" and s_.type == CX.X86_OP_REG and R.get(s_.reg) == "esp": bp = off; tag.pop("ebp", None)
            elif d_ == "esp": pass
            elif s_.type == CX.X86_OP_REG: tag[d_] = tag.get(R.get(s_.reg))
            elif s_.type == CX.X86_OP_MEM:
                sl = slot_of(s_); tag[d_] = (slots.get(sl) or (("a", (sl - 4) // 4) if sl is not None and sl >= 4 and sl % 4 == 0 else None)) if sl is not None else None
            else: tag[d_] = None
            done = True
        elif mn == "mov" and len(ops) == 2 and ops[0].type == CX.X86_OP_MEM:
            sl = slot_of(ops[0])
            if sl is not None: slots[sl] = tag.get(R.get(ops[1].reg)) if ops[1].type == CX.X86_OP_REG else None
            done = True
        elif mn == "lea" and len(ops) == 2 and ops[0].type == CX.X86_OP_REG and R.get(ops[0].reg):
            b = R.get(ops[1].mem.base); tag[R[ops[0].reg]] = tag.get(b) if b not in (None, "esp", "ebp") and not ops[1].mem.index else None; done = True
        elif mn in ("add", "sub") and len(ops) == 2 and ops[0].type == CX.X86_OP_REG and R.get(ops[0].reg) == "esp" and ops[1].type == CX.X86_OP_IMM:
            off += ops[1].imm if mn == "add" else -ops[1].imm; done = True
        elif mn in ("add", "sub") and len(ops) == 2 and ops[0].type == CX.X86_OP_REG and ops[1].type == CX.X86_OP_IMM: done = True      # reg +- imm keeps the tag (pointer arithmetic)
        elif mn == "call" and ops:
            if ops[0].type == CX.X86_OP_IMM:
                t = ops[0].imm & 0xffffffff
                if BASE <= t < IMGEND:
                    for j in writes_through(t, depth - 1):
                        tg = slots.get(off + 4 * j)
                        if tg and tg[0] == "a": res.add(tg[1])
                    pp = orig_ret_pops(t); 
                    if len(pp) == 1: off += next(iter(pp))
            for r_ in ("eax", "ecx", "edx"): tag.pop(r_, None)
            done = True
        elif mn == "jmp" and ops and ops[0].type == CX.X86_OP_IMM:
            t = ops[0].imm & 0xffffffff
            if not (lo <= t < hi) and BASE <= t < IMGEND:      # tail jump: the target sees OUR frame slots (rewritten ones carry their new tag)
                for j in writes_through(t, depth - 1):
                    tg = slots.get(4 + 4 * j, ("a", j))
                    if tg and tg[0] == "a": res.add(tg[1])
            done = True
        elif mn in ("ret", "retn", "nop") or mn.startswith("j") or mn in ("cmp", "test"): done = True
        if not done:
            try: _, wr = ins.regs_access()
            except Exception: wr = ()
            for r_ in wr:
                if R.get(r_): tag.pop(R[r_], None)
    _WT[key] = frozenset(res)
    return _WT[key]

PRINTF = {"debug_printf": 0, "log_printf": 0, "printf": 0, "sprintf": 1, "wsprintfA": 1, "fprintf": 1, "_snprintf": 2}      # callee name -> index of the format argument
NORETURN = {"exit", "_exit", "abort", "ExitProcess", "_amsg_exit", "longjmp", "_fatal_error", "quick_exit"}      # a stubbed noreturn callee ends the case (the code behind the call is dead in the real program)

REGS = {"eax": UC_X86_REG_EAX, "ecx": UC_X86_REG_ECX, "edx": UC_X86_REG_EDX, "ebx": UC_X86_REG_EBX, "esi": UC_X86_REG_ESI, "edi": UC_X86_REG_EDI, "ebp": UC_X86_REG_EBP}

def import_stubs():
    """({stub address: Callee}, {IAT slot: stub address}): the exe's IAT points into the stub area"""
    tb = T(); infos, slots = {}, {}
    for k, (nm, slot) in enumerate(sorted(tb.iat.items())):
        a = IMPSTUB + 16 * k; n = tb.proto.get(nm)
        infos[a] = Callee(a, nm, 4 * (n or 0), n or 0, [], "i", known=n is not None); slots[slot] = a
    return infos, slots


# ---------------------------------------------------------------------------------------------------------- the emulator
SNAP = None      # set by seed.py
class Machine:
    """one emulated process: lazy pages (image data as in the file / random small ints / sprinkled, wild memory random), same seed -> same pages on both sides"""
    def __init__(m, seed, mode, lits, callees, imports, islots, wildfn=None):
        m.sw, m.frames, m.sp0, m.rowrite = set(), [], 0, []; m.track = False; m.wb = set(); m.uninit = set(); m.patch = {}; m.reads = None; m.small = False
        m.seed, m.mode, m.wild, m.trace, m.writes, m.cnt, m.islots, m.stub, m.wildfn = seed, mode, 0, [], [], {}, islots, {}, wildfn
        uc = m.uc = Uc(UC_ARCH_X86, UC_MODE_32)
        for a, n in sorted(((STACK, STK), (RET, 0x1000), (ARGBUF, 0x40000), (OURS, 0x10000), (SCR, 0x10000), (STUBS, 0x10000), (IMPSTUB, 0x10000), (FPPAGE, 0x1000))): uc.mem_map(a, n)      # ascending: with RET (0x7f000000) mapped before ARGBUF / OURS Unicorn 2.1.4 re-executes the first store into ARGBUF after a memory hook and loses the flags (cmp; mov [eax], imm; ja took the wrong branch: time_trial_default_record_b)
        if lits: uc.mem_write(SCR, lits)
        uc.mem_write(IMPSTUB, b"\xc3" * 0x10000)
        m.hooked = dict(imports); m.hooked.update(callees)
        uc.hook_add(UC_HOOK_MEM_UNMAPPED, m._unmapped); uc.hook_add(UC_HOOK_MEM_WRITE, m._write)
        for a in callees: uc.hook_add(UC_HOOK_CODE, m._callee, begin=a, end=a)
        uc.hook_add(UC_HOOK_CODE, m._callee, begin=IMPSTUB, end=IMPSTUB + 0x10000)
        m.nullcall = False; uc.hook_add(UC_HOOK_CODE, m._nullcode, begin=0, end=0xfff)      # code in the null page: a call through a pointer the function zeroed itself (malloc stub returned a small value, rep stos cleared the table): garbage execution, the case is skipped (NULLCALL)

    def page(m, p):
        r = random.Random(p * 2654435761 ^ m.seed * 40503 ^ 0x5bd1e995)
        if BASE <= p < IMGEND:
            b = bytearray(IMG[p - BASE:p - BASE + 0x1000].ljust(0x1000, b"\0"))
            if any(a <= p < z for a, z in WSEC):
                if m.mode == 1: b = bytearray(r.randbytes(0x1000))
                if m.mode in (1, 2):
                    for j in range(0, 0x1000, 4):
                        if r.random() < (0.75 if m.mode == 1 else 0.2): b[j:j + 4] = struct.pack("<i", r.choice((0, 0, 1, 2, 3, 4, -1, r.randrange(-3, 40))))
            for slot, a in m.islots.items():
                if p <= slot < p + 0x1000: b[slot - p:slot - p + 4] = struct.pack("<I", a)
            return defloat(bytes(b)) if m.mode in (1, 2) and any(a <= p < z for a, z in WSEC) else bytes(b)
        m.wild += 1
        return defloat(m.wildfn(r, p) if m.wildfn else r.randbytes(0x1000))

    def _unmapped(m, uc, access, addr, size, val, ud):
        p = addr & ~0xfff
        if access == UC_MEM_FETCH_UNMAPPED and not (BASE <= p < IMGEND):
            # indirect call through data (function pointer / vtable / RenderWare engine table) into the void: seeding of the top INCONCLUSIVE cause: the target becomes a stubbed callee
            # (logged with its address and the arguments the caller removes with `add esp, N`, returns a deterministic value, cdecl)
            if not m.indirect: return False
            try: uc.mem_map(p, 0x1000)
            except UcError: return False
            uc.mem_write(p, b"\xc3" * 0x1000); uc.hook_add(UC_HOOK_CODE, m._indirect, begin=p, end=p + 0xfff); return True
        try: uc.mem_map(p, 0x1000)
        except UcError: return False
        uc.mem_write(p, SNAP(p, m.page(p), m.seed) if SNAP else m.page(p))      # seed.py: game-state snapshot pages
        for a, v in m.patch.items():      # loop-bound retry (core.retry_case): dwords of this page that the case overrides
            if a & ~0xfff == p: uc.mem_write(a, struct.pack("<I", v))
        return True

    def poke(m, a, v):
        """override one dword of the input memory (same patch on both sides = just another input)"""
        try: m.uc.mem_write(a, struct.pack("<I", v))
        except UcError:
            if m._unmapped(m.uc, UC_MEM_WRITE_UNMAPPED, a, 4, 0, None): m.uc.mem_write(a, struct.pack("<I", v))

    indirect = True; ours = False; indargs = True

    def _indirect(m, uc, addr, size, ud):
        sp = uc.reg_read(UC_X86_REG_ESP); ra = struct.unpack("<I", bytes(uc.mem_read(sp, 4)))[0]; n = 0
        try:
            ins = next(CS.disasm(m.read(ra, 8), ra, 1), None)
            if ins and ins.mnemonic == "add" and ins.op_str.startswith("esp, "): n = min(int(ins.op_str[5:], 0) // 4, 2)      # <= 2: batched cleanup (one `add esp` for several calls) makes the count differ between orig and ours; the render-state / loader pointers take 2
        except (UcError, ValueError): pass
        args = struct.unpack("<%dI" % n, bytes(uc.mem_read(sp + 4, 4 * n))) if n and m.indargs else ()      # cluster mode: the argument count read from the caller's `add esp` is unreliable (batched cleanup), the target address (random per object) identifies the call
        m.trace.append(("indirect %08x" % addr, tuple(m.norm(a) for a in args), ()))
        h = int(sha("ind", addr, len(m.trace), m.seed), 16); uc.reg_write(UC_X86_REG_EAX, (0, 1, h % 50, u32(h))[h % 4]); uc.reg_write(UC_X86_REG_EDX, u32(h >> 20))

    def _nullcode(m, uc, addr, size, ud):
        m.nullcall = True; uc.emu_stop()

    def _write(m, uc, access, addr, size, val, ud):
        if m.track:
            for i in range(size): m.wb.add(addr + i)      # byte granular: a partly written word is still partly uninitialised
        if isstack(addr): m.sw.add(addr & ~3)      # stack words the function itself wrote (frame windows below: residue of an earlier frame is never compared)
        if not (isstack(addr) or OURS <= addr < OURS + 0x10000 or SCR <= addr < SCR + 0x10000 or FPPAGE <= addr < FPPAGE + 0x1000): m.writes.append((addr, size))

    def _rd(m, uc, access, addr, size, val, ud):
        """read of a stack byte below the entry ESP that this run has not written (uninitialised local / residue of the pre-fill)"""
        for i in range(size):
            if addr + i not in m.wb: m.uninit.add((addr + i) & ~3)

    def stubaddr(m, pop, fl):
        if (pop, fl) not in m.stub:
            a = STUBS + 0x20 * len(m.stub)
            m.uc.mem_write(a, (b"\xdd\x05" + struct.pack("<I", FPSRC) if fl else b"") + (b"\xc2" + struct.pack("<H", pop) if pop else b"\xc3")); m.stub[(pop, fl)] = a
        return m.stub[(pop, fl)]

    def norm(m, v):
        """argument value for the trace: stack addresses and literal addresses differ between the two builds, compare literals by content"""
        if isstack(v): return "S"
        if SCR <= v < SCR + 0x10000 or isro(v):
            try: b = bytes(m.uc.mem_read(v, 24)) if v >= SCR else bytes(IMG[v - BASE:v - BASE + 24])
            except UcError: return v
            z = b.find(b"\0"); return ("lit", b[:z + 1] if 0 <= z < 24 else b[:8])
        return v

    def _callee(m, uc, addr, size, ud):
        c = m.hooked.get(addr)
        if c is None: m._indirect(uc, addr, size, ud); return      # a wild jump into the middle of the import stub area (the bytes there are `ret`): a stubbed indirect call like any other
        sp = uc.reg_read(UC_X86_REG_ESP); i = m.cnt[addr] = m.cnt.get(addr, -1) + 1
        if c.name in NORETURN:
            m.trace.append((c.name, tuple(m.norm(a) for a in struct.unpack("<%dI" % min(c.nargs, 4), bytes(uc.mem_read(sp + 4, 4 * min(c.nargs, 4))))), ())); m.exited = True; uc.emu_stop(); return
        args = struct.unpack("<%dI" % c.nargs, bytes(uc.mem_read(sp + 4, 4 * c.nargs))) if c.nargs else ()
        pf = PRINTF.get(c.name)
        if pf is not None and len(args) > pf:      # printf-like: the arguments are the format and what its conversions consume (not a fixed count: the words behind are frame garbage that differs between the two builds)
            try:
                fmt = m.read(args[pf], 200).split(b"\0")[0].decode("latin1")
                k = 0
                for mm in re.finditer(r"%(?:%|[-+ #0]*(\*|\d+)?(?:\.(\*|\d+))?(hh|h|ll|l|I64|L|w)?([a-zA-Z]))", fmt):
                    if mm.group(0) != "%%": k += 1 + (mm.group(1) == "*") + (mm.group(2) == "*") + (mm.group(3) in ("ll", "I64") or mm.group(4) in "fgeEGaA")
                n_ = min(pf + 1 + k, 12); args = struct.unpack("<%dI" % n_, bytes(uc.mem_read(sp + 4, 4 * n_)))
            except (UcError, struct.error): pass
        nat = getattr(c, "natural", None)      # cluster mode: a callee outside the cluster that is already natural C: arguments in registers / stack slots of the original, all on the stack on our side
        if nat:
            args = struct.unpack("<%dI" % len(nat), bytes(uc.mem_read(sp + 4, 4 * len(nat)))) if m.ours else tuple(uc.reg_read(REGS[t]) if t in REGS else struct.unpack("<I", bytes(uc.mem_read(sp + 4 + 4 * int(t[1:]), 4)))[0] for t in nat)
            m.trace.append((c.name, tuple(m.norm(a) for a in args), ()))
        else: m.trace.append((c.name, tuple(m.norm(a) for a in args), tuple(m.norm(uc.reg_read(REGS[r])) for r in c.regs)))
        for k in writes_through(c.va):      # the ORIGINAL callee stores through its argument k: a destination in read-only memory (.rdata string / literal / code) is a write the stub must not hide (our code passed a string address instead of the buffer / NULL: oil_load_assets)
            a = struct.unpack("<I", bytes(uc.mem_read(sp + 4 + 4 * k, 4)))[0] if k < 12 else 0
            if isro_ptr(a):
                m.rowrite.append((c.name, k, region(a))); m.writes.append((a, 4))
                try: uc.mem_write(a, struct.pack("<I", int(sha(c.va, i, m.seed, "ro", k), 16) % 61))
                except UcError: pass
        h = int(sha(c.va, i, m.seed), 16); r = h >> 8
        outs = args
        if c.infer:      # no declared arguments (`f()`): the caller's `add esp, N` after the call tells how many words it pushed (used for the out parameters only, never compared: a compiler may batch the adds)
            ra = struct.unpack("<I", bytes(uc.mem_read(sp, 4)))[0]; code = m.read(ra, 40)      # m.read maps the next code page lazily (a plain mem_read across a page boundary raised READ_UNMAPPED: UNMAPPED 200 of four functions)
            for ins in CS.disasm(code, ra):      # the compiler may schedule a few instructions between the call and `add esp, N`
                if ins.mnemonic == "add" and ins.op_str.startswith("esp, "):
                    k = min(int(ins.op_str[5:], 0) // 4, 6); outs = struct.unpack("<%dI" % k, bytes(uc.mem_read(sp + 4, 4 * k))); break
                if ins.mnemonic in ("call", "ret", "push") or ins.mnemonic.startswith("j"): break
        for k, a in enumerate(outs):      # frame window: what the ORIGINAL callee would read behind a pointer to a local (a frame object whose members the callee walks: clump_has_uv_anim {flag, callback, flag pointer} passed as &callback)
            if isstack(a):
                snap = []
                nxt = min([(b - a) // 4 for b in outs if isstack(b) and b > a] + [FRAMEWIN] + ([c.lim[k]] if k < len(c.lim) and c.lim[k] else []))      # typed pointer parameter: the window ends with the object (ptr_limits)      # another pointer argument into the stack starts a separate object (RtIntersectionLineTriangle(&start, &delta, ..))
                for j in range(nxt):
                    w = a + 4 * j
                    if not isstack(w) or (w + 4 > m.sp0 and (w & ~3) not in m.sw): break      # the return address / arguments of the function itself are shared by the two sides (unless the function wrote them: a frame object compiled into its own argument slot, cluster weapon_flashbang_flare_particles)
                    if (w & ~3) in m.sw:
                        v = struct.unpack("<I", bytes(uc.mem_read(w, 4)))[0]; snap.append((j, ("S", v - a, v) if isstack(v) else m.norm(v)))
                m.frames.append((c.name, k, tuple(snap)))
                if os.environ.get("FRAMEDBG"): sys.stderr.write("FRAME %s\n" % (("ours" if m.ours else "orig", c.name, k, hex(a), hex(sp), hex(m.sp0), snap, [hex(x) for x in outs]),))
        for k, a in enumerate(outs):      # out parameters: a dword behind every pointer to a local gets a deterministic value (else it is frame garbage, different in the two builds)
            if isstack(a): uc.mem_write(a, struct.pack("<I", int(sha(c.va, i, m.seed, k), 16) % 61))
        if CANARY:      # K-use 2026-10-23: the hooked callee would store up to its asm store extent (stack_lint.extents: max(read, write) where it writes at all) behind every pointer into the frame; a canary there overwrites what a too small C frame struct has behind the member (saved registers, return address) and shows as DIFF (course_world_ray_hit_light_sample: 64 byte matrix into 60 bytes)
            ex_ = canary_extents(c.va, len(outs))
            if os.environ.get("CANDBG"): sys.stderr.write("CAN %s %x outs=%s ex=%s ours=%s\n" % (c.name, c.va, [hex(x) for x in outs], ex_, m.ours))
            for k, a in enumerate(outs):
                if isstack(a) and k < len(ex_) and ex_[k] > 4:
                    for j in range(min(ex_[k], 0x100) // 4):
                        w = a + 4 * j
                        if not isstack(w): break      # no bound at the entry esp on purpose: the return address / saved registers behind a too small frame struct are what the canary must hit
                        uc.mem_write(w, struct.pack("<I", 0xC0DE0000 | (int(sha(c.va, i, m.seed, k, j, "cn"), 16) & 0xffff)))
        for nm_, nw_ in OUTWORDS.items():      # callees that fill a whole record behind a pointer to a local: the generic single out-word leaves the rest as frame garbage that differs between the two builds
            if c.name.startswith(nm_):
                for k, a in enumerate(args):
                    if isstack(a):
                        for j in range(nw_):
                            if isstack(a + 4 * j) and a + 4 * j + 4 <= m.sp0 - 0x0: uc.mem_write(a + 4 * j, struct.pack("<I", int(sha(c.va, i, m.seed, k, j), 16) % 61))
        if c.name.startswith("RpPTankAtomicLock") and len(args) > 1 and isstack(args[1]):      # RpPTankLockStruct {data, stride}: a real buffer in the compared argument area (the generic out-parameter value would be a small integer: the stores of the function went to addresses 0..60, order dependent and different in the two builds)
            uc.mem_write(args[1], struct.pack("<II", ARGBUF + 0x10000 + 0x2000 * (i % 6), 0x20 + 4 * (i % 4)))
        if c.ret == "f": uc.mem_write(FPSRC, struct.pack("<d", ((h >> 4) % 4000 - 2000) / 8.0))
        w_ = RETWANT.get(c.va)
        if w_ and not m.small and m.seed % 3 == 2 and (h >> 3) % 3 != 0: uc.reg_write(UC_X86_REG_EAX, w_[(h >> 12) % len(w_)])      # compare-guided result (see ret_wants)
        else: uc.reg_write(UC_X86_REG_EAX, (0, 1, r % 3, r % 50, r % 6, r % 256, r % 10, 2)[h % 8] if m.small else (0, 1, u32(-1), r % 50, r % 6, r % 256, u32(r * 2654435761), u32(r))[h % 8])      # m.small: retry_case, a loop bound returned by a stubbed callee
        if not keeps_reg(c.va, "edx"): uc.reg_write(UC_X86_REG_EDX, u32(h >> 20))      # a callee that never writes ecx / edx (tools/pseudofix.may_write over its bytes) leaves the caller's value: the ORIGINAL keeps loop counters there (powerups_set_all_enabled)
        if not keeps_reg(c.va, "ecx"): uc.reg_write(UC_X86_REG_ECX, u32(h >> 12))
        uc.reg_write(UC_X86_REG_EIP, m.stubaddr(0 if nat and m.ours else c.pop, c.ret == "f"))

    def run(m, entry, args, regs, fill, lo, hi, count=MAXINS):
        """call entry (stack arguments `args`, initial registers `regs`, stack pre-fill bytes) until it returns to RET; dict of everything that is compared"""
        uc = m.uc; sp = STACK + STK - 0x200
        uc.mem_write(STACK, fill)
        uc.mem_write(sp, struct.pack("<%dI" % (1 + len(args)), RET, *[u32(x) for x in args]))
        for r, v in regs.items(): uc.reg_write(REGS[r], u32(v))
        uc.reg_write(UC_X86_REG_ESP, sp); m.blocks = {}; m.exited = False; m.sp0 = sp
        if m.track: uc.hook_add(UC_HOOK_MEM_READ, m._rd, begin=STACK, end=sp - 1)
        if count >= MAXINS // 7: uc.hook_add(UC_HOOK_BLOCK, lambda uc, a, s, u: m.blocks.__setitem__(a, max(s, m.blocks.get(a, 0))), begin=lo, end=hi)      # only the 12000 instruction trial runs of retry_case need no coverage (the hook used to stop after 3 runaway cases: count MAXINS // 7 > coverage ~0.4 whatever the case count)
        err = None
        t0 = time.time()
        try: uc.emu_start(entry, RET, timeout=2000000, count=count)
        except UcError as e: err = e
        slow = time.time() - t0 >= 1.9      # wall clock timeout of the emulator (a loaded machine), not a loop: never a DIFF
        eip = uc.reg_read(UC_X86_REG_EIP); r = dict(done=((eip == RET or m.exited) and err is None), exited=m.exited, err=err, eip=eip, wild=m.wild)
        if not r["done"]:
            if m.nullcall: c = "NULLCALL"
            elif err is None: c = "LOOP"
            elif err.errno == UC_ERR_FETCH_UNMAPPED: c = "INDIRECT_CALL"
            elif err.errno == UC_ERR_EXCEPTION: c = "EXCEPTION"
            elif err.errno in (UC_ERR_READ_UNMAPPED, UC_ERR_WRITE_UNMAPPED): c = "UNMAPPED"
            else: c = "OTHER"
            r["cause"] = "TIMEOUT" if slow and c == "LOOP" else ("WILD_" if m.wild and c in ("LOOP", "EXCEPTION") else "") + c; return r
        r.update(eax=uc.reg_read(UC_X86_REG_EAX), edx=uc.reg_read(UC_X86_REG_EDX), esp=uc.reg_read(UC_X86_REG_ESP) - sp,
                 saved=tuple(uc.reg_read(x) for x in (UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP)),
                 fptop=(uc.reg_read(UC_X86_REG_FPSW) >> 11) & 7, trace=m.trace, st0=None)
        r["xs"] = []
        if r["fptop"]:      # every value left on the x87 stack (the result on top, stale entries below), all 80 bits each, stored to the dedicated FPPAGE
            uc.mem_write(RET + 0x100, b"\xdb\x3d" + struct.pack("<I", FPOUT))
            try:
                for _ in range(fp_depth(uc, r["fptop"])): uc.emu_start(RET + 0x100, RET + 0x106); r["xs"].append(bytes(uc.mem_read(FPOUT, 10)))
            except UcError: pass
            r["st0"] = r["xs"][0] if r["xs"] else None
        return r

    def read(m, a, n):
        out = bytearray()
        while n > 0:
            k = min(n, 0x1000 - (a & 0xfff))
            try: out += uc_read(m, a, k)
            except UcError: out += b"\xff" * k
            a += k; n -= k
        return bytes(out)

def uc_read(m, a, k):
    try: return bytes(m.uc.mem_read(a, k))
    except UcError:
        if not m._unmapped(m.uc, UC_MEM_READ_UNMAPPED, a, k, 0, None): raise
        return bytes(m.uc.mem_read(a, k))


# ---------------------------------------------------------------------------------------------------------- function under test
class Fn:
    """everything needed to run one function: original address, our relocated code, signature, callees, block leaders"""
    def __init__(f, name, va, obj, sig):
        tb = T(); f.name, f.va, f.sig, f.notes = name, va, sig, []
        f.size = tb.size.get(va, 0x400); f.hi = va + f.size
        f.code, f.lits = our_code(obj, name)
        lo_, ext_o = scan(img_read, va, va, f.hi)
        lu_, ext_u = scan(lambda a, n: f.code[a - OURS:a - OURS + n], OURS, OURS, OURS + len(f.code))
        f.leaders_o, f.leaders_u = lo_, lu_
        f.imports, f.islots = import_stubs()
        real = real_helpers(); f.callees = {}
        for t in sorted((ext_o | ext_u) - real - {va}):
            if BASE <= t < IMGEND and not tb.rev.get(t, "").startswith("LAB_"): f.callees[t] = callee_info(t)      # LAB_xxx = unnamed code (function tail / thunk the size table cuts off): run the real bytes, its own callees are stubbed
        f.pushn = tb.gt.get(va, 0) if not tb.reg.get(va, ([],))[0] else 0      # arguments the original callers push, and the stack slots its body reads / rewrites (a thunk `mov [esp+4],x ; jmp f` passes the other slots of f through)
        o = tb.oslots.get(va)
        if o:
            f.pushn = max(f.pushn, o[0], o[1] // 4, o[3], *[tb.oslots.get(t, (0,))[0] for t in o[2]])
        f.pushn = min(f.pushn, 12)
        f.regin = tb.reg.get(va, ([], 0, []))[0]; f.retn = tb.reg.get(va, ([], 0, []))[1]
        unk = [c.name for c in f.callees.values() if not c.known]
        if unk: f.notes.append("callee args unknown: " + ",".join(unk[:3]))
        if f.regin: f.notes.append("register inputs " + "|".join(f.regin))
        f.cov_o, f.cov_u = set(), set()
        f.mutate = None      # (offset, byte) patch of our code


# ---------------------------------------------------------------------------------------------------------- inputs
def bounds(c):
    w = {"i1": 8, "i2": 16}.get(c, 32); s = {0, 1, 2, 3, 4, 5, 8, 15, 16, 17, 31, 32, 33, 63, 64, 100, 127, 128, 255, 256, 1000}
    for k in range(7, w + 1): s |= {(1 << k) - 1, 1 << (k - 1) if k < w + 1 else 0, (1 << (k - 1)) + 1}
    s |= {(1 << w) - 1, (1 << w) - 2, (1 << (w - 1)) - 1, 1 << (w - 1)}
    out = set()
    for v in s:
        v &= (1 << w) - 1; sv = v - (1 << w) if v >> (w - 1) else v; out.add(sv & 0xffffffff)      # sign extended to a pushed dword
    return sorted(out)

FPOOL = [0.0, 1.0, -1.0, 0.5, -0.5, 2.0, 10.0, 100.0, 1000.0, -100.0, 0.001, 3.14159, 255.0, 0.25]

def gen_value(c, rnd):
    if c in ("i1", "i2", "i4"):
        r = rnd.random(); b = bounds(c)
        if r < 0.55: return [rnd.choice(b)]
        if r < 0.8: return [rnd.randrange(0, 100)]
        v = rnd.getrandbits({"i1": 8, "i2": 16}.get(c, 32)); return [v]
    if c == "f": return [struct.unpack("<I", struct.pack("<f", rnd.choice(FPOOL) if rnd.random() < 0.6 else rnd.uniform(-1000, 1000)))[0]]
    if c == "d": return list(struct.unpack("<II", struct.pack("<d", rnd.choice(FPOOL) if rnd.random() < 0.6 else rnd.uniform(-1000, 1000))))
    return [rnd.getrandbits(32), rnd.getrandbits(32)]      # ll

def window(rnd, size, text, struct_fn=None):
    """memory behind a pointer argument.  char pointers: a string; others: dwords of small ints and random values"""
    if "char" in text or "Char" in text:
        n = rnd.choice((0, 1, 3, 8, rnd.randrange(0, 60)));  b = bytes(rnd.choice(b"abcdefghijklmnopqrstuvwxyz0123456789_/.\\ ") for _ in range(n)) + b"\0"
        return b.ljust(size, b"\0")
    b = bytearray(rnd.randbytes(size))
    for j in range(0, size - 3, 4):
        if rnd.random() < 0.5: b[j:j + 4] = struct.pack("<i", rnd.choice((0, 0, 1, 2, 3, 4, -1, rnd.randrange(-3, 40))))
    if struct_fn: struct_fn(b, rnd)
    return defloat(bytes(b))

class Case:
    def __init__(c, seed, mode, fill, args, regs, wins): c.seed, c.mode, c.fill, c.args, c.regs, c.wins, c.patch, c.small = seed, mode, fill, args, regs, wins, {}, False
    def describe(c): return "seed %d mode %d fill %s args %s%s" % (c.seed, c.mode, c.fill if isinstance(c.fill, str) else hex(c.fill), [hex(a) if isinstance(a, int) else a for a in c.args], (" loop-bound patch %s" % {hex(a): v for a, v in c.patch.items()} if c.patch else "") + (" small stub returns" if c.small else ""))

FILLS = (0xA5, 0x00, 0xFF, "rand")

def cmp_targets(fn):
    """(addr, value) of the globals the ORIGINAL compares against an immediate: `cmp [abs], imm` and `mov r, [abs]` followed within 4 instructions by `cmp r, imm` (g_input_mode == 0x1000, mode switches).
    Random memory never hits these (coverage 0.3 of menu_left_pressed_any); evaluate() pokes 1-3 of them into a third of the cases (patch, applied to BOTH sides = just another input)."""
    if not hasattr(fn, "_cmpt"):
        out = []; a = fn.va; last = {}      # last: register -> (address, instruction index of the load)
        n = 0
        while a < fn.hi and n < 4000:
            ins = next(CS.disasm(img_read(a, 16), a, 1), None)
            if ins is None: a += 1; continue
            a += ins.size; n += 1; ops = ins.operands
            def absmem(o): return o.type == CX.X86_OP_MEM and o.mem.base == 0 and o.mem.index == 0 and BASE <= o.mem.disp < IMGEND
            if ins.mnemonic == "mov" and len(ops) == 2 and ops[0].type == CX.X86_OP_REG and absmem(ops[1]) and ops[1].size == 4: last[ops[0].reg] = (ops[1].mem.disp, n)
            elif ins.mnemonic == "cmp" and len(ops) == 2 and ops[1].type == CX.X86_OP_IMM:
                if absmem(ops[0]): out.append((ops[0].mem.disp, u32(ops[1].imm)))
                elif ops[0].type == CX.X86_OP_REG and ops[0].reg in last and n - last[ops[0].reg][1] <= 4 and ops[0].size == 4: out.append((last[ops[0].reg][0], u32(ops[1].imm)))
        fn._cmpt = sorted(set(out))
    return fn._cmpt

RETWANT = {}      # callee va -> immediates the ORIGINAL compares its result with (set by evaluate() for the function under test, one function at a time per process)
def ret_wants(fn):
    """{callee va: [imm..]}: `call f` followed within 4 instructions by `cmp eax, imm` / `sub eax, imm` (switch on a result): the stub returns these values in some cases"""
    if not hasattr(fn, "_retw"):
        out = {}; a = fn.va; n = 0; cur = None; since = 0
        while a < fn.hi and n < 4000:
            ins = next(CS.disasm(img_read(a, 16), a, 1), None)
            if ins is None: a += 1; continue
            a += ins.size; n += 1; ops = ins.operands; since += 1
            if ins.mnemonic == "call" and ops and ops[0].type == CX.X86_OP_IMM: cur = ops[0].imm; since = 0
            elif cur is not None and since <= 4 and ins.mnemonic in ("cmp", "sub") and len(ops) == 2 and ops[0].type == CX.X86_OP_REG and ops[0].reg in (CX.X86_REG_EAX, CX.X86_REG_AL) and ops[1].type == CX.X86_OP_IMM:
                v = u32(ops[1].imm) & (0xff if ops[0].reg == CX.X86_REG_AL else 0xffffffff)
                if v not in out.setdefault(cur, []): out[cur].append(v)
                cur = None
        fn._retw = out
    return fn._retw

def cmp_offsets(fn):
    """(disp, value, size) of `cmp [reg+disp], imm` (reg not esp / ebp, no index, 0 <= disp < 0x400): the field of a structure behind a pointer argument; evaluate() writes the value at that offset of the argument windows"""
    if not hasattr(fn, "_cmpo"):
        out = []; a = fn.va; n = 0; last = {}
        okm = lambda o: o.type == CX.X86_OP_MEM and o.mem.base not in (0, CX.X86_REG_ESP, CX.X86_REG_EBP) and o.mem.index == 0 and 0 <= o.mem.disp < 0x400 and o.size in (1, 2, 4)
        while a < fn.hi and n < 4000:
            ins = next(CS.disasm(img_read(a, 16), a, 1), None)
            if ins is None: a += 1; continue
            a += ins.size; n += 1; ops = ins.operands
            if ins.mnemonic == "mov" and len(ops) == 2 and ops[0].type == CX.X86_OP_REG and okm(ops[1]) and ops[1].size == 4: last[ops[0].reg] = (ops[1].mem.disp, n)      # mov eax, [edx+0x10] ... cmp eax, 0xb
            elif ins.mnemonic == "cmp" and len(ops) == 2 and ops[1].type == CX.X86_OP_IMM:
                if okm(ops[0]): out.append((ops[0].mem.disp, ops[1].imm & ((1 << 8 * ops[0].size) - 1), ops[0].size))
                elif ops[0].type == CX.X86_OP_REG and ops[0].reg in last and n - last[ops[0].reg][1] <= 6 and ops[0].size == 4: out.append((last[ops[0].reg][0], u32(ops[1].imm), 4))
        fn._cmpo = sorted(set(out))
    return fn._cmpo

def gen_cases(sig, n, k, seed0=0):
    """n argument sets x k memory seeds.  all-small-int signatures (1-2 arguments): the boundary values come first (cartesian); pointers get memory windows (struct sized when
    the pointee is a struct of names/MFL_structs*.txt)"""
    rnd = random.Random(seed0 * 7919 + len(sig.params)); cls = [p[0] for p in sig.params]; argsets = []
    if cls and len(cls) <= 2 and all(c in ("i1", "i2", "i4") for c in cls):
        import itertools
        pools = [sorted(set(bounds(c) + list(range(0, 256 if c == "i1" else 41 if len(cls) == 1 else 8)))) for c in cls]
        full = list(itertools.product(*pools)); rnd.shuffle(full); argsets = [[("v", [v]) for v in t] for t in full[:n]]
    while len(argsets) < n: argsets.append([("ptr", p) if p[0] == "p" else ("v", gen_value(p[0], rnd)) for p in sig.params])
    cases = []
    for si in range(k):
        for i, a in enumerate(argsets):
            seed = seed0 * 100003 + si * 7919 + i + 1; r = random.Random(seed); wins = []; flat = []; pos = ARGBUF
            for kind, x in a:
                if kind == "v": flat.append(x); continue
                if r.random() < 0.08: flat.append([0]); continue
                size = max(0x40, (T().structs[x[1]] + 15) & ~15) if x[1] in T().structs else WIN
                wins.append((pos, window(r, size, x[2]))); flat.append([pos]); pos += size + 0x40
            regs = {}; nreg = len(sig.regargs)
            for q, v in zip(("ecx", "edx"), flat[:nreg]): regs[q] = v[0]
            stack = [w for v in flat[nreg:] for w in v]
            cases.append(Case(seed, (i + si) % 3, FILLS[(i + si) % 4], stack, regs, wins))
    return cases


# ---------------------------------------------------------------------------------------------------------- run + compare
INIT = {"eax": 0, "ecx": 0, "edx": 0, "ebx": 0x12345678, "esi": 0x23456789, "edi": 0x34567890, "ebp": 0x456789ab}

_FILLS = {}
def _fill(f, r):
    if f != "rand":
        if f not in _FILLS: _FILLS[f] = defloat(bytes([f]) * (STK - 0x200))
        return _FILLS[f]
    return defloat(r.randbytes(STK - 0x200))

def side(fn, case, ours, count=MAXINS):
    m = Machine(case.seed, case.mode, fn.lits if ours else b"", fn.callees, fn.imports, fn.islots); m.ours = ours; m.indargs = not getattr(fn, "noindargs", False); m.track = case.fill == "rand" and not os.environ.get("DIFFEMU_NOUNINIT")      # random pre-fill: watch the reads of unwritten stack bytes (run_case)
    for a, b in case.wins: m.uc.mem_write(a, b)
    m.patch = case.patch; m.small = case.small
    for a, v in case.patch.items(): m.poke(a, v)
    args, cregs = fn.mapcase(case, ours) if getattr(fn, "mapcase", None) else (case.args, case.regs)      # cluster mode: register arguments of the original, stack arguments of ours
    if getattr(fn, "pushn", 0) > len(args): args = list(args) + [int(sha(case.seed, "pad", i), 16) % 61 + (0x1000 if ours else 0) for i in range(len(args), fn.pushn)]      # a dropped stack argument: the stack slots the ORIGINAL callers push / its body reads are seeded beyond the declared parameters; OUR side gets different values there, so a C definition that declares fewer parameters (an undeclared slot reaches a callee through a tail jump, or the call drops it) differs at the stubbed callee
    r = random.Random(case.seed ^ 0xabcdef); regs = dict(INIT, eax=r.getrandbits(32), ecx=r.getrandbits(32), edx=r.getrandbits(32)); regs.update(cregs)
    fill = _fill(case.fill, r)      # uninitialised locals read as floats: no NaN (0xFF fill) whose compare differs between `a < b` and `!(a >= b)` code
    if ours:
        code = bytearray(fn.code)
        if fn.mutate: code[fn.mutate[0]] = fn.mutate[1]
        m.uc.mem_write(OURS, bytes(code)); res = m.run(getattr(fn, "oentry", OURS), args, regs, fill, OURS, OURS + len(code), count)
    else: res = m.run(fn.va, args, regs, fill, getattr(fn, "olo", fn.va), fn.hi, count)
    return m, res

def covered(leaders, blocks): return {a for a in leaders if any(b <= a < b + s for b, s in blocks.items())} if blocks else set()

_FA = []
class _NoAllow(set):
    def add(self, x): pass
def frame_allow():
    """src/diffemu_frame_allow.txt: `function callee argindex reason`.  DIFFEMU_NOSTEER=1: empty (audit: do the allow lists hide a real difference?)"""
    if not _FA and os.environ.get("DIFFEMU_NOSTEER"): _FA.append(_NoAllow())
    if not _FA:
        s = set()
        try:
            for l in open(os.path.join(ROOT, "src", "diffemu_frame_allow.txt")):
                q = l.split(None, 3)
                if len(q) == 4 and not l.startswith("#"): s.add((q[0], q[1], int(q[2])))
        except OSError: pass
        _FA.append(s)
    return _FA[0]

def compare(fn, mo, ro, mu, ru):
    """list of difference descriptions between original (o) and ours (u); empty = equal"""
    d = []; sg = fn.sig
    if ro["exited"] != ru["exited"]: d.append("noreturn callee (exit): original %s, ours %s" % (ro["exited"], ru["exited"]))
    exited = ro["exited"] and ru["exited"]
    if exited: ro = dict(ro, esp=0, eax=0, edx=0, fptop=0, st0=None, saved=ru["saved"]); ru = dict(ru, esp=0, eax=0, edx=0, fptop=0, st0=None)
    if getattr(fn, "espok", None):
        if not exited and not fn.espok(ro["esp"], ru["esp"]): d.append("esp delta orig %d ours %d (not a plain cdecl / stdcall return)" % (ro["esp"], ru["esp"]))
    elif ro["esp"] != ru["esp"]: d.append("esp delta orig %d ours %d (ret N)" % (ro["esp"], ru["esp"]))
    for nm, a, b in zip(("ebx", "esi", "edi", "ebp"), ro["saved"], ru["saved"]):
        if a != b and nm not in getattr(fn, "skipsaved", ()):      # cluster root: a register INPUT the original does not preserve (the entry thunk of a bridge restores it)
             d.append("%s orig %08x ours %08x" % (nm, a, b))
    if ro["fptop"] != ru["fptop"]: d.append("x87 stack top orig %d ours %d" % (ro["fptop"], ru["fptop"]))
    if sg.ret in ("f", "d"):
        if ro["st0"] != ru["st0"]: d.append("ST0 orig %s ours %s" % (ro["st0"] and ro["st0"].hex(), ru["st0"] and ru["st0"].hex()))
    if not exited:      # stale x87 entries (below the result, or all of them in a void function): dead for the caller, but compared too (not masked)
        k0 = 1 if sg.ret in ("f", "d") else 0
        xo, xu = ro.get("xs", [])[k0:], ru.get("xs", [])[k0:]
        if xo != xu and len(xo) == len(xu): d.append("stale x87 stack entry %d orig %s ours %s" % (k0 + next(i for i in range(len(xo)) if xo[i] != xu[i]), *[x[next(i for i in range(len(xo)) if xo[i] != xu[i])].hex() for x in (xo, xu)]))
    elif sg.ret != "v" or os.environ.get("DIFFEMU_VOID_EAX"):      # DIFFEMU_VOID_EAX=1: also compare EAX of void functions (the original may leave a live result there: sweep-only functions have no caller to prove it)
        mask = {"i1": 0xff, "i2": 0xffff}.get(sg.ret, 0xffffffff)
        if ro["eax"] & mask != ru["eax"] & mask: d.append("eax orig %08x ours %08x" % (ro["eax"], ru["eax"]))
        if sg.ret == "ll" and ro["edx"] != ru["edx"]: d.append("edx orig %08x ours %08x" % (ro["edx"], ru["edx"]))
    a = [t for t in ro["trace"]]; b = [t for t in ru["trace"]]
    if a != b and len(a) == len(b):      # indirect calls: the argument count is read from the `add esp` behind the call (_indirect), a batched cleanup (`push 0; push esi; call [eax+0x108]` ... one `add esp, 8` later) hides it on one side: the common prefix decides
        for i, (x, y) in enumerate(zip(a, b)):
            if x != y and x[0] == y[0] and x[0].startswith("indirect ") and x[2] == y[2] and (x[1][:len(y[1])] == y[1] or y[1][:len(x[1])] == x[1]): a[i] = b[i] = (x[0], x[1][:min(len(x[1]), len(y[1]))], x[2])
    if a != b:
        k = next((i for i in range(min(len(a), len(b))) if a[i] != b[i]), min(len(a), len(b)))
        d.append("call trace differs at call %d (orig %d calls, ours %d): orig %s ours %s" % (k, len(a), len(b), a[k] if k < len(a) else "-", b[k] if k < len(b) else "-"))
    if mo.rowrite != mu.rowrite:
        x = [t for t in mu.rowrite if t not in mo.rowrite] or [t for t in mo.rowrite if t not in mu.rowrite]
        d.append("callee %s stores through argument %d pointing into read-only memory (%s) in %s only (the stub writes through destination pointers: orig %s ours %s)" % (x[0][0], x[0][1], x[0][2], "ours" if x[0] in mu.rowrite else "the original", mo.rowrite[:2], mu.rowrite[:2]))
    if len(mo.frames) == len(mu.frames):      # frame windows behind pointer arguments into the stack (the stubbed callee never dereferences them, the original callee does)
        for n, (fo, fu) in enumerate(zip(mo.frames, mu.frames)):
            if fo[:2] != fu[:2]: break
            do, du = dict(fo[2]), dict(fu[2])
            if (fn.name, fo[0], fo[1]) in frame_allow(): continue      # src/diffemu_frame_allow.txt: a frame window difference that was checked against the original callee (it reads fewer words than the window)
            bad = [j for j in sorted(do) if do[j] != du.get(j, "-") and not (isinstance(do[j], tuple) and isinstance(du.get(j), tuple) and len(do[j]) == 3 and len(du[j]) == 3 and do[j][0] == du[j][0] == "S" and (do[j][2] == du[j][2] or do[j][1] == du[j][1]))]  # initialised by the ORIGINAL and different / uninitialised in ours (words only ours initialised: ignored, unrelated locals)
            if bad: d.append("frame object behind stack pointer argument %d of call %s #%d differs at word(s) %s: orig %s ours %s" % (fo[1], fo[0], n, bad, [do[j] for j in bad], [du.get(j, "-") for j in bad])); break
    if os.environ.get("DIFFEMU_STACK"):      # strict stack mode (measurement; off by default, README diffemu "stack"): every frame word either side wrote must end with the same value at the same address.  Frame layouts of two builds legitimately differ, so this reports layout noise too.
        sw = sorted(w for w in mo.sw | mu.sw if w < mo.sp0 - 4)
        bad = []
        for w in sw:
            wo, wu = struct.unpack("<I", mo.read(w, 4))[0], struct.unpack("<I", mu.read(w, 4))[0]
            if wo != wu and not (isstack(wo) and isstack(wu)) and not (BASE <= wo < IMGEND and OURS <= wu < OURS + 0x10000): bad.append((w, wo, wu, w in mo.sw, w in mu.sw))
        if bad: d.append("stack: %d frame words differ (written by orig/ours): %s" % (len(bad), ", ".join("%08x %08x/%08x %s%s" % (w, a_, b_, "o" if o_ else "-", "u" if u_ else "-") for w, a_, b_, o_, u_ in bad[:6])))
    addrs = sorted({x + i for w in (mo.writes, mu.writes) for x, n in w for i in range(n)})
    if addrs:
        diffs = []      # final bytes of every touched page in both machines
        for p in sorted({x & ~0xfff for x in addrs}):
            bo, bu = mo.read(p, 0x1000), mu.read(p, 0x1000); aset = {x for x in addrs if x & ~0xfff == p}
            for x in sorted(aset):
                if bo[x - p] != bu[x - p]:
                    off = x & 0xffc; wo = struct.unpack("<I", bo[off:off + 4])[0]; wu = struct.unpack("<I", bu[off:off + 4])[0]
                    if isstack(wo) and isstack(wu): continue      # a pointer to a local: the two frames differ
                    diffs.append((x, bo[x - p], bu[x - p]))
        if diffs: d.append("memory: %d bytes differ: %s" % (len(diffs), ", ".join("%08x %02x/%02x" % x for x in diffs[:10])))
    return d

OUTWORDS = {"RwV3dTransformPoints": 9, "RwV3dTransformVectors": 3, "world_intersect_line": 8, "car_get_velocity_any_mode": 3, "FUN_005ba5c0": 16}      # record sizes in words (hit record / velocity vector / sound caps)
LOOPY = ("LOOP", "WILD_LOOP", "TIMEOUT")
PATCHES = [(v, k) for k in (4, 16, 64) for v in (0, 1, 2, 4)]      # (dword value, number of candidate addresses)

def probe_reads(fn, case):
    """addresses of the dwords the ORIGINAL reads (first read order) in the first MAXINS instructions of the runaway case that hold large values: loop counts / list heads read from random memory"""
    seen = {}
    def rd(uc, access, addr, size, val, ud):
        if size == 4 and addr not in seen and not isstack(addr) and not (OURS <= addr < IMPSTUB + 0x10000) and not isro(addr): seen[addr] = None
    m = Machine(case.seed, case.mode, b"", fn.callees, fn.imports, fn.islots); m.ours = False; m.indargs = not getattr(fn, "noindargs", False)
    for a, b in case.wins: m.uc.mem_write(a, b)
    m.uc.hook_add(UC_HOOK_MEM_READ, rd)
    r = random.Random(case.seed ^ 0xabcdef); regs = dict(INIT, eax=r.getrandbits(32), ecx=r.getrandbits(32), edx=r.getrandbits(32)); regs.update(case.regs)
    fill = defloat(r.randbytes(STK - 0x200) if case.fill == "rand" else bytes([case.fill]) * (STK - 0x200))
    args = list(case.args)
    if getattr(fn, "pushn", 0) > len(args): args += [int(sha(case.seed, "pad", i), 16) % 61 for i in range(len(args), fn.pushn)]
    m.run(fn.va, args, regs, fill, fn.va, fn.hi, 8000)      # only the first reads matter (list heads, counts, the first cells of a walk)
    allr = [a for a in seen if (a & 0xfff) <= 0xffc]
    return allr, [a for a in allr if struct.unpack("<I", m.read(a, 4))[0] > 64]

def sentinels(fn):
    """dword immediates (|v| >= 0x100) the ORIGINAL compares against (cmp / test), first 6 in address order: list terminators (`cmp [edx+eax*4], 0xff040000`)"""
    if not hasattr(fn, "_sent"):
        out = []; a = fn.va
        while a < fn.hi and len(out) < 6:
            ins = next(CS.disasm(img_read(a, 16), a, 1), None)
            if ins is None: a += 1; continue
            a += ins.size
            if ins.mnemonic == "cmp":
                for op in ins.operands:
                    if op.type == CX.X86_OP_IMM and abs(op.imm) >= 0x100 and u32(op.imm) not in out: out.append(u32(op.imm))
        fn._sent = out
    return fn._sent

def retry_case(fn, case, mo, ro):
    """a runaway case of the ORIGINAL (loop count / list head read from random memory): the same case with the large dwords the original read set to 0 / 1 / 2 / 4 (patch applied to BOTH sides: it is only a
    different input, the comparison stays real).  -> (case, mo, ro) of the first variant where the original returns, or None"""
    try: allr, cand = probe_reads(fn, case)
    except Exception: return None
    def tryit(patch, small=False):
        c2 = Case(case.seed, case.mode, case.fill, case.args, case.regs, case.wins); c2.patch = patch; c2.small = small
        if side(fn, c2, False, 12000)[1]["done"]:      # cheap trial, then the full run
            mo, ro = side(fn, c2, False)
            if ro["done"]: return c2, mo, ro
    pats = [("R",)] + [("L", v, k) for v, k in PATCHES if cand and not (k > 4 and len(cand) <= k // 4)]
    sents = sentinels(fn)      # sentinel terminated list walk: every m-th dword read from the k-th on becomes a terminator the original compares against (all of them, or cycling through them)
    pats += [("S", m_, k0, tuple(vals)) for m_ in (1, 2, 3, 5, 8) for k0 in (2, 3, 4) for vals in [[x] for x in sents] + ([sents] if len(sents) > 1 else [])]
    def build(p_):
        if p_[0] == "R": return {-1: 0}
        if p_[0] == "L": return {a: p_[1] for a in cand[:p_[2]]}
        _, m_, k0, vals = p_
        return {allr[k0 + j * m_]: vals[j % len(vals)] for j in range(16) if k0 + j * m_ < len(allr)} if k0 < len(allr) else {}
    last = getattr(fn, "_pat", None)      # the pattern that bounded the previous runaway case first (same function, same loop)
    t0 = time.time()
    for p_ in ([last] if last in pats else []) + [x for x in pats if x != last]:
        if time.time() - t0 > 6: break      # ponytail: 6 s per runaway case, a loop this stubborn needs a hand written seed
        patch = build(p_)
        r = (tryit({}, True) if p_[0] == "R" else tryit(patch)) if patch else None
        if r: fn._pat = p_; return r
    return None

def run_case(fn, case):
    """-> ('skip', cause) | ('equal', None) | ('diff', [descriptions]); updates the coverage sets"""
    mo, ro = side(fn, case, False, MAXINS if getattr(fn, "runaways", 0) < 3 else MAXINS // 7)      # a function that already ran away three times: the next runaways are cut earlier (a 300000 instruction run with a stubbed loop counter costs the 2 s wall clock)
    if not ro["done"] and ro["cause"] in LOOPY: fn.runaways = getattr(fn, "runaways", 0) + 1
    if not ro["done"] and ro["cause"] in LOOPY and getattr(fn, "retries", 0) < 6:
        rr = retry_case(fn, case, mo, ro)
        if rr is None: fn.retries = getattr(fn, "retries", 0) + 1      # a function whose runaway cases cannot be bounded stops paying for the retries
        else: case.patch, case.small, mo, ro = rr[0].patch, rr[0].small, rr[1], rr[2]; fn.retries = 0; fn.patched = getattr(fn, "patched", 0) + 1
    if not ro["done"]: return "skip", ro["cause"]
    fn.cov_o |= covered(fn.leaders_o, mo.blocks)
    mu, ru = side(fn, case, True)
    fn.cov_u |= covered(fn.leaders_u, mu.blocks)
    if (mo.uninit or mu.uninit) and ru["done"]:      # uninitialised stack read on either side under the random pre-fill: the two frames legitimately differ in size / layout, so the garbage differs.  Both sides rerun at the same ESP over a CONSTANT pre-fill, identical wherever neither side wrote (rule: README diffemu "stack").  Written words are never masked.
        fn.uninit_cases = getattr(fn, "uninit_cases", 0) + 1
        c2 = Case(case.seed, case.mode, (0xA5, 0x00)[case.seed & 1], case.args, case.regs, case.wins); c2.patch, c2.small = case.patch, case.small
        mo, ro = side(fn, c2, False); mu, ru = side(fn, c2, True)
        if not ro["done"]: return "skip", ro["cause"]
    if not ru["done"] and ru["cause"] == "TIMEOUT": return "skip", "TIMEOUT"
    if not ru["done"]: return "diff", ["ours does not return (%s, eip %08x%s) while the original does" % (ru["cause"], ru["eip"], ", " + str(ru["err"]) if ru["err"] else "")]
    d = compare(fn, mo, ro, mu, ru)
    return ("diff", d) if d else ("equal", None)
