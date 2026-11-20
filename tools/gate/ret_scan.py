#!/usr/bin/env python3
"""ret_scan.py: RET detector (W17 DET).  The v143-only bug class of W16: a recomp function that does not leave the value its callers read
(declared void or narrower than they read, or no EAX on some path), e.g. _RwaThreadCreate (race-load hang), rwp_sphere_0056cc50 (hull crash),
the 19 vr1 rows.  cl 13 happened to leave the value in EAX; v143 /O2 does not.

  ret_scan.py <ours.exe> <ours.map> [--src <recomp dir>] [--orig <MFL.exe>] [--allow <file>] [--out <report>] [--fn <name>]
  ret_scan.py --selftest

Callee side (v143 exe, functions above the image, 0x5d7000): the declared return width of the C definition (void 0, char/bool 8,
short 16, float/double ST0, else 32; the definition in the file named like the map's .obj wins, several there (#if forms) = the widest;
__declspec(naked) / asm body / no definition = binary only).  void = gives nothing, whatever EAX holds.  Otherwise a forward walk: EAX width
defined on every path to every `ret` (a call defines EAX; xor/mov/movzx eax = 32, mov ax = 16, setcc/mov al = 8 unless EAX was already
fully written), and for a tail `jmp` the target's width (image thunks resolved).
Need side: (1) FAIL: a v143 call site reads more than F gives (first EAX-family / ST0 event after `call F`; an x87 use with the caller's own
x87 value still pushed does not count); a caller that returns F's EAX untouched passes its own callers' need on (depth 5).
(2) FPTR: F's original address is a dword in the original's data, an original `call [reg+slot]` of that slot offset reads EAX (offset
unknown = assumed), every original exit leaves EAX written by F itself, and F gives < 32 (vtable / callback slots: rwp_sphere_0056cc50).
(3) WARN only: original call sites read more (origuse.py logic) but no v143 caller does; or an FPTR candidate whose slot nobody reads.
Rows of void functions carry "v143 EAX EQUALS / DIFFERS from the original's today" (register value numbering, eax_sigs): EQUALS = works by
luck today, DIFFERS = wrong value now (e.g. rwp_sphere_0056cc50 hang build: imm:0x1 vs arg@16).
--fn: the evidence of one function, no verdict.  Allow-list: `<name> <reason>` lines; an allowed row prints as ALLOW.
Exit 1 = a FAIL / FPTR row outside the allow-list, 2 = input error."""
import bisect, os, re, struct, subprocess, sys

G = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
ORIG = os.environ.get("MFL_ORIG_EXE") or os.path.join(os.path.dirname(G), "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")      # the user's own install, next to this tree in the private layout
PFX = ("rep", "repz", "repnz", "repe", "repne", "lock", "bnd", "notrack", "data16", "addr16", "ds", "cs", "es", "ss", "fs", "gs")
LINE = re.compile(r"\s*([0-9a-f]+):\s+(\S+)\s*(.*)")
HEX = re.compile(r"(?:0x)?([0-9a-f]+)")
R8 = re.compile(r"\b(eax|ax|al|ah)\b")
WR = {"eax": 32, "ax": 16, "al": 8, "ah": 16}
NOWRITE = {"cmp", "test", "push", "bt", "out"}
X87USE = {"fstp", "fst", "fcomp", "fcom", "fcompp", "fucomp", "fucompp", "fucom", "fxch", "fadd", "faddp", "fmul", "fmulp", "fsub", "fsubp",
          "fsubr", "fsubrp", "fdiv", "fdivp", "fdivr", "fdivrp", "fistp", "fist", "fisttp", "fchs", "fabs", "fsqrt", "ftst", "fcomi", "fcomip",
          "fucomi", "fucomip", "frndint", "fsin", "fcos", "fptan", "fpatan", "fscale", "fprem", "fyl2x"}


def parse(lines):
    """objdump -d -M intel text -> [(addr, mnem, ops)]"""
    out = []
    for l in lines:
        m = LINE.match(l)
        if not m: continue
        mn, op = m.group(2), m.group(3).split("<")[0].strip()
        while mn in PFX and op:
            p = op.split(None, 1); mn, op = p[0], p[1] if len(p) > 1 else ""
        out.append((int(m.group(1), 16), mn, op))
    return out


def objdump(exe):
    r = subprocess.run(["objdump", "-d", "-M", "intel", "--no-show-raw-insn", "-j", ".text", exe], capture_output=True, text=True)
    if r.returncode: sys.exit("objdump failed on %s" % exe)
    return parse(r.stdout.splitlines())


def tgt(op):
    m = HEX.fullmatch(op)
    return int(m.group(1), 16) if m else None


class Code:
    """an instruction list with function starts (sorted) and direct call/jmp indexes"""
    def __init__(self, ins, starts):
        self.ins, self.at = ins, {a: i for i, (a, _, _) in enumerate(ins)}
        self.starts = sorted(starts)
        self.callers = {}      # target -> [index of call/jmp]
        for i, (a, mn, op) in enumerate(ins):
            if mn in ("call", "jmp"):
                t = tgt(op)
                if t is not None: self.callers.setdefault(t, []).append(i)

    def fn_of(self, a):
        k = bisect.bisect_right(self.starts, a) - 1
        return self.starts[k] if k >= 0 else None

    def end_of(self, f):
        k = bisect.bisect_right(self.starts, f)
        return self.starts[k] if k < len(self.starts) else f + 0x10000


def first_event(code, j, f, seen, budget):
    """first EAX/ST0 event on the path from code.ins[j] inside function f: ('READ', w, txt) | ('X87', 0, txt) | ('TAIL',) | ('CLOB',)"""
    ins, end = code.ins, code.end_of(f)
    while j < len(ins) and budget > 0:
        if j in seen: return ("CLOB",)
        seen.add(j); budget -= 1
        a, mn, op = ins[j]
        txt = "%x %s %s" % (a, mn, op)
        if a >= end: return ("CLOB",)
        if mn == "ret": return ("TAIL",)
        if mn == "jmp":
            t = tgt(op)
            if t is not None and f <= t < end and t in code.at: j = code.at[t]; continue
            return ("CLOB",)      # a tail jump to another function: it makes its own EAX
        if mn == "call" or mn in ("int3", "hlt", "ud2"): return ("CLOB",)
        if mn[0] == "f":
            if mn.startswith(("fld", "fild")): return ("CLOB",)
            if mn in X87USE: return ("X87", 0, txt)
        if mn.startswith("j"):
            r = [first_event(code, j + 1, f, seen, budget)]
            t = tgt(op)
            if t is not None and t in code.at: r.append(first_event(code, code.at[t], f, seen, budget))
            for k in ("READ", "X87", "TAIL"):
                c = [x for x in r if x[0] == k]
                if c: return max(c, key=lambda x: x[1] if len(x) > 1 else 0)
            return ("CLOB",)
        if mn == "cdq": return ("READ", 32, txt)
        if mn in ("cwde", "cwd"): return ("READ", 16, txt)
        if mn == "cbw": return ("READ", 8, txt)
        if mn in ("mul", "div", "idiv") or (mn == "imul" and "," not in op): return ("READ", 32, txt)
        p = op.split(",", 1)
        dst, src = p[0], p[1] if len(p) > 1 else ""
        if mn in ("xor", "sub", "sbb") and dst == src and R8.fullmatch(dst): return ("CLOB",)
        if R8.fullmatch(dst) and ((mn == "or" and src in ("0xffffffff", "0xffff", "0xff")) or (mn == "and" and src == "0x0")): return ("CLOB",)
        if mn == "imul" and op.count(",") == 2: src = src.split(",")[0]; mn = "mov"      # imul r, r/m, imm: writes r only
        reads = [WR[x] for x in R8.findall(src)]
        if "[" in dst: reads += [32 for _ in R8.findall(dst)]      # eax as an address
        elif R8.fullmatch(dst) and (mn not in ("mov", "lea", "movzx", "movsx", "pop", "cvttss2si", "cvttsd2si", "cvtss2si", "cvtsd2si")
                                     and not mn.startswith(("set", "cmov"))):
            reads.append(WR[dst])
        if mn.startswith("cmov") and R8.fullmatch(dst): reads.append(WR[dst])
        if reads: return ("READ", max(reads), txt)
        if R8.fullmatch(dst): return ("CLOB",)
        j += 1
    return ("CLOB",)


def writes(mn, op):
    """EAX width this instruction defines (0 = none; 31 = full, by a call)"""
    if mn == "call": return 31
    if mn in ("cwde", "mul", "div", "idiv", "rdtsc", "cpuid") or (mn == "imul" and "," not in op): return 32
    p = op.split(",", 1)
    dst = p[0]
    if mn == "xchg" and len(p) > 1 and R8.fullmatch(p[1]): dst = p[1]
    if mn in NOWRITE or not R8.fullmatch(dst): return 0
    return 32 if dst == "eax" else 16 if dst == "ax" else 8 if dst == "al" else 0


def exits(code, f):
    """forward walk of function f: [(kind, eax width, target)] for every reachable ret / tail jmp (kind 'ret' | 'jmp' | 'ijmp')"""
    ins, end = code.ins, code.end_of(f)
    if f not in code.at: return []
    best, out, work = {}, {}, [(code.at[f], 0)]
    while work:
        j, w = work.pop()
        while j < len(ins):
            a, mn, op = ins[j]
            if a >= end or (j in best and best[j] <= w): break
            best[j] = w
            if mn in ("int3", "hlt", "ud2"): break
            if mn == "ret": out[j] = ("ret", w, None); break
            if mn == "jmp":
                t = tgt(op)
                if t is None:
                    m = re.search(r"\[(?:e\w\w)\*4\+0x([0-9a-f]+)\]", op)      # a switch table: its entries inside f are the successors
                    if m:
                        ta = int(m.group(1), 16)
                        for k in range(256):
                            e = code.rd(ta + 4 * k) if hasattr(code, "rd") else None
                            if e is None or not (f <= e < end) or e not in code.at: break
                            work.append((code.at[e], w))
                    else: out[j] = ("ijmp", w, None)
                    break
                if f <= t < end and t in code.at: j = code.at[t]; continue
                out[j] = ("jmp", w, t); break
            if mn.startswith("j"):
                t = tgt(op)
                if t is not None and f <= t < end and t in code.at: work.append((code.at[t], w))
            d = writes(mn, op)
            if d: w = d if d >= 31 else w if w >= 31 else max(w, d)
            j += 1
    return list(out.values())


def x87_depth(code, i, names):
    """x87 stack depth the caller holds at the call code.ins[i] (straight-line scan back to the previous call / branch; _ftol* helpers pop one).
    > 0: the caller keeps its own x87 value across a callee it knows (same TU), so an x87 use after the call is not the callee's ST0."""
    d, j = 0, i - 1
    while j >= 0 and j > i - 40:
        a, mn, op = code.ins[j]
        if mn == "call":
            if "ftol" not in names.get(tgt(op), ""): break
            d -= 1
        elif mn.startswith("j") or mn in ("ret", "int3"): break
        elif mn[0] == "f":
            if mn.startswith(("fld", "fild")): d += 1
            elif mn in ("fcompp", "fucompp"): d -= 2
            elif mn.endswith("p") and mn not in ("fdecstp",): d -= 1
        j -= 1
    return d


REGS = ("eax", "ecx", "edx", "ebx", "esi", "edi", "ebp")
STK = re.compile(r"DWORD PTR \[(esp|ebp)(?:\+0x([0-9a-f]+))?\]$")


def eax_sigs(code, f, tailname=lambda t: t):
    """what EAX holds at each exit of f, as comparable signatures: 'arg@N' (incoming stack argument at entry offset N), 'imm:V', 'call',
    'tail:<original target>', 'ijmp', or 'op@addr' (anything else: never equal across two binaries).  A small value numbering of the 7
    registers along every path (esp/ebp offsets tracked).  None = too many paths (gave up)."""
    ins, end = code.ins, code.end_of(f)
    if f not in code.at: return None
    out, seen, work, n = set(), set(), [(code.at[f], 0, None, tuple("in:" + r for r in REGS))], 0
    while work:
        j, P, Pb, rs = work.pop()
        while j < len(ins):
            a, mn, op = ins[j]
            key = (j, rs)
            if a >= end or key in seen: break
            seen.add(key); n += 1
            if n > 20000: return None
            if mn in ("int3", "hlt", "ud2"): break
            if mn == "ret": out.add(rs[0]); break
            if mn == "jmp":
                t = tgt(op)
                if t is not None and f <= t < end and t in code.at: j = code.at[t]; continue
                out.add("tail:%x" % tailname(t) if t is not None else "ijmp"); break
            if mn.startswith("j"):
                t = tgt(op)
                if t is not None and f <= t < end and t in code.at: work.append((code.at[t], P, Pb, rs))
            p = op.split(",", 1)
            dst, src = p[0], p[1] if len(p) > 1 else ""
            r = list(rs)
            if mn == "push": P += 4
            elif mn == "pop":
                P -= 4
                if dst in REGS: r[REGS.index(dst)] = "op@%x" % a
            elif mn in ("sub", "add") and dst == "esp" and HEX.fullmatch(src): P += int(HEX.fullmatch(src).group(1), 16) * (1 if mn == "sub" else -1)
            elif mn == "mov" and dst == "ebp" and src == "esp": Pb = P; r[6] = "frame"
            elif mn == "call": r[0], r[1], r[2] = "call", "op@%x" % a, "op@%x" % a
            elif dst in REGS:
                k, v = REGS.index(dst), "op@%x" % a
                m = STK.match(src)
                if mn == "mov" and m:
                    N = int(m.group(2) or "0", 16)
                    if m.group(1) == "esp": v = "arg@%d" % (N - P)
                    elif Pb is not None: v = "arg@%d" % (N - Pb)
                elif mn == "mov" and src in REGS: v = rs[REGS.index(src)]
                elif mn == "mov" and HEX.fullmatch(src): v = "imm:" + src
                elif mn in ("xor", "sub") and src == dst: v = "imm:0"
                r[k] = v
            elif mn == "xchg" and dst in REGS and src in REGS:
                x, y = REGS.index(dst), REGS.index(src); r[x], r[y] = rs[y], rs[x]
            rs = tuple(r)
            j += 1
    return out


class Scan:
    def __init__(self, code, decl, thunk, names=None):
        self.code, self.decl, self.thunk, self.memo, self.names = code, decl, thunk, {}, names or {}
        self.into = {}      # resolved target -> thunks that jump to it
        for t in thunk: self.into.setdefault(self.res(t), []).append(t)

    def res(self, t):
        n = 0
        while t in self.thunk and n < 8: t = self.thunk[t]; n += 1
        return t

    def eff(self, f, depth=0):
        """(eax width, st0) F leaves on every path"""
        f = self.res(f)
        if f in self.memo: return self.memo[f]
        self.memo[f] = (32, True)      # cycle / depth guard: assume defined
        if depth > 6: return (32, True)
        d = self.decl.get(f)
        if d == 0:      # declared void: no value, whatever EAX holds (a tail jmp that forwards the callee's EAX works by luck only)
            self.memo[f] = (0, False)
            return self.memo[f]
        w, st = 32, True
        for kind, ew, t in exits(self.code, f):
            if kind == "jmp": tw, tst = self.eff(t, depth + 1)
            elif kind == "ijmp": tw, tst = 32, True
            else: tw, tst = (32 if ew >= 31 else ew), (d is None or d == "st")
            if d == "st": tw = 32
            w, st = min(w, tw), st and tst
        self.memo[f] = (w, st)
        return self.memo[f]

    def need(self, f, depth=0, seen=None):
        """(width read, st0 read, evidence) by the callers of f in this code (TAIL chains followed)"""
        seen = seen if seen is not None else set()
        w, st, ev = 0, False, []
        tg = self.into.get(f, []) + [f]
        for t in tg:
            for i in self.code.callers.get(t, []):
                a, mn, _ = self.code.ins[i]
                g = self.code.fn_of(a)
                if g is None or g in self.thunk: continue
                e = ("TAIL",) if mn == "jmp" else first_event(self.code, i + 1, g, set(), 60)
                if e[0] == "READ" and e[1] > w: w, ev = e[1], ["%x in %x: %s" % (a, g, e[2])] + ev
                elif e[0] == "X87" and not st and x87_depth(self.code, i, self.names) == 0: st = True; ev.append("%x in %x: %s" % (a, g, e[2]))
                elif e[0] == "TAIL" and depth < 5 and g not in seen and g != f and self.decl.get(g) != 0:      # a void g's callers are g's own row
                    seen.add(g)
                    gw, gst, gev = self.need(g, depth + 1, seen)
                    if gw > w: w, ev = gw, ["via %x" % g + (": " + gev[0] if gev else "")] + ev
                    if gst and not st: st = True; ev.append("via %x" % g)
        return w, st, ev


# ---------------------------------------------------------------- inputs
def read_map(p):
    """[(name, va, obj)] of the 'f' rows"""
    out = []
    for l in open(p, errors="replace"):
        q = l.split()
        if len(q) >= 4 and ":" in q[0] and q[3] == "f" and re.fullmatch(r"[0-9a-f]{8}", q[2]):
            n = re.sub(r"@\d+$", "", q[1]); n = n[1:] if n[:1] in "_@" else n
            out.append((n, int(q[2], 16), q[-1]))
    return out


KW = re.compile(r"\b(static|extern|inline|__inline|__forceinline|__cdecl|__stdcall|__fastcall|__thiscall|const|volatile|register|WINAPI|APIENTRY|CALLBACK|RWAPI|__declspec_\w+)\b")
DEFRE = re.compile(r"^((?:[A-Za-z_][\w\*]*[ \t\*\n]+)+?)(\w+)[ \t]*\(", re.M)      # type words may end the line before the name
NOTTYPE = {"return", "else", "if", "while", "for", "switch", "sizeof", "case", "goto", "do", "typedef", "define"}
W8 = {"char", "signed char", "unsigned char", "uchar", "byte", "bool", "_Bool", "RwUInt8", "RwInt8", "RwChar", "undefined", "undefined1", "BYTE", "UCHAR"}
W16 = {"short", "unsigned short", "signed short", "ushort", "RwUInt16", "RwInt16", "undefined2", "wchar_t", "WORD", "USHORT", "short int", "unsigned short int"}
WST = {"float", "double", "long double", "RwReal", "float10", "FLOAT"}


def width_of(ty):
    naked = "naked" in ty
    t = re.sub(r"\s+", " ", KW.sub(" ", ty)).strip()
    if naked: return None
    if "*" in t: return 32
    if t == "void": return 0
    if t in W8: return 8
    if t in W16: return 16
    if t in WST: return "st"
    return 32


def read_src(root):
    """name -> {file base: [widths]} of every C definition under root"""
    out = {}
    for d, _, files in os.walk(root):
        for n in files:
            if not n.endswith(".c"): continue
            s = re.sub(r"__declspec\s*\(\s*naked\s*\)", "__declspec_naked", open(os.path.join(d, n), errors="replace").read())
            for m in DEFRE.finditer(s):
                ty, name = m.group(1), m.group(2)
                if NOTTYPE & set(re.findall(r"\w+", ty)) or name in NOTTYPE: continue
                k, depth = m.end(), 1
                while k < len(s) and depth:
                    depth += {"(": 1, ")": -1}.get(s[k], 0); k += 1
                rest = re.sub(r"/\*.*?\*/|//[^\n]*", "", s[k:k + 200], flags=re.S).lstrip()
                if rest[:1] != "{": continue
                b = s.find("{", k)
                e, depth = b + 1, 1
                while e < len(s) and depth:
                    depth += {"{": 1, "}": -1}.get(s[e], 0); e += 1
                asm = re.search(r"\b_?_asm\b|\basm\s*\(|__emit", s[b:e])      # an asm body sets EAX itself: the binary decides
                out.setdefault(name, {}).setdefault(n[:-2].lower(), []).append(None if asm else width_of(ty))
    return out


def pick(defs, obj):
    """declared width of the definition linked from <obj>: same-named file first, else the only def, else the widest"""
    def wide(ws):
        if None in ws: return None
        return "st" if "st" in ws else max(ws)
    b = obj.lower().rsplit(".", 1)[0]
    if b in defs: return wide(defs[b])
    allw = [w for ws in defs.values() for w in ws]
    return wide(allw) if allw else None


class PE:
    def __init__(self, p):
        self.b = open(p, "rb").read()
        pe = struct.unpack_from("<I", self.b, 0x3c)[0]
        nsec, opt = struct.unpack_from("<H", self.b, pe + 6)[0], struct.unpack_from("<H", self.b, pe + 20)[0]
        self.base = struct.unpack_from("<I", self.b, pe + 52)[0]
        self.secs = []
        for i in range(nsec):
            o = pe + 24 + opt + i * 40
            vs, va, rs, ro = struct.unpack_from("<IIII", self.b, o + 8)
            ch = struct.unpack_from("<I", self.b, o + 36)[0]
            self.secs.append((self.base + va, max(vs, rs), ro, rs, bool(ch & 0x20)))

    def rd(self, va):
        for s, sz, ro, rs, _ in self.secs:
            if s <= va < s + min(sz, rs) - 3: return struct.unpack_from("<I", self.b, ro + va - s)[0]
        return None

    def data_dwords(self):
        """aligned dword value -> [VA where it is stored] in the non-code sections"""
        out = {}
        for s, sz, ro, rs, code in self.secs:
            if code: continue
            for k, v in enumerate(struct.unpack_from("<%dI" % (rs // 4), self.b, ro)):
                if 0x401000 <= v < 0x5d7000: out.setdefault(v, []).append(s + 4 * k)
        return out


def slot_readers(code):
    """(sorted code immediates, {slot offset: True if some indirect `call [reg+X]` site reads EAX after it})"""
    imms, rd = set(), {}
    for i, (a, mn, op) in enumerate(code.ins):
        for h in re.findall(r"0x([0-9a-f]{6,8})\b", op): imms.add(int(h, 16))
        m = re.fullmatch(r"DWORD PTR \[e\w\w(?:\+0x([0-9a-f]+))?\]", op) if mn == "call" else None
        if m:
            x = int(m.group(1) or "0", 16)
            e = first_event(code, i + 1, code.fn_of(a) or a, set(), 60)
            rd[x] = rd.get(x, False) or e[0] == "READ"
    return sorted(imms), rd


def slot_of(p, imms):
    """offset of table slot p from the nearest table base the code names (push / mov of an immediate), None if none within 0x100"""
    k = bisect.bisect_right(imms, p) - 1
    return p - imms[k] if k >= 0 and p - imms[k] <= 0x100 else None


def orig_valuelike(code, f, depth=0, memo=None):
    """every original exit of f leaves a value: EAX fully written by f itself (not by a call) on every path to a ret, or a value tail"""
    memo = memo if memo is not None else {}
    if f in memo: return memo[f]
    memo[f] = False
    if depth > 4: return False
    ex = exits(code, f)
    memo[f] = bool(ex) and all(w == 32 if k == "ret" else k == "ijmp" or orig_valuelike(code, t, depth + 1, memo) for k, w, t in ex)
    return memo[f]


def run(exe, mapp, src, orig, allow, outp, fn=None):
    pe = PE(exe)
    funcs = read_map(mapp)
    code = Code(objdump(exe), {va for _, va, _ in funcs})
    code.rd = pe.rd
    name, obj = {}, {}
    for n, va, o in funcs:
        if not n.endswith("_orig"): name.setdefault(va, n); obj.setdefault(va, o)
    thunk, img_end = {}, 0x5d7000
    defs = read_src(src)
    for va in code.starts:
        if va >= img_end and name.get(va) in defs: continue      # a recomp `jmp` body (tail call) keeps its declared type
        i = code.at.get(va)
        if i is not None and code.ins[i][1] == "jmp" and tgt(code.ins[i][2]) is not None: thunk[va] = tgt(code.ins[i][2])
    decl, nodef = {}, 0
    for va, n in name.items():
        if va in thunk or va < img_end: continue      # below img_end: image thunks / the image's own library code, not v143-compiled
        if n in defs: decl[va] = pick(defs[n], obj[va])
        else: nodef += 1
    sc = Scan(code, decl, thunk, name)
    origva = {}      # recomp va -> original va (the image thunk that jumps to it)
    for a, t in thunk.items():
        if a < img_end: origva.setdefault(sc.res(t), a)
    oc = osc = None
    ddw = set()
    if orig and os.path.exists(orig):
        oc = Code(objdump(orig), [a for a in code.starts if a < img_end])
        osc = Scan(oc, {}, {})
        ope = PE(orig)
        oc.rd, ddw = ope.rd, ope.data_dwords()
        imms, sread = slot_readers(oc)
    allowed = {}
    if allow and os.path.exists(allow):
        for l in open(allow):
            q = l.split(None, 1)
            if q and not q[0].startswith("#"): allowed[q[0]] = q[1].strip() if len(q) > 1 else ""
    if fn:      # --fn NAME: the evidence of one function, no verdict
        for va, n in name.items():
            if n != fn: continue
            print("%s %08x obj %s thunk->%s defs %s decl %s eff %s exits %s" % (n, va, obj[va], "%x" % thunk[va] if va in thunk else "-",
                  defs.get(n), decl.get(va, "-"), sc.eff(va), exits(code, sc.res(va))[:6]))
            print("  v143 need", sc.need(sc.res(va)), "callers", ["%x" % code.ins[i][0] for t in sc.into.get(sc.res(va), []) + [sc.res(va)] for i in code.callers.get(t, [])][:10])
            ov = origva.get(sc.res(va))
            if oc is not None and ov:
                print("  orig %x in data %s valuelike %s need %s exits %s" % (ov, ov in ddw, orig_valuelike(oc, ov), osc.need(ov), exits(oc, ov)[:6]))
        return 0
    rows, nfail = [], 0
    for va in sorted(decl):
        n = name[va]
        w, st = sc.eff(va)
        if w == 32 and st: continue
        nw, nst, ev = sc.need(va)
        kind, why = None, ""
        if nw > w or (nst and not st):
            kind, why = "FAIL", "v143 caller reads %s: %s" % ("ST0" if nst and not st and nw <= w else "%d bit" % nw, ev[0] if ev else "")
        ov = origva.get(va)
        if kind is None and oc is not None and ov is not None and w < 32 and ov in ddw and orig_valuelike(oc, ov):
            sl = [slot_of(p, imms) for p in ddw[ov]]
            rd = [x for x in sl if x is None or sread.get(x)]
            where = " ".join("%x%s" % (p, "" if x is None else "+%x" % x) for p, x in zip(ddw[ov], sl))
            if rd: kind, why = "FPTR", "original %x is a table slot (%s) whose original dispatch reads EAX%s, and returns a value on every exit" % (
                ov, where, " (slot offset unknown)" if None in rd else "")
            else: kind, why = "WARN", "original %x is a table slot (%s), no original `call [reg+slot]` reads EAX" % (ov, where)
        if kind is None and oc is not None and ov is not None:
            ow, ost, oev = osc.need(ov)
            if ow > w or (ost and not st and decl.get(va) == 0):
                kind, why = "WARN", "original caller reads %s: %s (no v143 caller does)" % ("ST0" if ow <= w else "%d bit" % ow, oev[0] if oev else "")
        if kind is None: continue
        if oc is not None and ov is not None and decl.get(va) == 0:      # today's luck: v143 EAX = the original's EAX on every exit?
            os_, vs_ = eax_sigs(oc, ov), eax_sigs(code, va, lambda t: origva.get(sc.res(t), t))
            why += " | v143 EAX %s the original's today (%s vs %s)" % ("EQUALS" if os_ and os_ == vs_ else "DIFFERS from",
                                                                      ",".join(sorted(vs_ or ["?"])), ",".join(sorted(os_ or ["?"])))
        dd = decl.get(va)
        line = "%s %s %08x decl=%s gives=%d%s orig=%s | %s" % (kind, n, va, "naked/none" if dd is None else dd, w, "+st0" if st else "",
                                                            "%x" % ov if ov else "-", why)
        if kind in ("FAIL", "FPTR") and n in allowed: line = "ALLOW " + line + " | allow: " + allowed[n]
        elif kind in ("FAIL", "FPTR"): nfail += 1
        rows.append(line)
    rows.sort(key=lambda x: ({"FAIL": 0, "FPTR": 1, "ALLOW": 2, "WARN": 3}[x.split()[0]], x))
    cnt = {k: sum(r.startswith(k + " ") for r in rows) for k in ("FAIL", "FPTR", "ALLOW", "WARN")}
    summ = "ret_scan: %d functions with a C definition (%d map functions without one): %d FAIL, %d FPTR, %d ALLOW, %d WARN -> %s" % (
        len(decl), nodef, cnt["FAIL"], cnt["FPTR"], cnt["ALLOW"], cnt["WARN"], "FAIL" if nfail else "PASS")
    text = "\n".join(rows + [summ]) + "\n"
    if outp: open(outp, "w").write(text)
    print("\n".join([r for r in rows if r.startswith(("FAIL", "FPTR"))][:40] + [summ]))
    return 1 if nfail else 0


# ---------------------------------------------------------------- self-test: planted bad functions must be flagged
SELF = """
 1000:	mov    eax,DWORD PTR [esp+0x4]
 1004:	mov    DWORD PTR [eax],0x0
 1008:	ret
 1010:	push   0x1
 1012:	call   0x1000
 1017:	add    esp,0x4
 101a:	test   eax,eax
 101c:	ret
 1020:	xor    eax,eax
 1022:	cmp    ecx,0x2
 1025:	sete   al
 1028:	ret
 1030:	cmp    ecx,0x2
 1033:	sete   al
 1036:	ret
 1040:	call   0x1020
 1045:	test   eax,eax
 1047:	call   0x1030
 1048:	test   eax,eax
 1049:	call   0x1050
 104a:	push   eax
 104b:	ret
 1050:	jmp    0x1000
 1060:	test   ecx,ecx
 1062:	je     0x1069
 1064:	mov    eax,0x1
 1069:	ret
 1070:	call   0x1060
 1075:	mov    ecx,eax
 1077:	call   0x1080
 107c:	fstp   DWORD PTR [esp]
 107f:	ret
 1080:	mov    DWORD PTR [ecx],0x0
 1086:	ret
"""


def selftest():
    ins = parse(SELF.splitlines())
    ins.sort()
    code = Code(ins, [0x1000, 0x1010, 0x1020, 0x1030, 0x1040, 0x1050, 0x1060, 0x1070, 0x1080, 0x1090])
    # 1000 void (planted _RwaThreadCreate: EAX = a temp), 1020 bool with xor (ok), 1030 bool without xor (narrow), 1050 int tail to void,
    # 1060 int with a path that sets nothing, 1080 void read as float
    sc = Scan(code, {0x1000: 0, 0x1020: 8, 0x1030: 8, 0x1050: 32, 0x1060: 32, 0x1080: 0}, {})
    got = {}
    for f in (0x1000, 0x1020, 0x1030, 0x1050, 0x1060, 0x1080):
        w, st = sc.eff(f); nw, nst, _ = sc.need(f)
        got[f] = nw > w or (nst and not st)
    want = {0x1000: True, 0x1020: False, 0x1030: True, 0x1050: True, 0x1060: True, 0x1080: True}
    assert got == want, (got, want)
    assert width_of("static __inline void") == 0 and width_of("__declspec(naked) int") is None and width_of("RwCamera *") == 32
    assert width_of("unsigned short") == 16 and width_of("float") == "st" and width_of("bool") == 8
    print("ret_scan selftest: OK (6 planted functions, 5 flagged, the xor/setcc bool passes)")
    return 0


if __name__ == "__main__":
    a = sys.argv[1:]
    if a == ["--selftest"]: sys.exit(selftest())
    if len(a) < 2: sys.exit(__doc__)
    opt = {"--src": G + "/src", "--orig": ORIG, "--allow": os.path.join(os.path.dirname(os.path.abspath(__file__)), "ret_allow.txt"), "--out": None, "--fn": None}
    k = 2
    while k < len(a):
        if a[k] not in opt or k + 1 >= len(a): sys.exit("unknown or incomplete option %r" % a[k])
        opt[a[k]] = a[k + 1]; k += 2
    for p in (a[0], a[1], opt["--src"]):
        if not os.path.exists(p): print("missing input " + p); sys.exit(2)
    sys.exit(run(a[0], a[1], opt["--src"], opt["--orig"], opt["--allow"], opt["--out"], opt["--fn"]))
