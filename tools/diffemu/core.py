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
FPSRC, FPOUT = STUBS + 0x8000, STUBS + 0x8100
LITSZ = 256
MAXINS = 300000
WIN = 0x200      # default size of the memory window behind a pointer argument


FRAMEWIN = 8      # words (32 bytes) compared behind every pointer argument that points into the stack
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
        s.sig = {}      # va -> (conv, stack dwords, return type)
        for l in open(os.path.join(ROOT, "output", "sigs.txt")):
            p = l.rstrip("\n").split("|")
            if len(p) >= 7:
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
            for l in open(os.path.join(ROOT, "output", "orig_stackargs.txt")):
                q = l.split()
                s.oslots[int(q[0], 16)] = (int(q[1]), int(q[2]), [] if q[3] == "-" else [int(x, 16) for x in q[3].split(",")], max([int(x[1:]) for x in q[4].split(",") if re.fullmatch(r"w\d+", x)] or [0]), bool({"addr", "dyn"} & set(q[4].split(","))))
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
        "wchar_t": "i2", "char": "i1", "uchar": "i1", "byte": "i1", "bool": "i1", "undefined": "i1", "undefined1": "i1", "float": "f", "double": "d", "float10": "d",
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
            if not vas: raise Unresolved("symbol " + n)
            t = sorted(vas)[0] + add
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
class Callee:
    def __init__(s, va, name, pop, nargs, regs, ret, known=True, infer=False): s.va, s.name, s.pop, s.nargs, s.regs, s.ret, s.known, s.infer = va, name, pop, nargs, regs, ret, known, infer

def callee_info(va):
    """convention of a callee: ret N (output/regsigs.csv), number of stack arguments (declaration > sigs.txt > regsigs types > ret N / 4), register inputs, float return"""
    tb = T(); name = tb.rev.get(va, "FUN_%08x" % va)
    regs, ret_n, stack = tb.reg.get(va, ([], None, [])); sg = tb.sig.get(va); dc = tb.decl.get(va)
    cv = tb.convfix.get(name); stdc = (cv == "stdcall") if cv else (sg and sg[0] == "__stdcall")
    pop = ret_n if ret_n is not None else (sg[1] * 4 if sg and stdc else 0)
    n = None; o = tb.oslots.get(va); os_n = o[0] if o and not o[4] else 0
    if regs and (dc or sg) and va in tb.reg and tb.reg[va][0]: n = len(stack) or pop // 4      # register callee: the declaration / signature list the register inputs as parameters too (`f(undefined4 p0, undefined4 p1)` for eax|edx): only the stack arguments count
    elif dc and dc[1].strip() not in ("", "void"): n = sum(2 if re.search(r"\bdouble\b", p) and "*" not in p else 1 for p in dc[1].split(","))      # a double argument is two dwords
    elif sg and sg[0] != "unknown": n = sg[1]
    elif stack: n = len(stack)
    elif pop: n = pop // 4
    elif (va in tb.gt or os_n) and not (regs and not stack): n = max(tb.gt.get(va, 0), os_n)      # `()` declaration: the count comes from the original call sites (and the stack slots the callee's own body reads, output/orig_stackargs.txt, when exact: no address taken / lost esp), so the boundary call compares its argument VALUES too
    rt = (dc[0] if dc else (sg[2] if sg else "")).split()
    return Callee(va, name, pop, min(n or 0, 12), [r for r in regs if r in REGS], "f" if any(x in ("float", "double", "float10") for x in rt) else "i", known=n is not None or va not in tb.reg, infer=n is None and not pop)

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
class Machine:
    """one emulated process: lazy pages (image data as in the file / random small ints / sprinkled, wild memory random), same seed -> same pages on both sides"""
    def __init__(m, seed, mode, lits, callees, imports, islots, wildfn=None):
        m.sw, m.frames, m.sp0 = set(), [], 0
        m.seed, m.mode, m.wild, m.trace, m.writes, m.cnt, m.islots, m.stub, m.wildfn = seed, mode, 0, [], [], {}, islots, {}, wildfn
        uc = m.uc = Uc(UC_ARCH_X86, UC_MODE_32)
        for a, n in ((STACK, STK), (RET, 0x1000), (ARGBUF, 0x40000), (OURS, 0x10000), (SCR, 0x10000), (STUBS, 0x10000), (IMPSTUB, 0x10000)): uc.mem_map(a, n)
        if lits: uc.mem_write(SCR, lits)
        uc.mem_write(IMPSTUB, b"\xc3" * 0x10000)
        m.hooked = dict(imports); m.hooked.update(callees)
        uc.hook_add(UC_HOOK_MEM_UNMAPPED, m._unmapped); uc.hook_add(UC_HOOK_MEM_WRITE, m._write)
        for a in callees: uc.hook_add(UC_HOOK_CODE, m._callee, begin=a, end=a)
        uc.hook_add(UC_HOOK_CODE, m._callee, begin=IMPSTUB, end=IMPSTUB + 0x10000)

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
        uc.mem_write(p, m.page(p)); return True

    indirect = True; ours = False; indargs = True

    def _indirect(m, uc, addr, size, ud):
        sp = uc.reg_read(UC_X86_REG_ESP); ra = struct.unpack("<I", bytes(uc.mem_read(sp, 4)))[0]; n = 0
        try:
            ins = next(CS.disasm(bytes(uc.mem_read(ra, 8)), ra, 1), None)
            if ins and ins.mnemonic == "add" and ins.op_str.startswith("esp, "): n = min(int(ins.op_str[5:], 0) // 4, 8)
        except (UcError, ValueError): pass
        args = struct.unpack("<%dI" % n, bytes(uc.mem_read(sp + 4, 4 * n))) if n and m.indargs else ()      # cluster mode: the argument count read from the caller's `add esp` is unreliable (batched cleanup), the target address (random per object) identifies the call
        m.trace.append(("indirect %08x" % addr, tuple(m.norm(a) for a in args), ()))
        h = int(sha("ind", addr, len(m.trace), m.seed), 16); uc.reg_write(UC_X86_REG_EAX, (0, 1, h % 50, u32(h))[h % 4]); uc.reg_write(UC_X86_REG_EDX, u32(h >> 20))

    def _write(m, uc, access, addr, size, val, ud):
        if isstack(addr): m.sw.add(addr & ~3)      # stack words the function itself wrote (frame windows below: residue of an earlier frame is never compared)
        if not (isstack(addr) or OURS <= addr < OURS + 0x10000 or SCR <= addr < SCR + 0x10000): m.writes.append((addr, size))

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
        c = m.hooked[addr]; sp = uc.reg_read(UC_X86_REG_ESP); i = m.cnt[addr] = m.cnt.get(addr, -1) + 1
        if c.name in NORETURN:
            m.trace.append((c.name, tuple(m.norm(a) for a in struct.unpack("<%dI" % min(c.nargs, 4), bytes(uc.mem_read(sp + 4, 4 * min(c.nargs, 4))))), ())); m.exited = True; uc.emu_stop(); return
        args = struct.unpack("<%dI" % c.nargs, bytes(uc.mem_read(sp + 4, 4 * c.nargs))) if c.nargs else ()
        nat = getattr(c, "natural", None)      # cluster mode: a callee outside the cluster that is already natural C: arguments in registers / stack slots of the original, all on the stack on our side
        if nat:
            args = struct.unpack("<%dI" % len(nat), bytes(uc.mem_read(sp + 4, 4 * len(nat)))) if m.ours else tuple(uc.reg_read(REGS[t]) if t in REGS else struct.unpack("<I", bytes(uc.mem_read(sp + 4 + 4 * int(t[1:]), 4)))[0] for t in nat)
            m.trace.append((c.name, tuple(m.norm(a) for a in args), ()))
        else: m.trace.append((c.name, tuple(m.norm(a) for a in args), tuple(m.norm(uc.reg_read(REGS[r])) for r in c.regs)))
        h = int(sha(c.va, i, m.seed), 16); r = h >> 8
        outs = args
        if c.infer:      # no declared arguments (`f()`): the caller's `add esp, N` after the call tells how many words it pushed (used for the out parameters only, never compared: a compiler may batch the adds)
            ra = struct.unpack("<I", bytes(uc.mem_read(sp, 4)))[0]; code = bytes(uc.mem_read(ra, 40))
            for ins in CS.disasm(code, ra):      # the compiler may schedule a few instructions between the call and `add esp, N`
                if ins.mnemonic == "add" and ins.op_str.startswith("esp, "):
                    k = min(int(ins.op_str[5:], 0) // 4, 6); outs = struct.unpack("<%dI" % k, bytes(uc.mem_read(sp + 4, 4 * k))); break
                if ins.mnemonic in ("call", "ret", "push") or ins.mnemonic.startswith("j"): break
        for k, a in enumerate(outs):      # frame window: what the ORIGINAL callee would read behind a pointer to a local (a frame object whose members the callee walks: clump_has_uv_anim {flag, callback, flag pointer} passed as &callback)
            if isstack(a):
                snap = []
                nxt = min([(b - a) // 4 for b in outs if isstack(b) and b > a] + [FRAMEWIN])      # another pointer argument into the stack starts a separate object (RtIntersectionLineTriangle(&start, &delta, ..))
                for j in range(nxt):
                    w = a + 4 * j
                    if w + 4 > m.sp0 or not isstack(w): break      # the return address / arguments of the function itself are shared by the two sides
                    if (w & ~3) in m.sw:
                        v = struct.unpack("<I", bytes(uc.mem_read(w, 4)))[0]; snap.append((j, ("S", v - a) if isstack(v) else m.norm(v)))
                m.frames.append((c.name, k, tuple(snap)))
                if os.environ.get("FRAMEDBG"): sys.stderr.write("FRAME %s\n" % (("ours" if m.ours else "orig", c.name, k, hex(a), hex(sp), hex(m.sp0), snap, [hex(x) for x in outs]),))
        for k, a in enumerate(outs):      # out parameters: a dword behind every pointer to a local gets a deterministic value (else it is frame garbage, different in the two builds)
            if isstack(a): uc.mem_write(a, struct.pack("<I", int(sha(c.va, i, m.seed, k), 16) % 61))
        if c.ret == "f": uc.mem_write(FPSRC, struct.pack("<d", ((h >> 4) % 4000 - 2000) / 8.0))
        uc.reg_write(UC_X86_REG_EAX, (0, 1, u32(-1), r % 50, r % 6, r % 256, u32(r * 2654435761), u32(r))[h % 8])
        uc.reg_write(UC_X86_REG_EDX, u32(h >> 20)); uc.reg_write(UC_X86_REG_ECX, u32(h >> 12))
        uc.reg_write(UC_X86_REG_EIP, m.stubaddr(0 if nat and m.ours else c.pop, c.ret == "f"))

    def run(m, entry, args, regs, fill, lo, hi):
        """call entry (stack arguments `args`, initial registers `regs`, stack pre-fill bytes) until it returns to RET; dict of everything that is compared"""
        uc = m.uc; sp = STACK + STK - 0x200
        uc.mem_write(STACK, fill)
        uc.mem_write(sp, struct.pack("<%dI" % (1 + len(args)), RET, *[u32(x) for x in args]))
        for r, v in regs.items(): uc.reg_write(REGS[r], u32(v))
        uc.reg_write(UC_X86_REG_ESP, sp); m.blocks = {}; m.exited = False; m.sp0 = sp
        uc.hook_add(UC_HOOK_BLOCK, lambda uc, a, s, u: m.blocks.__setitem__(a, max(s, m.blocks.get(a, 0))), begin=lo, end=hi)
        err = None
        t0 = time.time()
        try: uc.emu_start(entry, RET, timeout=2000000, count=MAXINS)
        except UcError as e: err = e
        slow = time.time() - t0 >= 1.9      # wall clock timeout of the emulator (a loaded machine), not a loop: never a DIFF
        eip = uc.reg_read(UC_X86_REG_EIP); r = dict(done=((eip == RET or m.exited) and err is None), exited=m.exited, err=err, eip=eip, wild=m.wild)
        if not r["done"]:
            if err is None: c = "LOOP"
            elif err.errno == UC_ERR_FETCH_UNMAPPED: c = "INDIRECT_CALL"
            elif err.errno == UC_ERR_EXCEPTION: c = "EXCEPTION"
            elif err.errno in (UC_ERR_READ_UNMAPPED, UC_ERR_WRITE_UNMAPPED): c = "UNMAPPED"
            else: c = "OTHER"
            r["cause"] = "TIMEOUT" if slow and c == "LOOP" else ("WILD_" if m.wild and c in ("LOOP", "EXCEPTION") else "") + c; return r
        r.update(eax=uc.reg_read(UC_X86_REG_EAX), edx=uc.reg_read(UC_X86_REG_EDX), esp=uc.reg_read(UC_X86_REG_ESP) - sp,
                 saved=tuple(uc.reg_read(x) for x in (UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP)),
                 fptop=(uc.reg_read(UC_X86_REG_FPSW) >> 11) & 7, trace=m.trace, st0=None)
        if r["fptop"]:      # a value on the x87 stack: store it with all 80 bits
            uc.mem_write(RET + 0x100, b"\xdb\x3d" + struct.pack("<I", FPOUT))
            try: uc.emu_start(RET + 0x100, RET + 0x106); r["st0"] = bytes(uc.mem_read(FPOUT, 10))
            except UcError: pass
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
    def __init__(c, seed, mode, fill, args, regs, wins): c.seed, c.mode, c.fill, c.args, c.regs, c.wins = seed, mode, fill, args, regs, wins
    def describe(c): return "seed %d mode %d fill %s args %s" % (c.seed, c.mode, c.fill if isinstance(c.fill, str) else hex(c.fill), [hex(a) if isinstance(a, int) else a for a in c.args])

FILLS = (0xA5, 0x00, 0xFF, "rand")

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

def side(fn, case, ours):
    m = Machine(case.seed, case.mode, fn.lits if ours else b"", fn.callees, fn.imports, fn.islots); m.ours = ours; m.indargs = not getattr(fn, "noindargs", False)
    for a, b in case.wins: m.uc.mem_write(a, b)
    args, cregs = fn.mapcase(case, ours) if getattr(fn, "mapcase", None) else (case.args, case.regs)      # cluster mode: register arguments of the original, stack arguments of ours
    if getattr(fn, "pushn", 0) > len(args): args = list(args) + [int(sha(case.seed, "pad", i), 16) % 61 + (0x1000 if ours else 0) for i in range(len(args), fn.pushn)]      # a dropped stack argument: the stack slots the ORIGINAL callers push / its body reads are seeded beyond the declared parameters; OUR side gets different values there, so a C definition that declares fewer parameters (an undeclared slot reaches a callee through a tail jump, or the call drops it) differs at the stubbed callee
    r = random.Random(case.seed ^ 0xabcdef); regs = dict(INIT, eax=r.getrandbits(32), ecx=r.getrandbits(32), edx=r.getrandbits(32)); regs.update(cregs)
    fill = defloat(r.randbytes(STK - 0x200) if case.fill == "rand" else bytes([case.fill]) * (STK - 0x200))      # uninitialised locals read as floats: no NaN (0xFF fill) whose compare differs between `a < b` and `!(a >= b)` code
    if ours:
        code = bytearray(fn.code)
        if fn.mutate: code[fn.mutate[0]] = fn.mutate[1]
        m.uc.mem_write(OURS, bytes(code)); res = m.run(getattr(fn, "oentry", OURS), args, regs, fill, OURS, OURS + len(code))
    else: res = m.run(fn.va, args, regs, fill, getattr(fn, "olo", fn.va), fn.hi)
    return m, res

def covered(leaders, blocks): return {a for a in leaders if any(b <= a < b + s for b, s in blocks.items())} if blocks else set()

_FA = []
def frame_allow():
    """src/diffemu_frame_allow.txt: `function callee argindex reason`"""
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
    if ro["exited"] and ru["exited"]: ro = dict(ro, esp=0, eax=0, edx=0, fptop=0, st0=None, saved=ru["saved"]); ru = dict(ru, esp=0, eax=0, edx=0, fptop=0, st0=None)
    if getattr(fn, "espok", None):
        if not fn.espok(ro["esp"], ru["esp"]): d.append("esp delta orig %d ours %d (not a plain cdecl / stdcall return)" % (ro["esp"], ru["esp"]))
    elif ro["esp"] != ru["esp"]: d.append("esp delta orig %d ours %d (ret N)" % (ro["esp"], ru["esp"]))
    for nm, a, b in zip(("ebx", "esi", "edi", "ebp"), ro["saved"], ru["saved"]):
        if a != b and nm not in getattr(fn, "skipsaved", ()):      # cluster root: a register INPUT the original does not preserve (the entry thunk of a bridge restores it)
             d.append("%s orig %08x ours %08x" % (nm, a, b))
    if ro["fptop"] != ru["fptop"]: d.append("x87 stack top orig %d ours %d" % (ro["fptop"], ru["fptop"]))
    if sg.ret in ("f", "d"):
        if ro["st0"] != ru["st0"]: d.append("ST0 orig %s ours %s" % (ro["st0"] and ro["st0"].hex(), ru["st0"] and ru["st0"].hex()))
    elif sg.ret != "v":
        mask = {"i1": 0xff, "i2": 0xffff}.get(sg.ret, 0xffffffff)
        if ro["eax"] & mask != ru["eax"] & mask: d.append("eax orig %08x ours %08x" % (ro["eax"], ru["eax"]))
        if sg.ret == "ll" and ro["edx"] != ru["edx"]: d.append("edx orig %08x ours %08x" % (ro["edx"], ru["edx"]))
    a = [t for t in ro["trace"]]; b = [t for t in ru["trace"]]
    if a != b:
        k = next((i for i in range(min(len(a), len(b))) if a[i] != b[i]), min(len(a), len(b)))
        d.append("call trace differs at call %d (orig %d calls, ours %d): orig %s ours %s" % (k, len(a), len(b), a[k] if k < len(a) else "-", b[k] if k < len(b) else "-"))
    if len(mo.frames) == len(mu.frames):      # frame windows behind pointer arguments into the stack (the stubbed callee never dereferences them, the original callee does)
        for n, (fo, fu) in enumerate(zip(mo.frames, mu.frames)):
            if fo[:2] != fu[:2]: break
            do, du = dict(fo[2]), dict(fu[2])
            if (fn.name, fo[0], fo[1]) in frame_allow(): continue      # src/diffemu_frame_allow.txt: a frame window difference that was checked against the original callee (it reads fewer words than the window)
            bad = [j for j in sorted(do) if do[j] != du.get(j, "-")]      # initialised by the ORIGINAL and different / uninitialised in ours (words only ours initialised: ignored, unrelated locals)
            if bad: d.append("frame object behind stack pointer argument %d of call %s #%d differs at word(s) %s: orig %s ours %s" % (fo[1], fo[0], n, bad, [do[j] for j in bad], [du.get(j, "-") for j in bad])); break
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

def run_case(fn, case):
    """-> ('skip', cause) | ('equal', None) | ('diff', [descriptions]); updates the coverage sets"""
    mo, ro = side(fn, case, False)
    if not ro["done"]: return "skip", ro["cause"]
    fn.cov_o |= covered(fn.leaders_o, mo.blocks)
    mu, ru = side(fn, case, True)
    fn.cov_u |= covered(fn.leaders_u, mu.blocks)
    if not ru["done"] and ru["cause"] == "TIMEOUT": return "skip", "TIMEOUT"
    if not ru["done"]: return "diff", ["ours does not return (%s, eip %08x%s) while the original does" % (ru["cause"], ru["eip"], ", " + str(ru["err"]) if ru["err"] else "")]
    d = compare(fn, mo, ro, mu, ru)
    return ("diff", d) if d else ("equal", None)
