#!/usr/bin/env python3
"""t2_x87.py: symbolic x87 stack of the ORIGINAL function for t2_postfix.asm_sites (kind `ftol`).
Ghidra prints `iVar = _ftol();` when the float expression feeding `call __ftol` (fld / fmul / fsub ... on the x87 stack) is invisible to its decompiler: the C text then calls _ftol with an EMPTY x87 stack
(defectlint class 3, the x87 stack top differs in diffemu).  Here the stack is tracked symbolically (trees, interned in TREES; the canonical value string `F:<id>` is what asm_sites stores for a `fstp [esp+k]`
slot, `T:<id>` is the int result of a `call _ftol` in eax) and `render` turns a tree into C text with the helpers of t2_postfix (_cname for the memory operands).
Supported: fld m32/m64/st(i)/1/0, fild m16/m32, fadd/fsub/fsubr/fmul/fdiv/fdivr m32/m64 and the commutative reg forms (faddp/fmulp; fsubp / fdivp / reg-destination forms abort: objdump's Intel syntax swaps them),
fchs fxch, fst/fstp (stack slot -> `F:` value, other memory: dropped), fcom*/ftst (pops only), `call __ftol`.  Anything else (a float returning call, fsin ...) makes the stack unknown (None) and the site is left alone."""
import re, struct

TREES = []      # id -> tree; leaf ('m', canon, 'f'|'d') | ('i', canon, size) | ('k', float) ; ('op', sym, a_id, b_id) | ('neg'|'abs'|'sqrt', a_id)
def mk(t):
    TREES.append(t); return len(TREES) - 1

def _leaf_mem(op, ops, canon):
    m = re.search(r"\b(dword|qword|word|tbyte)\b", ops)
    return m.group(1) if m else None

def step(op, ops, fp, regs, d, mem, canon, ftol_va_site):
    """one x87 instruction. fp: list of tree ids (top last) or None (unknown). -> new fp. canon(operand) -> canonical operand string.  Unsupported instructions return None."""
    if fp is None and op not in ("fninit",): return None
    r = _step(op, ops, fp, regs, d, mem, canon, ftol_va_site)
    if r is None: LASTFAIL[:] = [op, ops]
    return r
LASTFAIL = []
def _step(op, ops, fp, regs, d, mem, canon, ftol_va_site):
    a = ops[0] if ops else ""; b = ops[1] if len(ops) > 1 else ""
    ismem = bool(a) and not a.startswith("st")
    sz = _leaf_mem(op, a, canon) if ismem else None
    def ld(kind):
        c = canon(a)
        if c is None: return None
        ms = re.fullmatch(r"M:\[S(-?\d+)\]", c)
        if ms and mem is not None:      # reload of a value this function stored into the stack slot with fst / fstp
            v = mem.get(int(ms.group(1)))
            if v and v.startswith("F:"): return int(v[2:])
        return mk(("m", c, kind))
    if op in ("fnstsw", "fnstcw", "fldcw", "fwait", "fnclex", "wait"): return fp
    if op == "fld":
        if a.startswith("st("): i = int(a[3:-1]); return fp + [fp[-1 - i]] if len(fp) > i else None
        if sz not in ("dword", "qword"): return None
        v = ld("f" if sz == "dword" else "d")
        return fp + [v] if v is not None else None
    if op == "fld1": return fp + [mk(("k", 1.0))]
    if op == "fldz": return fp + [mk(("k", 0.0))]
    if op == "fild":
        c = canon(a)
        if c is None or sz not in ("dword", "word"): return None
        return fp + [mk(("i", c, 4 if sz == "dword" else 2))]
    if op in ("fst", "fstp"):
        if not fp or not ismem: return None if not fp else (fp[:-1] if op == "fstp" else fp)
        c = canon(a)
        ms = re.fullmatch(r"M:\[S(-?\d+)\]", c or "")
        if sz == "dword" and ms and mem is not None: mem[int(ms.group(1))] = "F:%d" % fp[-1]
        return fp[:-1] if op == "fstp" else fp
    if op in ("fadd", "fsub", "fsubr", "fmul", "fdiv", "fdivr") and ismem and fp:
        if sz not in ("dword", "qword"): return None
        v = ld("f" if sz == "dword" else "d")
        if v is None: return None
        s = {"fadd": "+", "fmul": "*", "fsub": "-", "fdiv": "/"}.get(op)
        l, r = (fp[-1], v) if s else (v, fp[-1])
        s = s or {"fsubr": "-", "fdivr": "/"}[op]
        return fp[:-1] + [mk(("op", s, l, r))]
    base = op[:-1] if op in ("faddp", "fmulp", "fsubp", "fsubrp", "fdivp", "fdivrp") else op
    if base in ("fadd", "fmul", "fsub", "fsubr", "fdiv", "fdivr") and fp and (a.startswith("st") and (b.startswith("st") or not b)):
        pop = base != op
        def ix(x): return 0 if x == "st" else int(x[3:-1])
        di, si = (ix(a), ix(b)) if b else (ix(a), 0)
        if len(fp) <= max(di, si): return None
        sym = {"fadd": "+", "fmul": "*", "fsub": "-", "fsubr": "-", "fdiv": "/", "fdivr": "/"}[base]; rev = base in ("fsubr", "fdivr")
        dv, sv = fp[-1 - di], fp[-1 - si]; r = list(fp)
        r[-1 - di] = mk(("op", sym, sv, dv) if rev else ("op", sym, dv, sv))
        return r[:-1] if pop else r
    if op == "fchs" and fp: return fp[:-1] + [mk(("neg", fp[-1]))]
    if op == "fxch" and len(fp) >= 2:
        i = int(a[3:-1]) if a.startswith("st(") else 1
        if len(fp) <= i: return None
        r = list(fp); r[-1], r[-1 - i] = r[-1 - i], r[-1]; return r
    if op in ("fcom", "fucom", "ftst", "ficom") and fp: return fp
    if op in ("fcomp", "fucomp", "ficomp") and fp: return fp[:-1]
    if op in ("fcompp", "fucompp") and len(fp) >= 2: return fp[:-2]
    if op == "fstp" and a.startswith("st("): return fp[:-1] if fp else None      # fstp st(0): drop
    return None

# ---------------------------------------------------------------------------------------------------------------- rendering
def lit(v, kind):
    """C literal of a float32 / float64 constant (exact round trip) or None"""
    if v != v or v in (float("inf"), float("-inf")): return None
    for n in range(1, 18):
        s = "%.*g" % (n, v)
        if kind == "f":
            if struct.unpack("<f", struct.pack("<f", float(s)))[0] == v: break
        elif float(s) == v: break
    if not re.search(r"[.eE]", s): s += ".0"
    s = s + ("f" if kind == "f" else "")
    return "(%s)" % s if s.startswith("-") else s

def render(tid, leaf):
    """tree -> C text. leaf(canon, kind) -> C text of a memory operand read as float ('f') / double ('d') or None; ('i', canon, size) -> leaf(canon, 'i%d')"""
    t = TREES[tid]
    if t[0] == "u": return leaf("U:%s" % t[1], "u") if t[1] else None      # float result of a call (va), None for a merged / unknown value
    if t[0] == "k": return lit(t[1], "f")
    if t[0] == "m": return leaf(t[1], t[2])
    if t[0] == "i":
        x = leaf(t[1], "i%d" % t[2]); return "(float)%s" % x if x else None
    if t[0] == "op":
        l, r = render(t[2], leaf), render(t[3], leaf)
        return "(%s %s %s)" % (l, t[1], r) if l and r else None
    x = render(t[1], leaf)
    if not x: return None
    return {"neg": "(-%s)", "abs": "fabs(%s)", "sqrt": "sqrt(%s)"}[t[0]] % x
