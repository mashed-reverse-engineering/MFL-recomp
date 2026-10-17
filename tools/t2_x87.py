#!/usr/bin/env python3
"""t2_x87.py: symbolic x87 stack of the ORIGINAL function for t2_postfix.asm_sites (kind `ftol`).
Ghidra prints `iVar = _ftol();` when the float expression feeding `call __ftol` (fld / fmul / fsub ... on the x87 stack) is invisible to its decompiler: the C text then calls _ftol with an EMPTY x87 stack
(defectlint class 3, the x87 stack top differs in diffemu).  Here the stack is tracked symbolically (trees, interned in TREES; the canonical value string `F:<id>` is what asm_sites stores for a `fstp [esp+k]`
slot, `T:<id>` is the int result of a `call _ftol` in eax) and `render` turns a tree into C text with the helpers of t2_postfix (_cname for the memory operands).
Supported: fld m32/m64/st(i)/1/0, fild m16/m32, fadd/fsub/fsubr/fmul/fdiv/fdivr m32/m64 and the commutative reg forms (faddp/fmulp; fsubp / fdivp / reg-destination forms abort: objdump's Intel syntax swaps them),
fchs fxch, fst/fstp (stack slot -> `F:` value, other memory: dropped), fcom*/ftst (pops only), `call __ftol`.  Anything else (a float returning call, fsin ...) makes the stack unknown (None) and the site is left alone.
2026-10-17 (x87-stack-survivor class): fi{add,sub,mul,div}[r] m16/m32, fsqrt fabs fsin fcos fpatan; the compares (fcom/fucom/ficom/ftst + fnstsw + test ah,m / sahf + jcc, and cmp / test r,r + jcc) give PATH CONDITIONS
(mkcond / negate); at a control flow join the stack positions the incoming paths disagree on become `sel` trees (`c1 && c2 ? v1 : c3 ? v2 : v3`, clamps / round-half-away / min / max; the unsigned int idiom `fild; test; jge; fadd 2^32` is
`iu`) instead of an opaque value; render_cse turns shared sub trees (the clamp's condition and branches) into float temps assigned in front of the statement.  A reload of a spilled temporary (fst [slot]; fld [slot]) is the leaf
`m` with the stored tree as fallback (the C variable of the slot is found by tools/t2_unify.py); a `fild` of the int result of an earlier `_ftol` is `T:` (an int temp)."""
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
            if v and v.startswith("F:"): return mk(("m", ("W:" + c[2:]) if int(ms.group(1)) > 0 else c, kind, int(v[2:])))      # W: a parameter slot this function overwrote: not the parameter      # a spilled temporary: the C variable of that slot, else (render fallback) the expression that was stored
            if v and v.startswith(("M:", "M*:", "X:", "Y:")) and kind == "f": c = v      # the slot holds a copy (mov / push) of another operand: read that operand
            if v and v.startswith("I:") and kind == "f" and re.fullmatch(r"I:(?:0x[0-9a-f]+|\d+)", v):
                f_ = struct.unpack("<f", struct.pack("<I", int(v[2:], 0) & 0xffffffff))[0]
                if f_ == f_ and abs(f_) != float("inf"): return mk(("k", f_))
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
        ms = re.fullmatch(r"M:\[S(-?\d+)\]", c)
        if ms and mem is not None and (mem.get(int(ms.group(1))) or "").startswith(("T:", "ret:", "M:", "M*:", "I:", "X:", "Y:")) and sz == "dword": c = mem[int(ms.group(1))]      # reload of the int result of an earlier _ftol stored in a stack slot
        return fp + [mk(("i", c, 4 if sz == "dword" else 2))]
    if op in ("fst", "fstp"):
        if not fp or not ismem: return None if not fp else (fp[:-1] if op == "fstp" else fp)
        c = canon(a)
        ms = re.fullmatch(r"M:\[S(-?\d+)\]", c or "")
        if sz == "dword" and ms and mem is not None: mem[int(ms.group(1))] = "F:%d" % fp[-1]
        return fp[:-1] if op == "fstp" else fp
    if op in ("fadd", "fsub", "fsubr", "fmul", "fdiv", "fdivr", "fiadd", "fisub", "fisubr", "fimul", "fidiv", "fidivr") and ismem and fp:
        if op[1] == "i":      # integer operand (m16 / m32 int converted)
            op = "f" + op[2:]; c = canon(a)
            if c is None or sz not in ("dword", "word"): return None
            ms = re.fullmatch(r"M:\[S(-?\d+)\]", c)
            if ms and mem is not None and (mem.get(int(ms.group(1))) or "").startswith(("T:", "ret:", "M:", "M*:", "I:", "X:", "Y:")) and sz == "dword": c = mem[int(ms.group(1))]
            v = mk(("i", c, 4 if sz == "dword" else 2))
        elif sz not in ("dword", "qword"): return None
        else: v = ld("f" if sz == "dword" else "d")
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
    if op in ("fsqrt", "fabs", "fsin", "fcos") and fp: return fp[:-1] + [mk(({"fsqrt": "sqrt", "fabs": "abs", "fsin": "sin", "fcos": "cos"}[op], fp[-1]))]
    if op == "fpatan" and len(fp) >= 2: return fp[:-2] + [mk(("op", "atan2", fp[-2], fp[-1]))]
    if op == "fxch" and len(fp) >= 2:
        i = int(a[3:-1]) if a.startswith("st(") else 1
        if len(fp) <= i: return None
        r = list(fp); r[-1], r[-1 - i] = r[-1 - i], r[-1]; return r
    if op in ("fcom", "fucom", "ftst", "ficom") and fp: return fp
    if op in ("fcomp", "fucomp", "ficomp") and fp: return fp[:-1]
    if op in ("fcompp", "fucompp") and len(fp) >= 2: return fp[:-2]
    if op == "fstp" and a.startswith("st("): return fp[:-1] if fp else None      # fstp st(0): drop
    return None

CMPOPS = ("fcom", "fcomp", "fcompp", "fucom", "fucomp", "fucompp", "ftst", "ficom", "ficomp")
def cmp_operands(op, ops, fp, canon, mem):
    """(lhs tree, rhs tree) of an x87 compare of the stack top with its operand, or None"""
    if op not in CMPOPS or not fp: return None
    a = ops[0] if ops else ""
    if op == "ftst": return fp[-1], mk(("k", 0.0))
    if op in ("fcompp", "fucompp") or not a: return (fp[-1], fp[-2]) if len(fp) >= 2 else None
    if a.startswith("st"):
        i = 0 if a == "st" else int(a[3:-1]); return (fp[-1], fp[-1 - i]) if len(fp) > i else None
    sz = _leaf_mem(op, a, canon); c = canon(a)
    if c is None: return None
    if op.startswith("fi"):
        if sz not in ("dword", "word"): return None
        return fp[-1], mk(("i", c, 4 if sz == "dword" else 2))
    if sz not in ("dword", "qword"): return None
    ms = re.fullmatch(r"M:\[S(-?\d+)\]", c); v = mem.get(int(ms.group(1))) if ms and mem is not None else None
    if v and v.startswith("F:") and sz == "dword": return fp[-1], mk(("m", ("W:" + c[2:]) if int(ms.group(1)) > 0 else c, "f", int(v[2:])))
    return fp[-1], mk(("m", c, "f" if sz == "dword" else "d"))

# x87 status word tests (after fcom; fnstsw ax; test ah, m; jcc / sahf; jcc): the jump condition as ("f", kind, negated, lhs, rhs) with kind gt / lt / ge / eq / ne for  lhs ? rhs ; unordered is covered by the negations
_FJ = {(0x41, "je"): ("gt", 0), (0x41, "jne"): ("gt", 1), (0x05, "jp"): ("lt", 1), (0x05, "jnp"): ("lt", 0), (0x44, "jp"): ("eq", 1), (0x44, "jnp"): ("eq", 0),
       (0x01, "je"): ("ge", 0), (0x01, "jne"): ("ge", 1), (0x40, "je"): ("ne", 0), (0x40, "jne"): ("ne", 1), (0x45, "jp"): ("gt", 0), (0x45, "jnp"): ("gt", 1)}
_SJ = {"jb": ("lt", 0), "jae": ("lt", 1), "ja": ("gt", 0), "jbe": ("gt", 1), "je": ("eq", 0), "jne": ("eq", 1), "jc": ("lt", 0), "jnc": ("lt", 1), "jz": ("eq", 0), "jnz": ("eq", 1)}
_IJ = {"jl": "<", "jle": "<=", "jg": ">", "jge": ">=", "jb": "<u", "jbe": "<=u", "ja": ">u", "jae": ">=u", "je": "==", "jne": "!=", "jz": "==", "jnz": "!=", "js": "<", "jns": ">=", "jnge": "<", "jnl": ">=", "jng": "<=", "jnle": ">"}
_INV = {"<": ">=", ">=": "<", "<=": ">", ">": "<=", "<u": ">=u", ">=u": "<u", "<=u": ">u", ">u": "<=u", "==": "!=", "!=": "=="}
def mkcond(op, flg):
    """jump condition for the conditional jump `op` after the flag state flg, or None"""
    jj = {"jz": "je", "jnz": "jne", "jpe": "jp", "jpo": "jnp"}.get(op, op)
    if not flg: return None
    if flg[0] == "ahm":
        k = _FJ.get((flg[1], jj)); return ("f", k[0], k[1], flg[2][0], flg[2][1]) if k else None
    if flg[0] == "sahf":
        k = _SJ.get(op); return ("f", k[0], k[1], flg[1][0], flg[1][1]) if k else None
    if flg[0] in ("icmp", "itest") and op in _IJ: return ("i", _IJ[op], flg[1], flg[2] if flg[0] == "icmp" else "I:0x0")
    return None
def negate(c):
    if c is None: return None
    if c[0] == "f": return ("f", c[1], 1 - c[2], c[3], c[4])
    if c[0] == "i": return ("i", _INV[c[1]], c[2], c[3])
    return ("?", c[1], "n") if len(c) == 2 else ("?", c[1])
def sel(vals):
    """value of one stack position from incoming (value tree, path condition tuple) pairs: a chain `c1 ? v1 : c2 ? v2 : v_last`; None when a path condition is unknown"""
    if any(c is None or c[0] == "?" for _, pc in vals for c in pc): return None
    while len(vals) > 1 and vals[-2][0] == vals[-1][0]: vals = vals[:-2] + vals[-1:]      # a branch with the value of the default is no branch
    if len(vals) == 1: return vals[0][0]
    return mk(("sel", tuple((tuple(pc), v) for v, pc in vals[:-1]), vals[-1][0]))

FALLBACK = []      # slot canons rendered by the stored expression (no C variable known for the spill slot)
RODATA = [None]      # t2_postfix sets: va -> bytes of a read only data address or None
def _is2p32(t):
    if t == ("k", 4294967296.0): return True
    m = re.fullmatch(r"M:ds:0x([0-9a-f]+)", t[1]) if t[0] == "m" and t[2] == "f" else None
    b = RODATA[0](int(m.group(1), 16)) if m and RODATA[0] else None
    return b is not None and struct.unpack("<f", b[:4])[0] == 4294967296.0
def join(ids):
    """value of one stack position that the incoming paths of a join disagree on: the unsigned int idiom (`fild m32; test r,r; jge L; fadd 2^32; L:`: I and I + 4294967296.0) -> ('iu', ..), else opaque"""
    for a in ids:
        for b in ids:
            tb = TREES[b]
            if TREES[a][0] == "i" and tb[0] == "op" and tb[1] == "+" and tb[2] == a and _is2p32(TREES[tb[3]]) and set(ids) <= {a, b}: return mk(("iu",) + TREES[a][1:])
    return mk(("u", 0))

# ---------------------------------------------------------------------------------------------------------------- rendering
def lit(v, kind):
    """C literal of a float32 / float64 constant (exact round trip) or None"""
    if v != v or v in (float("inf"), float("-inf")): return None
    for n in range(1, 18):
        s = "%.*g" % (n, v)
        if kind == "f":
            if struct.unpack("<f", struct.pack("<f", float(s)))[0] == v: break
        elif float(s) == v: break
    if re.search(r"[eE]", s) and 1e-4 <= abs(float(s)) < 1e15: s = repr(float(s))      # 360 and not 3.6e+02
    if not re.search(r"[.eE]", s): s += ".0"
    s = s + ("f" if kind == "f" else "")
    return "(%s)" % s if s.startswith("-") else s

def rcond(c, leaf, sub=None):
    """C text of a path condition or None"""
    if c[0] == "f":
        l, r = (sub or (lambda i: render(i, leaf)))(c[3]), (sub or (lambda i: render(i, leaf)))(c[4])
        if not l or not r: return None
        e = {"gt": "%s > %s", "lt": "%s < %s", "ge": "%s >= %s", "eq": "%s == %s", "ne": "(%s < %s || %s > %s)"}[c[1]]
        e = e % ((l, r) if c[1] != "ne" else (l, r, l, r))
        return "!(%s)" % e if c[2] else "(%s)" % e if c[1] != "ne" else e
    if c[0] == "i":
        u = c[1].endswith("u"); op = c[1].rstrip("u"); a, b = leaf(c[2], "i4"), leaf(c[3], "i4")
        if not a or not b: return None
        return "((uint)%s %s (uint)%s)" % (a, op, b) if u else "(%s %s %s)" % (a, op, b)
    return None

def _kids(t):
    if t[0] == "m": return [t[3]] if len(t) > 3 else []
    if t[0] == "op": return [t[2], t[3]]
    if t[0] in ("neg", "abs", "sqrt", "sin", "cos"): return [t[1]]
    if t[0] == "sel": return [v for _, v in t[1]] + [t[2]] + [x for pcs, _ in t[1] for c in pcs if c[0] == "f" for x in c[3:5]]
    return []
def _intop(t, r):
    """`x op (float)n` with an integer right operand is `x op n` in C (the compiler emits fiadd / fimul / fidiv: the int enters the x87 exactly, a (float) cast would round it to 24 bits)"""
    if r and r.startswith("(float)") and TREES[t[3]][0] == "i" and t[1] != "atan2": return r[len("(float)"):]
    return r
def render_cse(tid, leaf, newname):
    """render with common subexpressions (a node reached twice: the clamp conditions and branches of a sel) as float temps -> (text, [(temp, text)] in dependency order)"""
    cnt = {}
    def walk(i):
        cnt[i] = cnt.get(i, 0) + 1
        if cnt[i] == 1:
            for k in _kids(TREES[i]): walk(k)
    walk(tid); memo = {}; assigns = []
    def rend(i, top=False):
        if i in memo: return memo[i]
        e = _render1(i, leaf, rend)
        if e is not None and not top and cnt.get(i, 0) >= 2 and TREES[i][0] in ("op", "sel", "neg", "abs", "sqrt", "sin", "cos"):
            nm = newname(); assigns.append((nm, e)); memo[i] = nm; return nm
        return e
    return rend(tid, True), assigns
def _render1(tid, leaf, sub):
    """one node of render() with sub(i) for its children"""
    t = TREES[tid]
    if t[0] == "sel":
        out = sub(t[2])
        for pcs, v in reversed(t[1]):
            cs = [rcond(c, leaf, sub) for c in pcs]; vv = sub(v)
            if not out or not vv or not all(cs): return None
            out = "(%s ? %s : %s)" % (" && ".join(cs) if cs else "1", vv, out)
        return out
    if t[0] == "op":
        l, r = sub(t[2]), sub(t[3])
        r = _intop(t, r)
        if t[1] == "atan2": return "atan2(%s, %s)" % (l, r) if l and r else None
        return "(%s %s %s)" % (l, t[1], r) if l and r else None
    if t[0] in ("neg", "abs", "sqrt", "sin", "cos"):
        x = sub(t[1])
        return {"neg": "(-%s)", "abs": "fabs(%s)", "sqrt": "sqrt(%s)", "sin": "sin(%s)", "cos": "cos(%s)"}[t[0]] % x if x else None
    if t[0] == "m" and len(t) > 3:
        r = leaf(t[1], t[2])
        if r is None: FALLBACK.append(t[1]); return sub(t[3])
        return r
    return render(tid, leaf)

def render(tid, leaf):
    """tree -> C text. leaf(canon, kind) -> C text of a memory operand read as float ('f') / double ('d') or None; ('i', canon, size) -> leaf(canon, 'i%d')"""
    t = TREES[tid]
    if t[0] == "u": return leaf("U:%s@%d" % (t[1], t[2]) if len(t) > 2 else "U:%s" % t[1], "u") if t[1] else None      # float result of a call (va), None for a merged / unknown value
    if t[0] == "k": return lit(t[1], "f")
    if t[0] == "m":
        r = leaf(t[1], t[2])
        if r is None and len(t) > 3: FALLBACK.append(t[1]); return render(t[3], leaf)
        return r
    if t[0] == "i":
        x = leaf(t[1], "i%d" % t[2]); return "(float)%s" % x if x else None
    if t[0] == "sel":
        out = render(t[2], leaf)
        for pcs, v in reversed(t[1]):
            cs = [rcond(c, leaf) for c in pcs]; vv = render(v, leaf)
            if not out or not vv or not all(cs): return None
            out = "(%s ? %s : %s)" % (" && ".join(cs) if cs else "1", vv, out)
        return out
    if t[0] == "iu":      # unsigned 32 bit int converted
        x = leaf(t[1], "i%d" % t[2]); return "(float)(uint)%s" % x if x else None
    if t[0] == "op":
        l, r = render(t[2], leaf), render(t[3], leaf)
        r = _intop(t, r)
        if t[1] == "atan2": return "atan2(%s, %s)" % (l, r) if l and r else None
        return "(%s %s %s)" % (l, t[1], r) if l and r else None
    x = render(t[1], leaf)
    if not x: return None
    return {"neg": "(-%s)", "abs": "fabs(%s)", "sqrt": "sqrt(%s)", "sin": "sin(%s)", "cos": "cos(%s)"}[t[0]] % x
