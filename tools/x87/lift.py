#!/usr/bin/env python3
"""lift.py VA [--name N] [--out F]: lift an x87-heavy function of MFL.exe (read from the exe bytes, objdump) to readable C in the shape cl 13.00 /G5 generates.
Symbolic execution of the integer registers and the x87 stack, structured `if` / `if-else` regions, `return`; loops, switch tables, indirect calls and register arguments other than the pointer
levers below raise Unsupported (the caller records the reason).
Rules (BYTEMATCH_PATTERNS.md RWP-X1..):
  * every `fstp [mem]` is one statement; an x87 stack value that is used twice (fld st(i)) or survives a store/call/branch is a float temp `fN`, declared and defined where it was pushed (declaration order = push order,
    the original often computes z, y, x);
  * a pointer loaded from memory (`mov r,[m]`) is a pointer local `pN`, a computed base (shl/add/lea) used as an address becomes a local at its first use; float-only bases are `float *` (index form `p[3]`),
    other bases are ints with byte offsets;
  * operand order follows the instruction (`fsubr [m]` = m - top); register forms are decoded from the opcode bytes (objdump's intel syntax swaps fsub/fsubr for the st(i) destination forms);
  * constants read from .rdata become float literals (pattern 4/13); sqrt/sin/cos/atan2 are `(float)sqrt(x)` etc.
  * T-X88 (RWP-X8..X13): address-taken frame slots become arrays up to the next slot / frame top; neg+sbb+neg, sbb r,r, setcc, shift by cl, imul r,r, rep movs (struct copy `BlkN`) and
    float-stack joins (temp assigned in each arm) are lifted; esp read as a value (by-value struct argument) stays Unsupported;
  * mutators for the permuter: tools/x87/mutators.py (they work on this exact text shape)."""
import os, re, struct, sys
HERE = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, os.path.join(HERE, "..")); sys.path.insert(0, HERE)
import cmpfn
from disx import dis

class Unsupported(Exception): pass
READDR = int(os.environ.get('X87_READDR', '0'))      # T-X90 (RWP-X20): computed bases (index << k + base) are re-addressed at every use instead of cached in a pointer local (fixes the eax/ecx roles, rwp_farfield_00576bf0)
PAREN = int(os.environ.get('X87_PAREN', '0'))      # T-X89: parenthesise the left part of 3+ term sum chains (RWP-X14)

REGS = ["eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"]
SUBREG = {"ax": "eax", "cx": "ecx", "dx": "edx", "bx": "ebx", "al": "eax", "cl": "ecx", "dl": "edx", "bl": "ebx", "ah": "eax"}
SYMS = {}
def symbols():
    if SYMS: return SYMS
    for l in [x for f in ("symbols.txt", "aliases.txt") for x in open(os.path.join(HERE, "..", "..", "src", f))]:
        p = l.split()
        if len(p) == 2:
            try: va = int(p[1], 16)
            except ValueError: continue
            if va not in SYMS or SYMS[va].startswith(("FUN_", "LAB_", "gap_")) or p[0].startswith("rwp_") and not SYMS[va].startswith("rwp_"): SYMS[va] = p[0]
    return SYMS

def rodata_float(va, kind):
    """value of a constant at va if it lies in a read-only section, else None"""
    d = cmpfn.exe_bytes(va, 8)
    base = 0x400000
    if not (0x5dd000 <= va < 0x5fb000): return None      # .rdata of MFL.exe
    return struct.unpack("<f" if kind == "f" else "<d", d[:4] if kind == "f" else d)[0]

def flit(v, kind):
    if v != v or abs(v) == float("inf"): raise Unsupported("nan const")
    if kind == "f":
        for p in range(1, 10):
            s = "%.*g" % (p, v)
            try:
                if struct.unpack("<f", struct.pack("<f", float(s)))[0] == v: break
            except OverflowError: pass
    else: s = repr(v)
    if "e" not in s and "." not in s and "inf" not in s: s += ".0"
    if s.startswith("0.") is False and s.startswith("-0.") is False and "." not in s and "e" in s: pass
    return s + ("f" if kind == "f" else "")

class Var:
    n = 0
    def __init__(s, kind, name=None):
        s.kind = kind; s.uses = set(); s.name = name; s.decl = None; Var.n += 1
    def isf(s): return s.kind == "p" and s.uses and all(u == "fb" for u in s.uses)

class N:      # x87 value
    def __init__(s, kind, *a, seq=0):
        s.kind = kind; s.a = a; s.dbl = False; s.refs = 0; s.temp = False; s.seq = seq; s.var = None
    def has_load(s):
        if s.kind == "cp": return False
        if s.kind in ("mem", "callf", "i"): return True
        return any(x.has_load() for x in s.a if isinstance(x, N))

class State:
    def __init__(s):
        s.lastcall = None; s.regs = {}; s.fp = []; s.delta = 0; s.flags = None; s.pend = []; s.slots = {}; s.dead = False; s.fcmp = None; s.axf = None
    def fork(s):
        n = State(); n.regs = dict(s.regs); n.fp = list(s.fp); n.delta = s.delta; n.flags = s.flags; n.pend = list(s.pend); n.dead = s.dead; n.fcmp = s.fcmp; n.axf = s.axf; n.lastcall = s.lastcall
        return n

class Lifter:
    def __init__(s, va, name=None, size=None):
        s.va = va; s.name = name or symbols().get(va, "FUN_%08x" % va)
        s.ins = dis(va, size); s.idx = {a: i for i, (a, *_r) in enumerate(s.ins)}
        s.args = {}; s.slots = {}; s.stmts = []; s.seq = 0; s.ptrs = []; s.tmps = []; s.regargs = {}
        s.blocks = set(); s.retf = False; s.reti = False; s.callee = {}; s.stdcall = 0; s.alias = {}; s.floatcalls = set(); s.entry_push = set(); s.midsub = False
        s.st = State(); s.pstates = []

    @property
    def lastcall(s): return s.st.lastcall      # T-X90: per path (a float result pending on one arm of a branch is still pending on the other)
    @lastcall.setter
    def lastcall(s, v): s.st.lastcall = v

    # ---------------------------------------------------------------- helpers
    def emit(s, st): s.out.append(st)
    def arg(s, e):
        if e not in s.args: s.args[e] = Var("p", "a%d" % ((e - 4) // 4 + 1))
        return s.args[e]
    def slot(s, e, kind):
        if e >= 4 and e in s.alias:
            v = s.alias[e]
            if kind in "fd" and v.kind != "f": v.kind = "f"; v.name = "f_a%d" % e
            return v
        if e >= 4:
            v = s.arg(e)
            if kind in "fd": v.fl = True
            return v
        if e not in s.slots: s.slots[e] = Var(kind, "%s%d" % ("f" if kind in "fd" else "i", -e)); s.slots[e].kind = kind
        elif kind in "fd" and s.slots[e].kind != "f": s.slots[e].kind = "f"; s.slots[e].name = "f%d" % -e
        return s.slots[e]
    def write_slot(s, mo, kind):
        """a store / address-taking into the home of an argument: from now on that slot is a local (cl reuses dead parameter homes)"""
        if mo[0] == "m" and mo[2] == "esp" and mo[3] is None:
            e = s.st.delta + mo[4]
            if e >= 4 and e not in s.alias: s.alias[e] = Var(kind, "%s_a%d" % ("f" if kind in "fd" else "i", e))
    def newp(s, name="p"):
        v = Var("p"); v.name = "%s%d" % (name, len(s.ptrs) + 1); s.ptrs.append(v); return v
    def reg(s, r):
        r = SUBREG.get(r, r)
        v = s.st.regs.get(r, "none")
        if v == "none":
            if r in ("ecx", "edx", "eax"): s.regargs[r] = Var("p", {"ecx": "this", "edx": "edx_in", "eax": "eax_in"}[r]); v = ("var", s.regargs[r]); s.st.regs[r] = v
            elif r == "esp": raise Unsupported("esp read as a value (by-value struct argument / outgoing area)")
            else: v = ("var", Var("p", "in_" + r)); s.st.regs[r] = v     # callee-saved register read before written: left as a variable (will fail at lint)
        if v == "unk": raise Unsupported("register %s unknown after join" % r)
        if v[0] == "phi":
            nv = Var("p"); nv.name = "r%d" % (len(s.ptrs) + 1); s.ptrs.append(nv)
            v[3].append(("decl", nv, v[1])); v[4].append(("decl", nv, v[2])); v = ("var", nv); s.st.regs[r] = v
            for stt in s.pstates: 
                if stt.regs.get(r) is not None and isinstance(stt.regs.get(r), tuple) and stt.regs[r][0] == "phi": stt.regs[r] = v
        return v
    def tmp(s, kind="f"):
        v = Var(kind); v.name = "%s%d" % ("f" if kind == "f" else "i", len(s.tmps) + 1); s.tmps.append(v); return v
    def mk(s, kind, *a):
        s.seq += 1; n = N(kind, *a, seq=s.seq)
        if kind not in ("k",): s.emit(("maybe", n))
        return n

    # ---------------------------------------------------------------- operands
    def parse_mem(s, op):
        m = re.match(r"(?:(DWORD|QWORD|WORD|BYTE|TBYTE|XMMWORD) PTR )?(?:[cdes]s:)?(.*)$", op)
        size = {"DWORD": 4, "QWORD": 8, "WORD": 2, "BYTE": 1, "TBYTE": 10}.get(m.group(1))
        body = m.group(2)
        if body.startswith("["):
            terms = re.findall(r"([+-]?)([a-z]{3}(?:\*\d)?|0x[0-9a-f]+)", body[1:-1])
            base = None; idx = None; disp = 0
            for sign, t in terms:
                if t.startswith("0x"): disp += int(t, 16) * (-1 if sign == "-" else 1)
                elif "*" in t:
                    r, sc = t.split("*"); idx = (r, int(sc))
                elif base is None: base = t
                else: idx = (t, 1)
            return ("m", size, base, idx, disp)
        return ("abs", size, int(body, 16))
    def is_mem(s, op): return "PTR" in op or re.match(r"[cdes]s:", op)

    def addr_of(s, mo):
        """-> ('s', slot var) | ('g', va) | ('a', ivbase, disp)  and registers uses"""
        if mo[0] == "abs": return ("g", mo[2])
        _, size, base, idx, disp = mo
        if base == "esp" and idx is None: return ("s", s.st.delta + disp)
        if base is None and idx is None: return ("g", disp)
        iv = None
        if base: iv = s.reg(base)
        if idx:
            t = s.reg(idx[0]); t = t if idx[1] == 1 else ("mul", t, idx[1])
            iv = t if iv is None else ("add", iv, t)
        return ("a", iv, disp)

    def base_var(s, iv, reg=None):
        """materialise a computed base so it becomes a pointer local (only when the base is a bare register value)"""
        if iv[0] == "var": return iv
        if READDR and iv[0] in ("add", "shl", "mul", "sub"): return iv
        if iv[0] in ("add", "shl", "mul", "sub"):
            v = s.newp(); s.emit(("decl", v, iv))
            if reg:
                for r, x in s.st.regs.items():
                    if x is iv: s.st.regs[r] = ("var", v)
            return ("var", v)
        return iv

    def mem_leaf(s, mo, kind_use):
        """-> address tuple usable in nodes: ('slot', var) | ('glob', va) | ('mem', ivbase, disp)"""
        a = s.addr_of(mo)
        if a[0] == "s":
            e = a[1]
            return ("slot", s.slot(e, "f" if kind_use in "fd" else "i"))
        if a[0] == "g": return ("glob", a[1])
        iv = a[1]; disp = a[2]
        if mo[3] is None and mo[2]:
            iv = s.base_var(iv, mo[2])
            s.st.regs[SUBREG.get(mo[2], mo[2])] = iv if iv[0] == "var" else s.st.regs.get(mo[2])
        if iv[0] == "var":
            use = "fb" if (kind_use in ("f", "i4f") and disp % 4 == 0 and mo[3] is None) else "o"
            if kind_use == "f" and mo[3] is None and disp % 4 == 0: use = "fb"
            else: use = "o"
            iv[1].uses.add(use)
        else: s.mark_other(iv)
        return ("mem", iv, disp)

    def mark_other(s, iv):
        if iv[0] == "var": iv[1].uses.add("o")
        elif iv[0] in ("add", "sub", "mul", "shl"):
            for x in iv[1:]:
                if isinstance(x, tuple): s.mark_other(x)

    # ---------------------------------------------------------------- x87
    def flush(s, keep=()):
        """stack values with pending loads must be evaluated now (a store / call / branch follows)"""
        for n in s.st.fp:
            if n not in keep and n.has_load() and n.kind != "k": n.temp = True
    def push(s, n): s.st.fp.append(n)
    def pop(s):
        if not s.st.fp:
            if s.lastcall is not None:
                st = s.lastcall; s.lastcall = None; st[1][1].kind = "f"; s.floatcalls.add(st[1][2]) if isinstance(st[1][2], str) else None; n = s.mk("callf", st[1]); n.temp = False; return n
            raise Unsupported("x87 stack underflow")
        return s.st.fp.pop()
    def sti(s, i):
        while i >= len(s.st.fp) and s.lastcall is not None and not getattr(s, "_callf_done", False):
            st = s.lastcall; s.lastcall = None; st[1][1].kind = "f"; s.floatcalls.add(st[1][2]) if isinstance(st[1][2], str) else None
            n = s.mk("callf", st[1]); s.st.fp.insert(0, n); s.retf_call = True
        if i >= len(s.st.fp): raise Unsupported("st(%d) beyond stack" % i)
        return s.st.fp[-1 - i]
    def setsti(s, i, n): s.st.fp[-1 - i] = n

    def fpstep(s, a, m, o, raw):
        st = s.st
        ops = [x.strip() for x in o.split(",")] if o else []
        if m in ("fnstsw",): st.axf = st.fcmp; return
        if m in ("fwait", "wait", "fnclex"): return
        if m == "fld":
            if ops[0].startswith("st"):
                i = int(re.search(r"\((\d)\)", ops[0]).group(1)); n = s.sti(i); n.refs += 1; n.temp = True; st.fp.append(s.mk("cp", n))     # cp = a var-first copy (RWP-X11)
            else:
                mo = s.parse_mem(ops[0]); kind = {4: "f", 8: "d"}.get(mo[1])
                if not kind: raise Unsupported("fld %s" % ops[0])
                ad = s.mem_leaf(mo, "f"); s.push(s.leaf(ad, kind))
            return
        if m == "fld1": s.push(s.mk("k", 1.0, "f")); return
        if m == "fldz": s.push(s.mk("k", 0.0, "f")); return
        if m == "fild":
            mo = s.parse_mem(ops[0]); ad = s.mem_leaf(mo, "i"); s.push(s.mk("i", ad, mo[1])); return
        if m in ("fst", "fstp"):
            pop = m == "fstp"
            if ops[0].startswith("st"):
                i = int(re.search(r"\((\d)\)", ops[0]).group(1))
                if i == 0:
                    if pop: s.pop()
                    return
                n = s.sti(0)
                if pop: s.pop(); i -= 1
                s.setsti(i, n)
                return
            mo = s.parse_mem(ops[0]); kind = {4: "f", 8: "d"}.get(mo[1])
            if not kind: raise Unsupported("fstp %s" % ops[0])
            popped = not st.fp      # T-X90: a pending call result popped for the operand is the fstp's pop
            n = s.sti(0) if st.fp else s.pop()
            if popped: pop = False
            if s.midsub and mo[0] == "m" and mo[2] == "esp" and mo[3] is None:
                if pop: s.pop()
                st.pend.append(("farg", n, kind)); return
            s.flush(keep=(n,))
            s.write_slot(mo, "f"); ad = s.mem_leaf(mo, "f")
            if pop: s.pop()
            s.emit(("store", ad, n, kind)); s.after_store(n)
            return
        if m == "fxch":
            i = int(re.search(r"\((\d)\)", ops[0]).group(1)) if ops else 1
            a1, b1 = s.sti(0), s.sti(i); s.setsti(0, b1); s.setsti(i, a1); return
        if m in ("fchs", "fabs", "fsqrt", "fsin", "fcos"):
            n = s.sti(0); s.setsti(0, s.mk("un", {"fchs": "neg", "fabs": "abs", "fsqrt": "sqrt", "fsin": "sin", "fcos": "cos"}[m], n)); return
        if m == "fpatan":
            x = s.pop(); y = s.pop(); s.push(s.mk("fn2", "atan2", y, x)); return
        if m in ("fcom", "fcomp", "fucom", "fucomp", "fcompp", "fucompp", "ftst"):
            A = s.sti(0)
            if m == "ftst": B = s.mk("k", 0.0, "f")
            elif m.endswith("pp"): B = s.sti(1)
            elif not ops: B = s.sti(1)
            elif ops[0].startswith("st"): B = s.sti(int(re.search(r"\((\d)\)", ops[0]).group(1)))
            else:
                mo = s.parse_mem(ops[0]); kind = {4: "f", 8: "d"}[mo[1]]; B = s.leaf(s.mem_leaf(mo, "f"), kind)
            st.fcmp = ("fcmp", A, B)
            if m.endswith("pp"): s.pop(); s.pop()
            elif m.endswith("p"): s.pop()
            return
        # arithmetic
        base = re.match(r"f(i?)(add|sub|subr|mul|div|divr)(p?)$", m)
        if base:
            isint, op, p = base.groups()
            sym = {"add": "+", "mul": "*", "sub": "-", "div": "/", "subr": "-", "divr": "/"}[op]; rev = op in ("subr", "divr")
            if ops and "PTR" in ops[0]:
                mo = s.parse_mem(ops[0]); sz = mo[1]
                ad = s.mem_leaf(mo, "i" if isint else "f")
                B = s.mk("i", ad, sz) if isint else s.leaf(ad, {4: "f", 8: "d"}[sz])
                A = s.sti(0)
                s.setsti(0, s.mk("bin", sym, B, A) if rev else s.mk("bin", sym, A, B)); return
            # register forms: decode from the opcode bytes
            b0, b1 = raw[0], raw[1]; i = b1 & 7; r = (b1 >> 3) & 7
            if b0 == 0xD8:
                A = s.sti(0); B = s.sti(i); rv = r in (5, 7)
                if A.kind == "mem" and i > 0 and B.kind != "cp": B.dbl = True      # RWP-X11: `fld [m]; fop st,st(i)` = the x87 resident operand is a double
                s.setsti(0, s.mk("bin", {0: "+", 1: "*", 4: "-", 5: "-", 6: "/", 7: "/"}[r], B, A) if rv else s.mk("bin", {0: "+", 1: "*", 4: "-", 5: "-", 6: "/", 7: "/"}[r], A, B)); return
            if b0 in (0xDC, 0xDE):
                symt = {0: "+", 1: "*", 4: "-", 5: "-", 6: "/", 7: "/"}[r]; rv = r in (4, 6)      # DC E0 = sti = st0 - sti ; DC E8 = sti - st0
                A = s.sti(i); B = s.sti(0)
                nn = s.mk("bin", symt, B, A) if rv else s.mk("bin", symt, A, B)
                if b0 == 0xDE: s.pop(); s.setsti(i - 1, nn)
                else: s.setsti(i, nn)
                return
        raise Unsupported("x87 %s %s" % (m, o))

    def leaf(s, ad, kind):
        if ad[0] == "glob":
            v = rodata_float(ad[1], kind)
            if v is not None: return s.mk("k", v, kind)
        return s.mk("mem", ad, kind)
    def after_store(s, n): pass

    # ---------------------------------------------------------------- integer / control
    def iv_of(s, op, size=4):
        op = op.strip()
        if re.fullmatch(r"-?0x[0-9a-f]+|-?\d+", op): return ("imm", int(op, 0))
        if re.fullmatch(r"e[a-ds][xip]|e[sd]i|ebp", op): return s.reg(op)
        if re.fullmatch(r"[a-d][xlh]", op): return s.reg(op)
        raise Unsupported("operand " + op)

    def load_int(s, mo, size, signed=False, dst=None):
        ad = s.mem_leaf(mo, "i4" if size == 4 else "i%d" % size)
        v = s.newp("v") if False else Var("p"); v.name = "v%d" % (len(s.ptrs) + 1); s.ptrs.append(v)
        s.emit(("decl", v, ("ld", ad, size, signed)))
        return ("var", v)

    def block_ret(s, i):
        """index i at a ret sequence (pops, add esp, ret) -> True"""
        j = i
        while j < len(s.ins) and s.ins[j][1] in ("pop", "add", "mov", "leave"):
            if s.ins[j][1] == "add" and not s.ins[j][2].startswith("esp"): return False
            if s.ins[j][1] == "mov" and not s.ins[j][2].startswith("esp"): return False
            j += 1
        return j < len(s.ins) and s.ins[j][1] == "ret"

    def ret_block_value(s, ti):
        """`pop*; [mov eax,imm | xor eax,eax]; add esp; ret` at index ti -> ('ret', value or None); None when it is more than that"""
        val = "keep"; j = ti
        while j < len(s.ins):
            a, m, o, _r = s.ins[j]
            if m == "pop" or (m == "add" and o.startswith("esp")): pass
            elif m == "mov" and re.fullmatch(r"eax,(0x[0-9a-f]+|\d+)", o): val = ("imm", int(o.split(",")[1], 0))
            elif m == "xor" and o == "eax,eax": val = ("imm", 0)
            elif m == "ret": return ("ret", val)
            else: return None
            j += 1
        return None

    def do_ret(s, ret_ins):
        st = s.st
        if ret_ins[2]: s.stdcall = int(ret_ins[2], 0)
        if st.fp:
            n = st.fp[-1]; s.retf = True; s.emit(("return", "f", n)); st.fp = []
        else:
            x = st.regs.get("eax")
            if isinstance(x, tuple) and x[0] == "phi" and x[1][0] == "imm" and x[2][0] == "imm" and len(x) > 5 and x[5] is not None:      # RWP-X10: `return c ? A : B;`
                c, A, B = x[5], x[1], x[2]
                if c[0] == "fcmp" and c[1] in ("<=", "<"): c, A, B = ("fcmp", s.NEG[c[1]], c[2], c[3]), B, A      # the positive spelling `a > b ? ... : ...`
                x = ("sel", c, A, B)
            elif isinstance(x, tuple) and x[0] == "phi" and x[1][0] == "imm" and x[2][0] == "imm": x = s.reg("eax")
            if isinstance(x, tuple) and x[0] != "phi" and st.regs.get("eax_defined"): s.reti = True; s.emit(("return", "i", x))
            else: s.emit(("return", None, None))
        st.dead = True

    JREL = {"jl": "<", "jle": "<=", "jg": ">", "jge": ">=", "je": "==", "jne": "!=", "jb": "<u", "jbe": "<=u", "ja": ">u", "jae": ">=u", "js": "<0", "jns": ">=0"}
    NEG = {"<": ">=", "<=": ">", ">": "<=", ">=": "<", "==": "!=", "!=": "==", "<u": ">=u", "<=u": ">u", ">u": "<=u", ">=u": "<u", "<0": ">=0", ">=0": "<0"}
    def jcc_fall(s, m):
        """condition (tuple) under which control FALLS THROUGH a jcc"""
        st = s.st; f = st.flags
        if m in ("jp", "jnp", "jne", "je", "jb", "jbe", "ja", "jae") and st.axf is not None and isinstance(f, tuple) and f[0] == "ah":
            A, B = st.axf[1], st.axf[2]; mask = f[1]
            tab = {(0x41, "jne"): ">", (0x41, "je"): "<=", (0x45, "jne"): ">", (0x45, "je"): "<=", (0x01, "jne"): ">=", (0x01, "je"): "<", (0x40, "jne"): "!=", (0x40, "je"): "==",
                   (0x05, "jp"): "<", (0x05, "jnp"): ">=", (0x41, "jp"): "<=", (0x41, "jnp"): ">", (0x45, "jp"): "<=", (0x45, "jnp"): ">", (0x44, "jp"): "==", (0x44, "jnp"): "!="}
            if (mask, m) not in tab: raise Unsupported("fcmp mask %x %s" % (mask, m))
            return ("fcmp", tab[(mask, m)], A, B)
        if f and f[0] == "sahf" and st.axf is not None:
            A, B = st.axf[1], st.axf[2]
            tab = {"jbe": ">", "ja": "<=", "jb": ">=", "jae": "<", "je": "!=", "jne": "=="}
            if m not in tab: raise Unsupported("sahf " + m)
            return ("fcmp", tab[m], A, B)
        if f and f[0] == "cmp":
            if m not in s.JREL: raise Unsupported("jcc " + m)
            return ("icmp", s.NEG[s.JREL[m]], f[1], f[2])
        if f and f[0] == "test":
            a, b = f[1], f[2]
            if a == b:
                rel = s.JREL.get(m)
                if m in ("je", "jne"): return ("icmp", "!=" if m == "je" else "==", a, ("imm", 0))
                if m in ("jl", "js"): return ("icmp", ">=", a, ("imm", 0))
                if m in ("jge", "jns"): return ("icmp", "<", a, ("imm", 0))
                if m == "jle": return ("icmp", ">", a, ("imm", 0))
                if m == "jg": return ("icmp", "<=", a, ("imm", 0))
                if m == "jbe": return ("icmp", "!=", a, ("imm", 0))
                if m == "ja": return ("icmp", "==", a, ("imm", 0))
            elif m in ("je", "jne"): return ("tst", m == "je", a, b)
        raise Unsupported("jcc %s without known flags" % m)

    def merge(s, a, b, cond=None, thenb=None, elseb=None, pc=None):
        """join two live states into self.st; registers that differ become variables assigned at the end of each arm"""
        if a.dead: s.st = b; return
        if b.dead: s.st = a; return
        if len(a.fp) != len(b.fp): raise Unsupported("x87 stack depth differs at join")
        if a.delta != b.delta: raise Unsupported("esp differs at join")
        m = a.fork()
        for k, (x, y) in enumerate(zip(a.fp, b.fp)):
            if x is not y:
                if cond is None and thenb is None: raise Unsupported("x87 stack differs at join")
                if cond is None:
                    v = s.tmp("f"); thenb.append(("fassign", v, x)); elseb.append(("fassign", v, y)); jn = N("jv", seq=0); jn.temp = True; jn.var = v; m.fp[k] = jn; continue
                m.fp[k] = s.mk("sel", cond, x, y)
        for r in set(a.regs) | set(b.regs):
            x, y = a.regs.get(r), b.regs.get(r)
            if x is y or x == y: continue
            if r in REGS and thenb is not None and isinstance(x, tuple) and isinstance(y, tuple): m.regs[r] = ("phi", x, y, thenb, elseb, pc)
            else: m.regs[r] = "unk"
        m.flags = None; s.st = m

    # ---------------------------------------------------------------- run
    def run(s, i, end, out):
        """execute instructions [i, end) appending statements to out; returns index reached"""
        saved = s.out; s.out = out
        try: return s._run(i, end)
        finally: s.out = saved

    def _run(s, i, end):
        st = s.st
        while i < end:
            if s.st.dead:
                while i < end and s.ins[i][0] not in s.pending: i += 1
                if i >= end: return end
            a, m, o, raw = s.ins[i]; s.cur = a
            if a in s.pending:
                cur = s.st
                for ps in s.pending.pop(a):
                    if cur.dead: cur = ps; s.st = ps
                    else: s.st = cur; s.merge(cur, ps); cur = s.st
                s.st = cur; s.emit(("label", a))
            st = s.st
            if a in s.loops and a not in s.done_loops:
                s.done_loops.add(a); i = s.do_loop(i, s.loops[a]); continue
            ops = [x.strip() for x in re.split(r",(?![^\[]*\])", o)] if o else []
            nxt = i + 1
            if m.startswith("f") or m in ("wait",):
                if m == "fnstsw": st.axf = st.fcmp; i = nxt; continue
                if m in ("fnstcw", "fldcw"): raise Unsupported("control word")
                s.fpstep(a, m, o, raw); st = s.st; i = nxt; continue
            if m == "push":
                if ops[0] not in st.regs and not st.pend and ops[0] not in s.entry_push and not s.args_started and (i < 8 or ops[0] in ("ebx", "esi", "edi", "ebp")): s.entry_push.add(ops[0]); st.delta -= 4; i = nxt; continue
                st.delta -= 4
                if "PTR" in ops[0]:
                    mo = s.parse_mem(ops[0]); st.pend.append(("i", s.load_int(mo, 4)))
                else: st.pend.append(("i", s.iv_of(ops[0])))
                s.args_started = True; i = nxt; continue
            if m == "pop":
                st.delta += 4
                if ops[0] in s.entry_push: i = nxt; continue
                st.pend = []; st.regs[ops[0]] = "unk"; i = nxt; continue      # stack clean-up of an argument / scratch slot
            if m == "sub" and ops[0] == "esp":
                n = int(ops[1], 0)
                if i < 6 and not s.args_started: s.frame = n; s.frame_hi = st.delta; st.delta -= n
                else: s.midsub = True; st.delta -= n
                i = nxt; continue
            if m == "add" and ops[0] == "esp":
                n = int(ops[1], 0); st.delta += n
                if s.midsub and st.pend and st.pend[-1][0] == "farg": pass
                st.pend = []; s.midsub = False; i = nxt; continue
            if m == "mov":
                d, sr = ops
                if "PTR" in d or d.startswith(("ds:", "[")):
                    mo = s.parse_mem(d); v = s.iv_of(sr) if "PTR" not in sr else None
                    if v is None: raise Unsupported("mem-mem mov")
                    s.write_slot(mo, "i"); ad = s.mem_leaf(mo, "i4"); s.flush(); s.emit(("istore", ad, v, mo[1])); i = nxt; continue
                if "PTR" in sr or sr.startswith(("ds:", "[")):
                    mo = s.parse_mem(sr)
                    if d in SUBREG and mo[1] in (1, 2) and d not in ("ah",):
                        v = Var("p"); v.name = "v%d" % (len(s.ptrs) + 1); s.ptrs.append(v); ad = s.mem_leaf(mo, "i%d" % mo[1])
                        s.emit(("decl", v, ("ld", ad, mo[1], False))); st.regs[SUBREG[d]] = ("var", v); i = nxt; continue
                    if mo[1] not in (4, None) or d not in REGS: raise Unsupported("mov " + o)
                    ad0 = s.addr_of(mo)
                    if ad0[0] == "s" and ad0[1] >= 4:      # argument slot: the register is the argument variable
                        st.regs[d] = ("var", s.arg(ad0[1])); i = nxt; continue
                    st.regs[d] = s.load_int(mo, 4); st.regs["eax_defined"] = d == "eax"; i = nxt; continue
                if d in REGS and (sr in REGS or re.fullmatch(r"-?0x[0-9a-f]+|-?\d+", sr)):
                    st.regs[d] = s.iv_of(sr); st.regs["eax_defined"] = (d == "eax") or st.regs.get("eax_defined") and d != "eax"; i = nxt; continue
                raise Unsupported("mov " + o)
            if m == "xor" and ops[0] == ops[1] and ops[0] in REGS:
                st.regs[ops[0]] = ("imm", 0); st.regs["eax_defined"] = ops[0] == "eax" or st.regs.get("eax_defined"); i = nxt; continue
            if m == "lea":
                mo = s.parse_mem(ops[1]); a_ = s.addr_of(mo)
                if a_[0] == "s":
                    s.write_slot(mo, "f"); sv = s.slot(a_[1], "f"); sv.kind = "f"; sv.addr_taken = True; st.regs[ops[0]] = ("addrof", sv); st.regs["eax_defined"] = ops[0] == "eax" or st.regs.get("eax_defined"); i = nxt; continue
                if a_[0] != "a": raise Unsupported("lea slot")
                iv = a_[1]
                if a_[2]: iv = ("add", iv, ("imm", a_[2])) if iv is not None else ("imm", a_[2])
                s.mark_other(iv); st.regs[ops[0]] = iv; st.regs["eax_defined"] = ops[0] == "eax" or st.regs.get("eax_defined"); i = nxt; continue
            if m == "neg" and ops[0] in REGS and i + 2 < end and s.ins[i + 1][1] == "sbb" and s.ins[i + 2][1] == "neg":      # neg r; sbb r,r; neg r  =  (r != 0)  (RWP-X9)
                r = ops[0]; cur = s.reg(r); s.mark_other(cur); v = ("cond", ("icmp", "!=", cur, ("imm", 0)))
                st.regs[r] = v; st.flags = ("test", v, v); st.regs["eax_defined"] = r == "eax" or st.regs.get("eax_defined"); i += 3; continue
            if m == "sbb" and len(ops) == 2 and ops[0] == ops[1] and ops[0] in REGS:      # sbb r,r after cmp a,b = -(a <u b)
                f = st.flags
                if not (f and f[0] == "cmp"): raise Unsupported("sbb without cmp")
                st.regs[ops[0]] = ("un", "neg", ("cond", ("icmp", "<u", f[1], f[2]))); st.flags = None; st.regs["eax_defined"] = ops[0] == "eax" or st.regs.get("eax_defined"); i = nxt; continue
            if m.startswith("set") and ops[0] in SUBREG and m[3:] in ("e", "ne", "l", "le", "g", "ge", "b", "be", "a", "ae", "s", "ns"):
                j_ = "j" + m[3:]; fall = s.jcc_fall(j_); r = SUBREG[ops[0]]
                st.regs[r] = ("cond", ("not", fall) if fall[0] != "not" else fall[1]); st.regs["eax_defined"] = r == "eax" or st.regs.get("eax_defined"); i = nxt; continue
            if m in ("shl", "sar", "shr") and ops[0] in REGS and ops[1] == "cl":      # shift by cl
                r = ops[0]; cur = s.reg(r); cnt = s.reg("ecx"); s.mark_other(cur); s.mark_other(cnt)
                st.regs[r] = ("shv", cur, cnt, m); st.flags = None; st.regs["eax_defined"] = r == "eax" or st.regs.get("eax_defined"); i = nxt; continue
            if m == "imul" and len(ops) == 2 and ops[0] in REGS and ops[1] in REGS:
                r = ops[0]; cur = s.reg(r); o2 = s.reg(ops[1]); s.mark_other(cur); s.mark_other(o2)
                st.regs[r] = ("bin", "mul", cur, o2); st.flags = None; st.regs["eax_defined"] = r == "eax" or st.regs.get("eax_defined"); i = nxt; continue
            if m in ("movs", "rep") and (m == "movs" or o.startswith("movs")) and "DWORD" in o:      # struct copy (RWP-X9)
                cnt = 1
                if m == "rep":
                    c_ = st.regs.get("ecx")
                    if not (isinstance(c_, tuple) and c_[0] == "imm"): raise Unsupported("rep movs count")
                    cnt = c_[1]
                src, dst = s.reg("esi"), s.reg("edi")
                if any(x[0] == "var" and x[1].name.startswith("in_") for x in (src, dst)): raise Unsupported("struct copy through an unknown register (by-value argument)")
                s.mark_other(src); s.mark_other(dst)
                s.flush(); s.emit(("copy", dst, src, cnt)); s.blocks.add(cnt)
                st.regs["esi"] = ("add", src, ("imm", 4 * cnt)); st.regs["edi"] = ("add", dst, ("imm", 4 * cnt))
                if m == "rep": st.regs["ecx"] = ("imm", 0)
                i = nxt; continue
            if m in ("add", "sub", "shl", "imul", "inc", "dec", "and", "or") and ops[0] in REGS:
                r = ops[0]
                if m == "and" and r in ("eax", "ax") and st.axf is not None and re.fullmatch(r"0x[0-9a-f]+", ops[1]) and int(ops[1], 16) in (0x100, 0x400, 0x4000, 0x4100, 0x4500, 0x500, 0x4400):
                    st.flags = ("ah", int(ops[1], 16) >> 8); st.regs["eax"] = ("imm", 0); i = nxt; continue
                cur = s.reg(r)
                if m == "inc": new = ("add", cur, ("imm", 1))
                elif m == "dec": new = ("add", cur, ("imm", -1))
                elif m == "shl": new = ("shl", cur, int(ops[1], 0))
                elif m == "imul":
                    if len(ops) == 3: new = ("mul", s.iv_of(ops[1]), int(ops[2], 0))
                    elif re.fullmatch(r"-?0x[0-9a-f]+|-?\d+", ops[1]): new = ("mul", cur, int(ops[1], 0))
                    else: raise Unsupported("imul")
                else:
                    if m == "and" and r in ("eax", "ax") and st.axf is not None and re.fullmatch(r"0x[0-9a-f]+", ops[1]) and int(ops[1], 16) in (0x100, 0x400, 0x4000, 0x4100, 0x4500, 0x500, 0x4400):
                        st.flags = ("ah", int(ops[1], 16) >> 8); st.regs["eax"] = ("imm", 0); i = nxt; continue
                    if "PTR" in ops[1]: new = (m, cur, s.load_int(s.parse_mem(ops[1]), 4))
                    else: new = (m if m != "add" else "add", cur, s.iv_of(ops[1]))
                s.mark_other(cur);
                if new[0] in ("and", "or"): new = ("bin", m, cur, new[2])
                st.regs[r] = new; st.regs["eax_defined"] = r == "eax" or st.regs.get("eax_defined"); st.flags = ("test", new, new) if m in ("and", "or") else None; i = nxt; continue
            if m == "cmp" or m == "test":
                if "PTR" in ops[0]: a1 = s.load_int(s.parse_mem(ops[0]), 4)
                elif ops[0] == "ah": st.flags = ("ah", int(ops[1], 0)) if m == "test" else None; i = nxt; continue
                else: a1 = s.iv_of(ops[0])
                b1 = s.load_int(s.parse_mem(ops[1]), 4) if "PTR" in ops[1] else s.iv_of(ops[1])
                s.mark_other(a1); s.mark_other(b1); st.flags = (m, a1, b1); i = nxt; continue
            if m in ("movzx", "movsx") and ops[0] in REGS and "PTR" in ops[1]:
                mo = s.parse_mem(ops[1]); v = Var("p"); v.name = "v%d" % (len(s.ptrs) + 1); s.ptrs.append(v); ad = s.mem_leaf(mo, "i%d" % mo[1])
                s.emit(("decl", v, ("ld", ad, mo[1], m == "movsx"))); st.regs[ops[0]] = ("var", v); st.regs["eax_defined"] = ops[0] == "eax" or st.regs.get("eax_defined"); i = nxt; continue
            if m in ("sar", "shr") and ops[0] in REGS and re.fullmatch(r"0x[0-9a-f]+|\d+", ops[1]):
                st.regs[ops[0]] = ("shr", s.reg(ops[0]), int(ops[1], 0), m == "sar"); st.flags = None; i = nxt; continue
            if m in ("neg", "not") and ops[0] in REGS:
                cur = s.reg(ops[0])
                st.regs[ops[0]] = cur[2] if (m == "neg" and cur[0] == "un" and cur[1] == "neg") else ("un", m, cur); st.flags = None; i = nxt; continue
            if m == "sahf": st.flags = ("sahf",); i = nxt; continue
            if m == "ret":
                s.do_ret(s.ins[i]); i = nxt; continue
            if m == "call":
                i = s.docall(i, ops); continue
            if m == "jmp":
                t = int(ops[0], 16)
                if t in s.idx and s.block_ret(s.idx[t]): s.do_ret(s.ins[next(j for j in range(s.idx[t], len(s.ins)) if s.ins[j][1] == "ret")]); i = nxt; continue
                if t > a and t in s.idx:
                    s.emit(("goto", t)); s.pending.setdefault(t, []).append(s.st.fork()); st.dead = True; i = nxt; continue
                raise Unsupported("jmp %s" % ops[0])
            if m in ("jp", "jnp", "jl", "jle", "jg", "jge", "je", "jne", "jb", "jbe", "ja", "jae", "js", "jns"):
                t = int(ops[0], 16)
                if t <= a: raise Unsupported("loop (backward jump)")
                cond = s.jcc_fall(m)
                s.flush()
                if t not in s.idx: raise Unsupported("jump out of function")
                ti = s.idx[t]
                rb = s.ret_block_value(ti) if not st.fp and ti > end else None
                if rb is not None:
                    val = rb[1] if rb[1] != "keep" else (st.regs.get("eax") if st.regs.get("eax_defined") and isinstance(st.regs.get("eax"), tuple) and st.regs["eax"][0] == "imm" else None)
                    if val is not None: s.reti = True
                    s.emit(("if", ("not", cond), [("return", "i" if val is not None else None, val)], [])); i = nxt; continue
                # early-return arm: the target is a ret block -> `if (!fall) return` is the same as if (jump cond) return
                if s.block_ret(ti) and (nxt >= ti or True) and ti >= end:
                    pass
                last = s.ins[ti - 1] if ti > nxt else None
                st0 = s.st; sA = st0.fork(); s.st = sA
                thenb = []
                if last and last[1] == "jmp" and int(last[2], 16) > t and int(last[2], 16) in s.idx and s.idx[int(last[2], 16)] <= end:
                    u = s.idx[int(last[2], 16)]
                    s.run(nxt, ti - 1, thenb); sT = s.st; s.st = st0.fork(); elseb = []; s.run(ti, u, elseb); sE = s.st
                    s.emit(("if", cond, thenb, elseb)); pos = len(s.out) - 1
                    s.merge(sT, sE, cond, thenb, elseb, cond)
                    i = u; continue
                if ti > end:
                    s.emit(("goto_if", ("not", cond), t)); s.pending.setdefault(t, []).append(s.st.fork()); i = nxt; continue
                s.run(nxt, ti, thenb); sT = s.st; s.st = st0.fork()
                elseb = []; s.emit(("if", cond, thenb, elseb)); pos = len(s.out) - 1
                s.merge(sT, s.st, None, thenb, elseb, cond)
                i = ti; continue
            raise Unsupported("insn %s %s" % (m, o))
        return i

    def do_loop(s, i, j):
        """instructions [i, j] form a loop whose last instruction (j) is a conditional jump back to i: `do { ... } while (cond);` Registers that hold a value at the head become loop variables."""
        st = s.st
        if st.fp: raise Unsupported("x87 stack not empty at loop head")
        s.flush()
        lv = {}
        for r in REGS:
            if r == "esp": continue
            x = st.regs.get(r)
            if isinstance(x, tuple) and x[0] != "imm" or isinstance(x, tuple) and x[0] == "imm":
                v = Var("p"); v.name = "l%d" % (len(s.ptrs) + 1); s.ptrs.append(v); s.emit(("decl", v, x)); lv[r] = v; st.regs[r] = ("var", v)
        st.flags = None
        body = []
        s.run(i, j, body)
        if s.st.dead: raise Unsupported("loop body ends dead")
        a, m, o, _r = s.ins[j]
        if m == "jmp": raise Unsupported("infinite loop")
        s.cur = a
        if s.st.fp: raise Unsupported("x87 stack not empty at loop end")
        fall = s.jcc_fall(m)
        for r, v in lv.items():
            x = s.st.regs.get(r)
            if x == ("var", v) or x is None: continue
            if x == "unk": raise Unsupported("loop register %s unknown" % r)
            body.append(("decl", v, x)); fall = s.subst_cond(fall, x, ("var", v)); s.st.regs[r] = ("var", v)
        s.emit(("dowhile", body, ("not", fall)))
        return j + 1

    def subst_cond(s, c, old, new):
        def f(x):
            if x == old: return new
            if isinstance(x, tuple) and x and isinstance(x[0], str) and x[0] in ("add", "sub", "mul", "shl", "bin"): return (x[0],) + tuple(f(y) if isinstance(y, tuple) else y for y in x[1:])
            return x
        return tuple(f(y) if isinstance(y, tuple) and y and y[0] in ("add", "sub", "mul", "shl", "var", "imm", "bin", "ld", "addrof") else y for y in c)

    def docall(s, i, ops):
        st = s.st; a = s.ins[i][0]
        if "PTR" in ops[0]:
            mo = s.parse_mem(ops[0]); ad = s.mem_leaf(mo, "i4"); nm = ("ind", ad)
        elif not re.fullmatch(r"0x[0-9a-f]+", ops[0]): raise Unsupported("indirect call")
        else: t = int(ops[0], 16); nm = symbols().get(t, "FUN_%08x" % t)
        if isinstance(nm, str) and nm in ("_ftol", "__ftol", "ftol"):
            n = s.pop(); v = Var("p"); v.name = "v%d" % (len(s.ptrs) + 1); s.ptrs.append(v); s.flush()
            s.emit(("decl", v, ("ftol", n))); st.regs["eax"] = ("var", v); st.regs["eax_defined"] = True; s.args_started = False; return i + 1
        if "ecx" in st.regs and st.regs.get("ecx") != "unk" and False: pass
        args = [p for p in reversed(st.pend)]
        st.pend = []; s.flush()
        v = Var("p"); v.name = "r%d" % (len(s.ptrs) + 1); s.ptrs.append(v)
        stmt = ["call", v, nm, args, False]; s.emit(stmt); s.lastcall = (a, stmt)
        if isinstance(nm, str): s.callee[nm] = t
        st.regs["eax"] = ("var", v); st.regs["eax_defined"] = True
        for r in ("ecx", "edx"): st.regs.pop(r, None)
        s.args_started = False
        return i + 1

    # ---------------------------------------------------------------- driver
    def lift(s):
        s.out = s.stmts; s.args_started = False; s.frame = 0; s.pending = {}; s.loops = {}; s.done_loops = set()
        for k, (a, m, o, _r) in enumerate(s.ins):
            if m.startswith("j") and re.fullmatch(r"0x[0-9a-f]+", o) and int(o, 16) <= a and int(o, 16) in s.idx: s.loops[int(o, 16)] = max(s.loops.get(int(o, 16), 0), k)
        try: s.run(0, len(s.ins), s.stmts)
        except Unsupported as e: raise Unsupported("%s @%x" % (e, getattr(s, "cur", 0)))
        except (ValueError, KeyError, IndexError) as e: raise Unsupported("operand form %r @%x" % (e, getattr(s, "cur", 0)))
        s.tail_merge(s.stmts)
        s.join_store(s.stmts)
        return s.render()

    def join_store(s, stmts):
        """RWP-X15: `if (c) { A; } else { B; }` followed by a store of the joined x87 value `c ? p : q`: the original source has the store in BOTH arms (cl shares the fstp itself),
        the ternary spelling changes the code (rwp_math_0056f500)"""
        for st in stmts:
            if st[0] == "if": s.join_store(st[2]); s.join_store(st[3])
        i = 0
        while i < len(stmts) - 1:
            st = stmts[i]
            if st[0] == "if" and st[3]:
                j = i + 1
                while j < len(stmts) and stmts[j][0] == "maybe": j += 1
                if j < len(stmts) and stmts[j][0] == "store" and stmts[j][2].kind == "sel" and stmts[j][2].a[0] == st[1]:
                    sx = stmts[j]; n = sx[2]
                    st[2].append(("store", sx[1], n.a[1], sx[3])); st[3].append(("store", sx[1], n.a[2], sx[3]))
                    del stmts[j]; continue
            i += 1

    def tail_merge(s, stmts):
        """RWP-X10: `if (c) { X = k1 * v; return r; } X = k2 * v; return r;` (arms identical but for literals) is ONE statement with a ternary factor `(c ? k1 : k2) * v`"""
        allow_mem = [False]
        def mn(c, a, b):
            if a is b: return a
            if a.kind != b.kind:
                return N("sel", c, a, b) if (a.kind == "k" or allow_mem[0]) and (b.kind == "k" or allow_mem[0]) and a.kind in ("k", "mem") and b.kind in ("k", "mem") else None
            if a.kind == "k": return N("sel", c, a, b) if a.a != b.a else a
            if a.kind == "mem" and a.a != b.a: return N("sel", c, a, b) if allow_mem[0] else None
            if a.kind == "bin" and a.a[0] == b.a[0]:
                x, y = mn(c, a.a[1], b.a[1]), mn(c, a.a[2], b.a[2])
                if x is None or y is None: return None
                if x is a.a[1] and y is a.a[2]: return a
                return N("bin", a.a[0], x, y)
            if a.kind == "mem" and a.a == b.a: return a
            return None
        for st in stmts:
            if st[0] == "if": s.tail_merge(st[2]); s.tail_merge(st[3])
        for i, st in enumerate(stmts):
            if st[0] != "if" or st[3] or not st[2] or st[2][-1][0] != "return": continue
            keep = lambda x: not (x[0] == "maybe" and not x[1].temp and x[1].refs == 0)
            T = [x for x in st[2] if keep(x)]; R = [x for x in stmts[i + 1:] if keep(x)]
            if len(T) != len(R) or len(T) < 2 or T[-1] != R[-1] or T[-1][1] == "f": continue
            out = []; allow_mem[0] = len(T) == 2      # a lone differing store is a ternary; with more statements the arms stay (rwp_math_0056f500)
            for a, b in zip(T[:-1], R[:-1]):
                if a == b: out.append(b)
                elif a[0] == b[0] == "store" and a[1] == b[1] and a[3] == b[3]:
                    n = mn(st[1], a[2], b[2])
                    if n is None: break
                    out.append(("store", a[1], n, a[3]))
                else: break
            else:
                stmts[i:] = out + [R[-1]]; return

    # ---------------------------------------------------------------- rendering
    def iv(s, v, top=True):
        k = v[0]
        if k in ("add", "sub") and any(x[0] == "var" and x[1].isf() for x in v[1:] if isinstance(x, tuple)):      # byte arithmetic on a float * base: RWP-X9
            r = v[2]; l0 = ("(char *)" + v[1][1].name) if v[1][0] == "var" and v[1][1].isf() else s.iv(v[1], False)
            r0 = ("(char *)" + r[1].name) if r[0] == "var" and r[1].isf() else s.iv(r, False)
            txt = "%s %s %s" % (l0, "+" if k == "add" else "-", r0)
            return txt if top else "(" + txt + ")"
        if k == "addrof": return "&" + v[1].name
        if k == "imm": return str(v[1]) if -9 < v[1] < 10 else ("0x%x" % v[1] if v[1] >= 0 else str(v[1]))
        if k == "var": return s.vname(v[1])
        if k in ("add", "sub"):
            r = v[2]
            if r[0] == "imm" and r[1] < 0: txt = "%s - %s" % (s.iv(v[1], False), s.iv(("imm", -r[1])))
            else: txt = "%s %s %s" % (s.iv(v[1], False), "+" if k == "add" else "-", s.iv(r, False))
            return txt if top else "(" + txt + ")"
        if k == "shr": return ("%s >> %d" % (s.iv(v[1], False), v[2])) if top else "(%s >> %d)" % (s.iv(v[1], False), v[2])
        if k == "un": return {"neg": "-", "not": "~"}[v[1]] + s.iv(v[2], False)
        if k == "shl": return ("%s << %d" % (s.iv(v[1], False), v[2])) if top else "(%s << %d)" % (s.iv(v[1], False), v[2])
        if k == "mul": return ("%s * %d" % (s.iv(v[1], False), v[2])) if top else "(%s * %d)" % (s.iv(v[1], False), v[2])
        if k == "bin":
            txt = "%s %s %s" % (s.iv(v[2], False), {"and": "&", "or": "|", "mul": "*"}[v[1]], s.iv(v[3], False))
            return txt if top else "(" + txt + ")"      # RWP-X9: `(a2 & 0xffff) << 5`, not `a2 & 0xffff << 5`
        if k == "ld": return s.load_txt(v[1], v[2], v[3])
        if k == "cond":
            c = v[1]
            if c[0] == "icmp" and c[1] in ("==", "!=") and c[3] == ("imm", 0): return "(%s %s 0)" % (s.iv(c[2], False), c[1])
            return "(%s)" % s.cond_txt(c)
        if k == "shv": return "(%s %s %s)" % (s.iv(v[1], False), {"shl": "<<", "sar": ">>", "shr": ">>"}[v[3]], s.iv(v[2], False))
        if k == "sel": return "%s ? %s : %s" % (s.cond_txt(v[1]), s.iv(v[2], False), s.iv(v[3], False))
        if k == "ftol": return "(int)" + s.fx(v[1], True)
        raise Unsupported("render iv %s" % (k,))
    def vname(s, v):
        if v.isf(): return v.name
        return v.name
    def addr(s, ad, cast):
        if ad[0] == "slot": return ad[1].name
        if ad[0] == "glob": return "*(%s *)0x%x" % (cast, ad[1])
        iv, disp = ad[1], ad[2]
        if iv[0] == "var" and iv[1].isf() and disp % 4 == 0 and cast == "float": return "%s[%d]" % (iv[1].name, disp // 4)
        b = s.iv(iv, False) if iv[0] != "var" else iv[1].name
        if iv[0] in ("add", "sub", "mul", "shl"): b = s.iv(iv, False)
        off = ("" if disp == 0 else (" + 0x%x" % disp if disp > 0 else " - 0x%x" % -disp))
        return "*(%s *)(%s%s)" % (cast, b if iv[0] in ("var", "add", "sub", "mul", "shl") else b, off) if iv[0] != "var" or not iv[1].isf() else "*(%s *)((char *)%s%s)" % (cast, b, off)
    def load_txt(s, ad, size, signed):
        cast = {4: "int", 2: "short" if signed else "unsigned short", 1: "char" if signed else "unsigned char"}[size]
        return s.addr(ad, cast)
    PREC = {"+": 1, "-": 1, "*": 2, "/": 2}
    def fx(s, n, top=False):
        """float expression text"""
        if n.temp and n.var is not None: return n.var.name
        t, p = s.fxp(n)
        return t
    def fxp(s, n):
        k = n.kind
        if k == "k": return flit(n.a[0], n.a[1]), 9
        if k == "cp": return s.fx(n.a[0]), 9
        if k == "mem":
            ad, kind = n.a
            return s.addr(ad, "float" if kind == "f" else "double"), 9
        if k == "i": return "(float)" + s.addr(n.a[0], {4: "int", 2: "short"}[n.a[1]]), 9
        if k == "callf": return n.a[0][1].name, 9
        if k == "un":
            nm = n.a[0]; x = s.fx(n.a[1])
            if nm == "neg": return "-" + (x if s.fxp(n.a[1])[1] == 9 or n.a[1].temp else "(" + x + ")"), 8
            if nm == "abs": return "(float)fabs(%s)" % x, 9
            return "(float)%s(%s)" % (nm, x), 9
        if k == "fn2": return "(float)%s(%s, %s)" % (n.a[0], s.fx(n.a[1]), s.fx(n.a[2])), 9
        if k == "sel":
            c, A, B = n.a; return "%s ? %s : %s" % (s.cond_txt(c), s.fx(A), s.fx(B)), 0
        if k == "bin" and PAREN == 2 and n.a[0] == "+":      # RWP-X14: cl evaluates a FLAT sum chain `t0 + t1 + t2` as t2, t1, t0: the spelling of an evaluation order e0, e1, e2 is `e2 + e1 + e0`
            def chain(x): return chain(x.a[1]) + [x.a[2]] if x.kind == "bin" and x.a[0] == "+" and not x.temp and x.var is None else [x]
            ts = chain(n)
            if len(ts) >= 3: return " + ".join(("(%s)" % s.fx(x)) if (not x.temp and s.fxp(x)[1] < 2) else s.fx(x) for x in reversed(ts)), 1
        if k == "bin":
            op, A, B = n.a; pr = s.PREC[op]
            at, ap = (s.fx(A), 9 if A.temp else s.fxp(A)[1]); bt, bp = (s.fx(B), 9 if B.temp else s.fxp(B)[1])
            if A.kind == "cp" and A.a[0].dbl: at = "(float)" + at
            if B.kind == "cp" and B.a[0].dbl: bt = "(float)" + bt
            if ap < pr or (ap == pr and PAREN == 1 and A.kind == "bin" and not A.temp and pr == 1): at = "(" + at + ")"      # RWP-X14: cl evaluates a parenthesised left pair in source order, a flat chain reordered
            if bp <= pr and not (bp == pr and op in "+*" and False): bt = "(" + bt + ")" if bp < 9 else bt
            return "%s %s %s" % (at, op, bt), pr
        raise Unsupported("render " + k)

    def cond_txt(s, c):
        if c[0] == "not": return "!(%s)" % s.cond_txt(c[1])
        if c[0] == "fcmp": return "%s %s %s" % (s.fx_c(c[2]), c[1], s.fx_c(c[3]))
        if c[0] == "icmp":
            rel = c[1]
            if rel in ("<0", ">=0"): return "%s %s 0" % (s.iv(c[2], False), rel[:-1])
            if rel.endswith("u"): return "(unsigned)%s %s (unsigned)%s" % (s.iv(c[2], False), rel[:-1], s.iv(c[3], False))
            if rel in ("==", "!=") and c[3] == ("imm", 0): return ("!%s" if rel == "==" else "%s") % s.iv(c[2], False)
            return "%s %s %s" % (s.iv(c[2], False), rel, s.iv(c[3], False))
        if c[0] == "tst": return "%s(%s & %s)" % ("!" if c[1] else "", s.iv(c[2], False), s.iv(c[3], False))
        raise Unsupported("cond")
    def fx_c(s, n):
        t = s.fx(n)
        return t if (n.temp or s.fxp(n)[1] >= 2) else "(" + t + ")"

    def prepare(s, stmts):
        """walk statements: pick float temps (refs>0 or flushed), give them variables in creation order"""
        for st in stmts:
            if st[0] == "maybe":
                n = st[1]
                if n.refs > 0 or n.temp:
                    if n.kind not in ("k", "cp"): n.temp = True; n.var = s.tmp("f"); n.var.dbl = n.dbl
            elif st[0] == "if": s.prepare(st[2]); s.prepare(st[3])
            elif st[0] == "dowhile": s.prepare(st[1])
        return stmts

    def lines(s, stmts, ind):
        pad = "  " * ind; out = []
        for st in stmts:
            k = st[0]
            if k == "return" and st[1] is None and ind == 1 and st is stmts[-1]: continue
            if k == "maybe":
                n = st[1]
                if n.temp and n.var is not None:
                    # temp expression text without referencing itself
                    t, _ = s.fxp(n)
                    out.append("%s%s = %s;" % (pad, n.var.name, t))
            elif k == "store":
                _, ad, n, kind = st; out.append("%s%s = %s;" % (pad, s.addr(ad, "float" if kind == "f" else "double"), s.fx(n)))
            elif k == "istore":
                _, ad, v, sz = st
                if ad[0] == "slot" and ad[1].kind == "f" and v[0] == "imm":
                    out.append("%s%s = %s;" % (pad, ad[1].name, flit(struct.unpack("<f", struct.pack("<I", v[1] & 0xffffffff))[0], "f")))
                else: out.append("%s%s = %s;" % (pad, s.addr(ad, {4: "int", 2: "short", 1: "char"}[sz]), s.iv(v)))
            elif k == "decl":
                _, v, e = st; x = s.iv(e)
                out.append("%s%s = %s;" % (pad, v.name, ("(float *)(" + x + ")") if v.isf() and e[0] != "var" else x))
            elif k == "call":
                _, v, nm, args, rf = st
                al = []
                for a in args:
                    al.append(s.iv(a[1]) if a[0] == "i" else s.fx(a[1]))
                fn = nm if isinstance(nm, str) else "((%s (*)())%s)" % ("float" if v.kind == "f" else "int", s.addr(nm[1], "int"))
                out.append("%s%s = %s(%s);" % (pad, v.name, fn, ", ".join(al)))
            elif k == "return":
                if st[1] is None: out.append(pad + "return;")
                elif st[1] == "f": out.append("%sreturn %s;" % (pad, s.fx(st[2])))
                else: out.append("%sreturn %s;" % (pad, s.iv(st[2])))
            elif k == "copy":
                _, dst, src, cnt = st; out.append("%s*(Blk%d *)(%s) = *(Blk%d *)(%s);" % (pad, cnt, s.iv(dst, False), cnt, s.iv(src, False)))
            elif k == "fassign": out.append("%s%s = %s;" % (pad, st[1].name, s.fx(st[2])))
            elif k == "label": out.append("%sL_%x:;" % ("  " * max(ind - 1, 0), st[1]))
            elif k == "goto": out.append("%sgoto L_%x;" % (pad, st[1]))
            elif k == "goto_if": out.append("%sif (%s) goto L_%x;" % (pad, s.cond_txt(st[1]), st[2]))
            elif k == "dowhile":
                out.append(pad + "do {"); out += s.lines(st[1], ind + 1); out.append("%s} while (%s);" % (pad, s.cond_txt(st[2])))
            elif k == "if":
                if not st[2] and not st[3]: continue      # arms filled lazily by phi declarations: an if that stayed empty is dropped at render time
                la, lb = s.lines(st[2], ind + 1), s.lines(st[3], ind + 1)
                if not la and not lb: continue      # arms that only held x87 value nodes (the ternary took them over)
                out.append("%sif (%s) {" % (pad, s.cond_txt(st[1]))); out += la
                if lb: out.append(pad + "} else {"); out += lb
                out.append(pad + "}")
        return out

    def render(s):
        s.prepare(s.stmts)
        # frame slots the original really stores to (fstp [esp+k] ... fld [esp+k]) are memory resident: cl keeps arrays / address-taken variables there, never scalars it can enregister
        fs = [(e, v) for e, v in s.slots.items() if v.kind == "f" and not getattr(v, "addr_taken", False)]
        s.fvec = None
        if fs:
            emin = min(e for e, _ in fs); n = (max(e for e, _ in fs) - emin) // 4 + 1
            for e, v in fs: v.name = "fv[%d]" % ((e - emin) // 4)
            s.fvec = n
            s.slots = {e: v for e, v in s.slots.items() if not (v.kind == "f" and not getattr(v, "addr_taken", False))}
        # RWP-X8: an address-taken frame slot is an object the callee fills (a vec3 out parameter): the array spans up to the next used slot / the top of the frame
        used = sorted(set(list(s.slots) + [e for e in s.alias if e < 0]))
        top = getattr(s, "frame_hi", None)
        for e in sorted(s.slots):
            v = s.slots[e]
            if getattr(v, "addr_taken", False) and top is not None:
                nxt = min([x for x in used if x > e] + [top]); n = (nxt - e) // 4
                if n > 1: v.arr = n; v.base = v.name; v.name = v.name + "[0]"
        # variable types
        decls = []
        body = s.lines(s.stmts, 1)
        # collect names used
        def vt(v):
            if v.kind == "f": return "float"
            return "float*" if v.isf() else "int"
        locs = []
        for v in s.ptrs: locs.append("  %s %s;" % (vt(v), v.name) if not vt(v).endswith("*") else "  %s%s;" % (vt(v)[:-1] + " *", v.name))
        for e in sorted(s.slots, reverse=True):
            v = s.slots[e]; ty = "float" if v.kind == "f" else "int"
            locs.append("  %s %s[%d];" % (ty, v.base, v.arr) if getattr(v, "arr", 0) else "  %s %s;" % (ty, v.name))
        for e in sorted(s.alias): v = s.alias[e]; locs.append("  %s %s;" % ("float" if v.kind == "f" else "int", v.name))
        # float temps in creation order
        for v in s.tmps: locs.append("  %s %s;" % ("double" if getattr(v, "dbl", False) else "float", v.name))
        if s.fvec: locs.insert(0, "  float fv[%d];" % s.fvec)
        params = []
        for e in sorted(s.args):
            v = s.args[e]; params.append("float %s" % v.name if getattr(v, "fl", False) and not v.uses else ("%s %s" % (vt(v), v.name)).replace("float* ", "float *") if v.kind == "p" else "float %s" % v.name)
        for r, v in s.regargs.items(): params.append("%s %s" % ("int", v.name))
        if s.regargs: raise Unsupported("register arguments %s" % ",".join(s.regargs))
        rt = "float" if s.retf else "int" if s.reti else "void"
        cc = "__stdcall " if s.stdcall else ""
        ext = ["/*%08x*/ extern %s %s();" % (s.callee[c], "float" if c in s.floatcalls else "int", c) for c in sorted(s.callee)]
        hdr = "/* %s: lifted from the original code (tools/x87/lift.py) */\n" % s.name + "%s %s%s(%s)" % (rt, cc, s.name, ", ".join(params) or "void")
        body, locs = inline_single_use(body, locs)
        ext = ["typedef struct { int d[%d]; } Blk%d;" % (n, n) for n in sorted(s.blocks)] + ext
        return "\n".join(["#include <math.h>"] + ext + [hdr, "{"] + locs + [""] * bool(locs) + body + ["}", ""])

def inline_single_use(body, locs):
    """`v1 = expr;` whose variable is read exactly once, later, with only loads / temps in between (no store, call, branch) -> substituted into the use"""
    changed = True
    while changed:
        changed = False
        for i, l in enumerate(body):
            m = re.fullmatch(r"(\s*)([vpr]\d+) = (.*);", l)
            if not m: continue
            v, e = m.group(2), m.group(3)
            if m.group(2)[0] == "r": continue
            uses = [j for j, x in enumerate(body) if j != i and re.search(r"\b%s\b" % v, x)]
            if len(uses) != 1 or uses[0] < i: continue
            j = uses[0]
            if len(re.findall(r"\b%s\b" % v, body[j])) != 1: continue
            if any(not re.fullmatch(r"\s*([vp]\d+|f\d+) = [^\[]*;", x) or re.search(r"\(\*|=.*\(\w+\(", x) and False for x in body[i + 1:j]): continue
            if any(re.match(r"\s*(\*|\w+\[)", x) or "if (" in x or x.strip().startswith("}") or re.search(r"\w\(.*\);", x) and "sqrt" not in x and "(float)" not in x for x in body[i + 1:j]): continue
            pe = e if re.fullmatch(r"[\w.]+|\*\([^()]*\([^()]*\)[^()]*\)|\*\(\w+ \*\)\([^()]*\)", e) else "(" + e + ")"
            if re.fullmatch(r"\(([^()]*)\)", pe) and re.search(r"\*\(\w+ \*\)\($", body[j][:body[j].find(v)]): pe = e      # already inside parens
            body[j] = re.sub(r"\b%s\b" % v, lambda _: pe, body[j]); del body[i]; changed = True; break
    used = "\n".join(body)
    locs = [l for l in locs if not re.search(r"\b([vp]\d+);", l) or re.search(r"\b%s\b" % re.search(r"\b([vp]\d+);", l).group(1), used)]
    return body, locs

def lift(va, name=None):
    L = Lifter(va, name); return L.lift()

if __name__ == "__main__":
    va = int(sys.argv[1], 16)
    try: print(lift(va))
    except Unsupported as e: print("UNSUPPORTED:", e); sys.exit(1)
