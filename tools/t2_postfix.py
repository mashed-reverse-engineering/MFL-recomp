#!/usr/bin/env python3
"""t2_postfix.py FILE... : mechanical, diffemu-verified text fixes for generated tier 2 candidates (used by t2_auto.py before the compile stage).
  fbits  a float expression passed to an integer parameter / a `()` callee: the original pushes the 4 float BYTES (Ghidra prints `fld; fstp [esp]` as the float variable and types the
         parameter undefined4); C converts (_ftol) or widens to double.  -> `*(int *)&x` (lvalue) or `(fbN = x, *(int *)&fbN)` with a float temp at the top of the function.
  fcast  `(float)p[-26]` of a dword element (fild conversion) -> `*(float *)&p[-26]`.
  fstore `dword = (int)(float expr);` -> `*(float *)&dword = float expr;`.
  dropextra  `f(a, b, stale, stale)` with f taking 2 parameters (original reads 2 slots): the surplus plain-variable arguments are dropped.
  sgn    undefined4 object compared with a signed value (defectlint class 1): the operand gets `(int)`.
  enum   names of names/MFL_enums.txt used but not defined (C2065: COURSE_ANGELS, WEAPON_MINE ...): `#define NAME value` below the includes.
  argfill  `f()` (Ghidra dropped the arguments: unprototyped / K&R callee) where EVERY call of f in the original function pushes the same constants (immediates / addresses): `f(c1, c2)`.
  tailfwd  callee that rewrites its stack argument and tail-jumps (orig_stackargs flag wN) so it really takes the tail target's slot count, but the extern has fewer: the extern gets the real parameter count, the call gets the stale extra arguments Ghidra attached to the NEXT call back (`mem_zero(&g, n, str, 0, 0)` after `f(str)`).
  isnan  Ghidra's `NAN(x)` -> `((x) != (x))` (the prelude's NAN is the constant; also applied by t2_auto.fix_round before generation).
  argsite  `f()` / `f_r(a)` with fewer arguments than the callee takes (C2198; Ghidra dropped them: K&R / register callee): simulation of the ORIGINAL function's call sites (asm_sites: pushes, register definitions, esp-relative slots) gives each missing argument as an immediate, a global, a parameter / local_N slot, the result variable of the call just before, or the C argument another call site passes for the same asm value; all or nothing per call.
  varargs  a call with more arguments than the extern has, whose callee takes the address of its argument area (orig_stackargs flag addr, slots == declared): the extern becomes variadic (`, ...`).
  unused  an extern prototype whose name occurs nowhere else in the file is dropped (autocompile also declares names that only appear in the comment: `start` of course_lap_line_find_index -> a false "register callee called plain").
  headjoin  a definition head Ghidra wrapped after the calling convention (`T * __cdecl` / newline / `name(`) is joined (core.defs matches one line).
  frame  defectlint class 9: the cluster of stack-slot locals (local_N / uStack_N / auStack_N, gaps <= 8 bytes, known sizes) around a flagged slot becomes ONE struct local `frame` with the Ghidra offsets (padding members for gaps); uses are renamed `frame.local_N`.
fix(path) -> list of applied fix kinds (file rewritten in place)."""
import os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
LV = re.compile(r"^\**[A-Za-z_]\w*(?:\[[^\[\]]+\]|\.\w+|->\w+)*$")      # an lvalue: its address gives the bits
def fbits_expr(t, pos, a, decls, ctr):
    """the int bits of the float expression `a` at text offset pos: `*(int *)&x` for an lvalue, else a float temp declared at the top of the enclosing function: `(fbN = a, *(int *)&fbN)`"""
    if LV.match(a) and not re.search(r"\bdouble\s+\**%s\b" % re.escape(re.sub(r"\W.*", "", a.lstrip("*"))), t): return "*(int *)&%s" % a
    ctr[0] += 1; v = "fbits%d" % ctr[0]
    m = None
    for m in re.finditer(r"^\{[ \t]*\n", t[:pos], re.M): pass
    at = m.end() if m else 0
    decls.append((at, at, "  float %s;\n" % v))
    return "(%s = %s, *(int *)&%s)" % (v, a, v)
_ENUM = {}
def enums():
    if not _ENUM:
        for l in open(os.path.join(G, "names", "MFL_enums.txt")):
            p = l.strip().split(",")
            if len(p) == 3 and p[0] == "v": _ENUM[p[2]] = p[1]
    return _ENUM
def _args(t, i):
    """t[i] == '(' -> list of (start, end) of the top level arguments, end index of ')'"""
    d, st, out, j = 0, i + 1, [], i
    while j < len(t):
        c = t[j]
        if c in "([": d += 1
        elif c in ")]":
            d -= 1
            if d == 0: out.append((st, j)); return out, j
        elif c == "," and d == 1: out.append((st, j)); st = j + 1
        j += 1
    return out, -1
def isnan(t):
    """Ghidra's `NAN(x)` (x is NaN) next to the prelude's object-like NAN constant: -> `((x) != (x))`"""
    while True:
        m = re.search(r"(?<![\w.>])NAN\(", t)
        if not m: return t
        al, close = _args(t, m.end() - 1)
        if close < 0 or len(al) != 1: return t
        a = t[al[0][0]:al[0][1]].strip(); t = t[:m.start()] + "((%s) != (%s))" % (a, a) + t[close + 1:]
_FN = {}; _STD = {}
def orig_const_args(name, va_callees):
    """-> {callee va: tuple of constant argument texts} for callees whose every call site in the ORIGINAL function `name` pushes the same immediates (count from `add esp,N` / stdcall ret N / slots read)"""
    import collections, subprocess, reginputs2 as R
    if not _FN:
        for l in open(os.path.join(G, "output", "fnsizes.txt")):
            q = l.split()
            if len(q) == 3: _FN[q[1]] = (int(q[0], 16), int(q[2]))
        for l in open(os.path.join(G, "output", "stdcall.txt")):
            q = l.split()
            if len(q) == 2: _STD[q[0]] = int(q[1]) // 4
        for l in open(os.path.join(G, "output", "orig_stackargs.txt")):
            q = l.split()
            if q: _STD.setdefault("s" + q[0], int(q[1]))
    if name not in _FN: return {}
    lo, sz = _FN[name]
    o = subprocess.run(["objdump", "-w", "-M", "intel", "-D", "--no-show-raw-insn", "--start-address=0x%x" % lo, "--stop-address=0x%x" % (lo + sz), R.EXE], capture_output=True, text=True).stdout
    ins, order = R.parse_objdump(o); sites = collections.defaultdict(list)
    for i, a in enumerate(order):
        op, ar = ins[a]
        if op != "call" or not re.fullmatch(r"0x[0-9a-f]+", ar): continue
        va = "%08x" % int(ar, 16)
        if va not in va_callees: continue
        nx = ins.get(order[i + 1]) if i + 1 < len(order) else None
        m = nx and nx[0] == "add" and re.fullmatch(r"esp,0x([0-9a-f]+)", nx[1])
        k = int(m.group(1), 16) // 4 if m else _STD.get(va, _STD.get("s" + va, 0))
        ps = []; j = i - 1
        while j >= 0 and len(ps) < k:
            o2, a2 = ins[order[j]]
            if o2 == "call" or o2 == "ret" or o2.startswith("j"): break
            if o2 == "push": ps.append(a2)
            j -= 1
        sites[va].append(tuple(ps[::-1]) if len(ps) == k else None)
    out = {}
    for va, s in sites.items():
        if len(set(s)) == 1 and s[0] and all(re.fullmatch(r"0x[0-9a-f]+|\d+", x) for x in s[0]): out[va] = s[0]
    return out
_SL = {}
def orig_slots(va):
    """stack argument slots the original function reads (orig_stackargs.txt) if its row has no tail / addr / dyn / flag column, else None"""
    if not _SL:
        for l in open(os.path.join(G, "output", "orig_stackargs.txt")):
            q = l.split()
            if len(q) >= 6: _SL[q[0]] = int(q[1]) if q[3] == "-" and q[2] == "0" and not any(x in q[4:-1] for x in ("dyn", "addr", "ijmp")) and not re.match(r"w\d|dyn|addr", q[4]) else None
    return _SL.get(va)
def ext_va(t, name):
    m = re.search(r"^/\*([0-9a-f]{8})\*/ extern [^;]*?\b%s\(" % re.escape(name), t, re.M)
    return m.group(1) if m else None
_O = {}
def orig():
    if not _O:
        for l in open(os.path.join(G, "output", "orig_stackargs.txt")):
            q = l.split()
            if len(q) >= 6: _O[q[0]] = (int(q[1]), q[3].split(","), q[4:-1])
    return _O
def tail_true(va):
    """parameters of a function that rewrites slot(s) and tail-jumps (flag wN): the tail target's slot count; else 0"""
    o = orig().get(va)
    if not o or o[1] == ["-"] or not any(re.fullmatch(r"w\d+", f) for f in o[2]): return 0
    ts = [orig().get(t) for t in o[1]]
    return max([o[0]] + [x[0] for x in ts if x]) if all(ts) else 0
def tailfwd(t, nm=None):
    """-> (new text, n fixed)"""
    ext = {}
    for m in re.finditer(r"^/\*([0-9a-f]{8})\*/ extern ([^;]*?\b)(\w+)\(([^)]*)\);", t, re.M):
        ps = m.group(4).strip(); ext[m.group(3)] = (m.group(1), 0 if ps in ("", "void") else ps.count(",") + 1, m)
    todo = {n: tail_true(v[0]) for n, v in ext.items()}
    todo = {n: k for n, k in todo.items() if k > ext[n][1] and ext[n][1] > 0 and "..." not in ext[n][2].group(4)}
    if not todo: return t, 0
    calls = [(m.start(), m.group(1)) for m in re.finditer(r"(?<![\w.>])(\w+)\(", t) if m.group(1) in ext and t[t.rfind("\n", 0, m.start()) + 1:m.start()].strip() not in ("extern",) and not t[t.rfind("\n", 0, m.start()) + 1:].startswith(("/*", "//"))]
    edits = []; fixed = set(); used = set()
    for i, (p, n) in enumerate(calls):
        if n not in todo or p in used: continue
        k = todo[n]; al, close = _args(t, p + len(n))
        if len(al) != ext[n][1]: continue
        for p2, n2 in calls[i + 1:]:
            if "\n}\n" in t[p:p2]: break
            if p2 < close: continue
            al2, close2 = _args(t, p2 + len(n2)); d = ext[n2][1]
            if len(al2) >= d + k:
                ex = [t[s:e].strip() for s, e in al2[d:d + k]]
                if not all(re.fullmatch(r"[\w.]+", x) for x in ex): break
                edits.append((close, close, "".join("," + x for x in ex[len(al):])))
                edits.append((al2[d - 1][1], al2[d + k - 1][1], ""))
                fixed.add(n); used.add(p)
            break
    # no stale arguments to take back: every call of the original pushes the same constants (immediates / addresses)
    left = {n for p, n in calls if n in todo and p not in used}
    ca = orig_const_args(nm, {ext[n][0] for n in left}) if left and nm else {}
    for p, n in calls:
        a = ca.get(ext[n][0]) if n in left and p not in used else None
        if a and len(a) == todo[n]:
            al, close = _args(t, p + len(n))
            if len(al) == ext[n][1]: edits.append((close, close, "".join(", " + x for x in a[len(al):]))); fixed.add(n); used.add(p)
    if not edits: return t, 0
    for n in fixed:
        m = ext[n][2]; k = todo[n]; ps = m.group(4); a, b = m.span(4)
        edits.append((b, b, "".join(", undefined4 p%d" % j for j in range(ext[n][1], k))))
    for s, e, x in sorted(edits, reverse=True): t = t[:s] + x + t[e:]
    return t, len(fixed)

# ---- argsite: arguments Ghidra dropped at a call (`f()` / `f_r()`), recovered from the original call sites
_RV = {}
def reg_va(name):
    """original function address of a register callee `name` (output/reginputs.csv)"""
    if not _RV:
        import csv
        for r in csv.reader(open(os.path.join(G, "output", "reginputs.csv"))):
            if len(r) > 2: _RV.setdefault(r[0], r[1])
    return _RV.get(name)
_RG = ("eax", "ecx", "edx", "ebx", "esi", "edi", "ebp")
def _canon(o, regs, d):
    """operand text -> canonical value string: I:<imm> | A:<lea addr> | M:<mem> | R:<opaque reg> | ret:<callee va> (esp relative addresses become entry relative S<k>)"""
    o = re.sub(r"^(?:dword |byte |word )?ptr ", "", o.strip())
    if re.fullmatch(r"0x[0-9a-f]+|\d+", o): return "I:" + o
    if o in _RG: return regs.get(o) or "R:" + o
    if o.startswith("ds:"): return "M:" + o
    m = re.fullmatch(r"\[(\w+)(?:\+0x([0-9a-f]+))?\]", o)
    c0 = regs.get(m.group(1)) if m and m.group(1) in _RG else None
    if c0 and c0.startswith("A:[S"):      # [reg] with reg = &slot: the slot (or the next ones)
        k0 = re.fullmatch(r"A:\[S(-?\d+)\]", c0)
        if k0: return "M:[S%d]" % (int(k0.group(1)) + (int(m.group(2), 16) if m.group(2) else 0))
    if c0 and c0.startswith(("M:", "M*:", "ret:")): return "M*:%s|%d" % (c0, int(m.group(2), 16) if m.group(2) else 0)
    mx = re.fullmatch(r"\[(\w+)\*(\d)\+0x([0-9a-f]+)\]", o)      # [idx*scale + global]: element idx of the table at that address
    if mx and mx.group(1) in _RG and regs.get(mx.group(1)) and not regs[mx.group(1)].startswith(("R:", "?")): return "X:%s|%s|%08x" % (regs[mx.group(1)], mx.group(2), int(mx.group(3), 16))
    m = re.fullmatch(r"\[(.*)\]", o)
    if m:
        def sp(q): return "S%d" % (d + (int(q.group(1).replace("+", ""), 16) if q.group(1) else 0))
        return "M:[" + re.sub(r"\besp([+-]0x[0-9a-f]+)?", sp, m.group(1)) + "]"
    return None
def asm_sites(nm, vas):
    """-> {va: [site, ...]} (address order, straight-line simulation) of the calls of `vas` in the ORIGINAL function nm; site = dict(addr, stack=[canon, ..] top of stack first (up to 8), regs={reg: canon}, prev=(callee va of the last call, its address)); None when the stack does not balance at a `ret` (unknown stdcall callee)"""
    import subprocess, reginputs2 as R
    orig_const_args(nm, set())          # fills _FN / _STD
    if nm not in _FN: return None
    lo, sz = _FN[nm]
    o = subprocess.run(["objdump", "-w", "-M", "intel", "-D", "--no-show-raw-insn", "--start-address=0x%x" % lo, "--stop-address=0x%x" % (lo + sz), R.EXE], capture_output=True, text=True).stdout
    ins, order = R.parse_objdump(o)
    vas_i = {int(v, 16) for v in vas}
    tg = {int(a, 16) for op, a in ins.values() if op.startswith("j") and re.fullmatch(r"0x[0-9a-f]+", a)}
    single = {}      # callee saved register written exactly once in the function, by an immediate (xor r,r / mov r,imm): its value at every later point
    wr = {}
    for a in order:
        op, ar = ins[a]; ops = split_args(ar)
        if not ops or op in ("cmp", "test", "push", "pop") or op.startswith("j") or op == "call": continue
        r = __import__("reginputs2").FULL.get(ops[0], "x")
        if r in ("ebx", "esi", "edi", "ebp"):
            wr.setdefault(r, []).append((a, "I:0x0" if op in ("xor", "sub") and len(ops) == 2 and ops[0] == ops[1] else ("I:" + ops[1] if op == "mov" and len(ops) == 2 and re.fullmatch(r"0x[0-9a-f]+", ops[1]) else None)))
    single = {r: w[0] for r, w in wr.items() if len(w) == 1 and w[0][1]}
    back = {int(ar, 16) for a, (op, ar) in ins.items() if op.startswith("j") and re.fullmatch(r"0x[0-9a-f]+", ar) and int(ar, 16) <= a}
    def run(single):
        late = {}; snaps = {}; fall = True
        mem, regs, d, out, prev, ok, dmap, cont = {}, {}, 0, {}, None, True, {}, True      # mem: entry relative stack slot -> canonical value; dmap: esp delta at jump targets
        dbp = [0, False]      # esp delta at `mov ebp, esp`, stack aligned (`and esp, -16`: entry relative slots are meaningless)
        def slot(o): return mem.get(o) or ("M:[S%d]" % o if o > 0 else "?")
        for a in order:
            op, ar = ins[a]
            if a in tg:      # control flow join: registers all incoming forward edges agree on (a loop head, target of a backward jump, keeps nothing)
                cand = snaps.get(a, []) + ([regs] if fall else [])
                regs = {} if a in back or not cand else {r: v for r, v in cand[0].items() if v is not None and all(c.get(r) == v for c in cand)}
            fall = True
            for r_, (wa, wc) in single.items():
                if a > wa and r_ not in regs: regs[r_] = wc
            if a in dmap: d = dmap[a]
            ops = split_args(ar)
            if op.startswith("j") and ops and re.fullmatch(r"0x[0-9a-f]+", ops[0]):
                t_ = int(ops[0], 16); dmap.setdefault(t_, d); snaps.setdefault(t_, []).append(dict(regs))
                if op == "jmp": fall = False
                if op == "jmp" and not (lo <= t_ < lo + sz) and t_ in vas_i: out.setdefault("%08x" % t_, []).append(dict(addr=a, stack=[slot(d + 4 + 4 * j) for j in range(8)], regs=dict(regs), prev=prev, tail=True, al=dbp[1]))
            if op == "push" and ops:
                d -= 4; mem[d] = _canon(ops[0], regs, d + 4) or "?"
            elif op == "pop" and ops: d += 4; regs.pop(ops[0], None)
            elif op == "mov" and ops == ["ebp", "esp"]: dbp[0] = d; regs["ebp"] = None
            elif op == "mov" and ops == ["esp", "ebp"]: d = dbp[0]
            elif op == "and" and ops and ops[0] == "esp": dbp[1] = True
            elif op in ("add", "sub") and ops and ops[0] == "esp" and len(ops) == 2 and re.fullmatch(r"0x[0-9a-f]+", ops[1]):
                n = int(ops[1], 16); d += n if op == "add" else -n
            elif op == "call":
                m = re.fullmatch(r"0x([0-9a-f]+)", ar)
                va = "%08x" % int(m.group(1), 16) if m else None
                if va in vas: out.setdefault(va, []).append(dict(addr=a, stack=[slot(d + 4 * j) for j in range(8)], regs=dict(regs), prev=prev, al=dbp[1]))
                k = _STD.get(va, 0) if va else 0
                d += k * 4
                regs = {r: v for r, v in regs.items() if r not in ("eax", "ecx", "edx")}
                if va: regs["eax"] = "ret:" + va
                prev = (va, a)
            elif op in ("ret", "retn"):
                fall = False
                if d != 0: ok = False; DBG.append((a, d))
            elif op == "mov" and len(ops) == 2 and ops[0] in _RG:
                regs[ops[0]] = _canon(ops[1], regs, d)
                if ops[0] in sc and regs[ops[0]] and not regs[ops[0]].startswith(("R:", "?", "ret:")): late[ops[0]] = (a, regs[ops[0]])
            elif op == "mov" and len(ops) == 2 and re.fullmatch(r"(?:dword ptr )?\[esp(?:\+0x[0-9a-f]+)?\]", ops[0]):
                mem[d + (int(re.search(r"\+0x([0-9a-f]+)", ops[0]).group(1), 16) if "+" in ops[0] else 0)] = _canon(ops[1], regs, d) or "?"
            elif op == "lea" and len(ops) == 2 and ops[0] in _RG:
                c = _canon(ops[1], regs, d); regs[ops[0]] = "A:" + c[2:] if c and c.startswith("M:") else None
            elif op in ("xor", "sub") and len(ops) == 2 and ops[0] == ops[1] and ops[0] in _RG: regs[ops[0]] = "I:0x0"
            elif ops and op not in ("cmp", "test", "push") and __import__("reginputs2").FULL.get(ops[0], "x") in _RG: regs[__import__("reginputs2").FULL[ops[0]]] = None
            elif op in ("fnstsw", "cdq"): regs.pop("eax" if op == "fnstsw" else "edx", None)
        return out, ok, late
    sc = {r for r, w in wr.items() if len(w) == 1}      # written once (mov): its value after that point, whatever the control flow
    out, ok, late = run(single)
    if late: out, ok, _ = run({**late, **single})
    return out if ok else None
def split_args(a): return [x.strip() for x in re.split(r",(?![^\[]*\])", a)] if a else []
_XP = dict(tail=False, own=0, max=0)
def _cname(c, t, vas, cn, nm):
    """canonical value -> C expression in text t, or None"""
    if c is None or c.startswith(("R:", "?")): return None
    if c.startswith("I:"): return c[2:]
    if c.startswith("ret:"): return _XP["ret"](c) if _XP.get("ret") else None
    if c.startswith("M*:"):
        i, n_ = c[3:].rsplit("|", 1); x = _cname(i, t, vas, cn, nm)
        if not x: return None
        if re.fullmatch(r"\w+", x) and re.search(r"^\s*(?:int|uint|undefined4)\s+(?:\w+\s*,\s*)*%s\b" % re.escape(x), t, re.M) or x.startswith(("*(int *)(", "*(undefined4 *)(", "*(uint *)(")): return "*(int *)(%s + 0x%x)" % (x, int(n_))      # Ghidra's own form for an int-typed base
        return "*(int *)((char *)%s + %s)" % (x, n_)
    if c.startswith("X:"):
        i, sc, ad = c[2:].rsplit("|", 2); x = _cname(i, t, vas, cn, nm); g = vas.get(ad)
        return "(&%s)[%s]" % (g, x) if x and g and sc == "4" else None
    k, v = c[0], c[2:]
    m = re.fullmatch(r"ds:0x([0-9a-f]+)", v)
    if m and k in "MA":
        g = vas.get("%08x" % int(m.group(1), 16))
        if not g: return None
        return g if k == "M" else "&" + g
    m = re.fullmatch(r"\[S(-?\d+)\]", v)
    if m and k in "MA":
        s = int(m.group(1))
        if s > 0 and s % 4 == 0 and k == "M":
            h = re.search(r"^[A-Za-z_][^;{}]*?\b%s\s*\(([^)]*)\)\s*\{" % re.escape(nm), t, re.M)
            ps = [re.findall(r"\w+", p)[-1] for p in h.group(1).split(",") if re.findall(r"\w+", p)] if h and h.group(1).strip() not in ("", "void") else []
            if 0 < s // 4 <= len(ps): return ps[s // 4 - 1]
            if _XP["tail"] and s // 4 <= 8 and len(ps) == _XP["own"]: _XP["max"] = max(_XP["max"], s // 4); return "unused%d" % (s // 4)      # tail forwarding passes the caller's frame on: the slots behind our parameters become (unused) parameters of the definition
            return None
        if s < 0:
            ns = set(re.findall(r"(?<![\w.])((?:frame\w*\.)?(?:local|[a-zA-Z]+Stack)_%x)\b" % -s, t))
            ns = {x for x in ns if "." in x or not any(y.endswith("." + x) for y in ns)}      # the struct member declaration `local_c;` next to its uses `frame.local_c`
            if len(ns) != 1: return None
            n = ns.pop()
            if re.search(r"\b%s\s*\[" % re.escape(n.split(".")[-1]), t): return n if k == "A" else n + "[0]"
            return ("&" + n) if k == "A" else n
    return None
DBG = []      # (address, esp delta) of the `ret`s where asm_sites lost the stack balance (debugging)
_PMK = {}
def pushmin_slots(va):
    """min(stack slots the original reads, pushes pending at every call site) of the callee at va: what a prototype must have at least (0 if unknown / no consensus)"""
    if not _PMK:
        pm = {l.split()[0]: int(l.split()[1]) for l in open(os.path.join(G, "output", "call_pushmin.txt")) if len(l.split()) >= 2}
        for va_, (sl, _, fl) in orig().items(): _PMK[va_] = min(max(sl, tail_true(va_)), pm.get(va_, 0))      # a tail-forwarder really takes its target's slots
    return _PMK.get(va, 0)
def argsite(t, nm):
    """-> (new text, number of calls filled): `f()` / `f_r(a)` whose original call site pushes / loads more arguments than Ghidra printed. Each missing argument comes from the original call site (asm_sites): an immediate, a global, a local / parameter slot (Ghidra's local_N), the result variable of the call just before (`v = P(..); f()` with eax pushed), or the C argument another call site of f passes for the same asm value. All or nothing per call; diffemu verifies."""
    ext = {}      # callee name -> (va, regs, nstack)
    for m in re.finditer(r"^/\*([0-9a-f]{8})\*/ extern [^;]*?\b(\w+)\(([^)]*)\);", t, re.M):
        ps = m.group(3).strip(); ext[m.group(2)] = (m.group(1), [], 0 if ps in ("", "void") else ps.count(",") + 1)
    for m in re.finditer(r"^static [^;]*?\b(\w+)_r\(([^)]*)\);", t, re.M):
        va = reg_va(m.group(1)); ps = [p.strip() for p in m.group(2).split(",") if p.strip() and p.strip() != "void"]
        if va: ext[m.group(1) + "_r"] = (va, [p.split("reg_")[1].strip() for p in ps if "reg_" in p], sum(1 for p in ps if "reg_" not in p))
    if not ext: return t, 0
    glob = {}
    for m in re.finditer(r"^/\*([0-9a-f]{8})\*/ extern (?:undefined4|int|uint|float|[\w ]+ \*|\w+ \*) (\w+);", t, re.M): glob[m.group(1)] = m.group(2)
    names = {v[0]: n for n, v in ext.items()}
    calls = {}
    for m in re.finditer(r"(?<![\w.>])(\w+)\(", t):
        n = m.group(1)
        if n not in ext or n == nm: continue
        ls = t.rfind("\n", 0, m.start()) + 1
        if t[ls:m.start()].lstrip().startswith(("extern", "/*", "*", "static", "__declspec")) : continue
        if t[ls:m.start()].strip() == "" and t.startswith("{", m.start()) : continue
        al, close = _args(t, m.end() - 1)
        if close < 0: continue
        args = [t[s:e].strip() for s, e in al if t[s:e].strip()]
        calls.setdefault(n, []).append((m.start(), close, args))
    want = {n: len(ext[n][1]) + ext[n][2] for n in calls}
    grow = set()      # the extern itself has fewer parameters than the original reads AND every caller pushes (orig_stackargs slots, call_pushmin): the extern grows
    for n, cs in calls.items():
        k = pushmin_slots(ext[n][0])
        if k and not ext[n][1] and 0 < ext[n][2] < k and re.search(r"^/\*%s\*/ extern [^;\n]*\b%s\([^.)]+\);" % (ext[n][0], re.escape(n)), t, re.M) and all(len(a) == ext[n][2] for _, _, a in cs): want[n] = k; grow.add(n)
    want_t = set(); orig_const_args(nm, set())          # fills _FN / _STD
    ow = orig().get("%08x" % _FN[nm][0], (0, [], [])) if nm in _FN else (0, [], [])
    tails = {tv: int(f[1:]) for tv in ow[1] for f in ",".join(ow[2]).split(",") if re.fullmatch(r"w\d+", f)}      # tail-jump targets and the number of frame arguments the original rewrites before the `jmp`
    for n in calls:
        if ext[n][0] in tails and not ext[n][1] and ext[n][2] == 0 and re.search(r"^/\*%s\*/ extern [^;\n]*\b%s\(\);" % (ext[n][0], re.escape(n)), t, re.M): want[n] = tails[ext[n][0]]; want_t.add(n)      # `f();` K&R extern: the call is the tail jump (arguments = the rewritten frame slots)
    for n, cs in calls.items():      # `f()` K&R extern called with fewer arguments than the original reads / pushes: the calls get them, the extern stays unprototyped
        k = pushmin_slots(ext[n][0])
        if k and not ext[n][1] and ext[n][2] == 0 and n not in want_t and re.search(r"^/\*%s\*/ extern [^;\n]*\b%s\(\);" % (ext[n][0], re.escape(n)), t, re.M) and len({len(a) for _, _, a in cs}) == 1 and len(cs[0][2]) < k: want[n] = k
    need = {n for n, cs in calls.items() if any(len(a) < want[n] for _, _, a in cs)}
    if not need: return t, 0
    sites = asm_sites(nm, {ext[n][0] for n in calls})
    if not sites: return t, 0
    def canon_of(n):
        va, rg, ns = ext[n]; tot = want[n]; ns = tot - len(rg); cs = calls[n]; ss = sites.get(va, [])
        if len(ss) != len(cs) or ns > 8: return None
        return [[("?" if s.get("al") and "[S" in (s["regs"].get(r) or "") else (s["regs"].get(r) or "R:" + r)) for r in rg] + [("?" if s.get("al") and "[S" in x else x) for x in s["stack"][:ns]] for s in ss]
    def retvar(c, p):      # "ret:<callee va>": the variable the call just before assigned (`v = P(..); f()`)
        pn = names.get(c[4:])
        if not pn: return None
        pre = t[max(0, p - 700):p]; mm = list(re.finditer(r"([\w.]+)\s*=\s*(?:\([\w \*]+\)\s*)?%s\(" % re.escape(pn), pre))
        return mm[-1].group(1) if mm and not re.search(r"\b%s\s*=[^=]" % re.escape(mm[-1].group(1)), pre[mm[-1].end():]) else None
    pool = []      # (position, canonical asm value, C text) of the arguments Ghidra did print, at every call of every callee: the same asm value elsewhere in the function has the same C expression
    for n2 in calls:
        c2 = canon_of(n2)
        for i, (pp, _, a2) in enumerate(calls[n2] if c2 else []):
            for j, a in enumerate(a2[:len(c2[i])]):
                if not c2[i][j].startswith(("R:", "?", "I:")) and not re.search(r"[\w\]]\s*\(|[^=!<>]=[^=]|\+\+|--", a): pool.append((pp, c2[i][j], a))
    edits = []; done = 0; xmax = 0
    for n in need:
        va, rg, ns = ext[n]; tot = want[n]; ns = tot - len(rg); cs = calls[n]; ss = sites.get(va, []); e0 = len(edits)
        canon = canon_of(n)
        if canon is None: continue
        if any(len(a) >= tot and re.fullmatch(r"0x[0-9a-f]+|\d+", a[j]) and canon[i2][j].startswith("I:") and int(a[j], 0) != int(canon[i2][j][2:], 0) for i2, (_, _, a) in enumerate(cs) for j in range(tot)): continue      # the order of the C calls differs from the original's
        for i, (p, close, args) in enumerate(cs):
            if len(args) >= tot: continue
            new = []; _XP.update(tail=bool(ss[i].get("tail")), own=ow[0], max=0, ret=lambda c, p=p: retvar(c, p))
            for j in range(len(args), tot):
                c = canon[i][j]; x = _cname(c, t, glob, names, nm)
                if x is None:      # the argument another call of f passes for the same asm value
                    cands = sorted((abs(pp - p), a) for pp, cc, a in pool if cc == c and pp != p)
                    if cands: x = cands[0][1]
                if x is None: break
                new.append(x)
            else:
                edits.append((close, close, "".join((", " if args or q else "") + x for q, x in enumerate(new)))); done += 1; xmax = max(xmax, _XP["max"])
        if n in grow:
            if done and len([e for e in edits[e0:] if e[0] == e[1]]) == len(cs):
                m = re.search(r"^/\*%s\*/ extern [^;\n]*\b%s\(([^)]*)\);" % (ext[n][0], re.escape(n)), t, re.M); edits.append((m.end(1), m.end(1), "".join(", undefined4 p%d" % j for j in range(ext[n][2], tot))))
            else: del edits[e0:]
    if xmax:      # definition head: parameters up to unused<xmax>
        h = re.search(r"^[A-Za-z_][^;{}]*?\b%s\s*\(([^)]*)\)\s*\{" % re.escape(nm), t, re.M)
        if not h: return t, 0
        ps = [p_ for p_ in h.group(1).split(",") if p_.strip() and p_.strip() != "void"]
        edits.append((h.start(1), h.end(1), ",".join(ps + [" undefined4 unused%d" % q for q in range(len(ps) + 1, xmax + 1)]).lstrip()))
    for s, e, x in sorted(edits, reverse=True): t = t[:s] + x + t[e:]
    return t, done

# class 9 frame merge
FES = {"undefined":1,"undefined1":1,"undefined2":2,"undefined4":4,"undefined8":8,"char":1,"uchar":1,"byte":1,"short":2,"ushort":2,"int":4,"uint":4,"long":4,"ulong":4,"float":4,"RwReal":4,"double":8,"bool":1,"BOOL":4,"code":4,
      "RwV3d":12,"RwV2d":8,"RwRGBA":4,"RwMatrix":64,"RwSphere":16,"RwBBox":24,"RwLine":24,"RwPlane":16}
DECL = re.compile(r"^  ((?:(?:unsigned|signed|const)\s+)*(\w+)\s*(\*+)?\s*((?:local|[a-z]*Stack)_([0-9a-fA-F]+))\s*(?:\[\s*(\d+|0x[0-9a-fA-F]+)\s*\])?)\s*;[ \t]*\n", re.M)
def merge_fn(body, flagged, maxgap=8):
    """-> new body or None. flagged: slot names of defectlint class 9 findings"""
    sl = []
    for m in DECL.finditer(body):
        ty, ptr, nm, off, n = m.group(2), m.group(3), m.group(4), int(m.group(5), 16), m.group(6)
        sz = 4 if ptr else FES.get(ty)
        if sz is None: return None
        al = min(sz, 8) if not n else min(sz, 4)
        if ptr: al = 4
        if n: sz *= int(n, 0)
        if ty in ("RwV3d", "RwV2d", "RwMatrix", "RwRGBA", "RwSphere", "RwBBox", "RwLine", "RwPlane"): al = 4
        sl.append(dict(nm=nm, off=off, sz=sz, al=al, txt=m.group(1), span=m.span()))
    sl.sort(key=lambda s: -s["off"])
    cl = []
    for s in sl:
        if cl and cl[-1][-1]["off"] - cl[-1][-1]["sz"] - s["off"] < 0: return None      # overlapping slots: not a plain layout
        if cl and cl[-1][-1]["off"] - cl[-1][-1]["sz"] - s["off"] <= maxgap: cl[-1].append(s)
        else: cl.append([s])
    # address-taken slots count too: `&local_N`, an array / struct slot named bare in an argument list, `p = local_N;` (a callee may read past the object we declared)
    flagged = set(flagged) | {s["nm"] for s in sl if re.search(r"&\s*%s\b|[(,=]\s*%s\s*[,);]" % (s["nm"], s["nm"]), body)}
    todo = [c for c in cl if len(c) > 1 and any(s["nm"] in flagged for s in c)]
    if not todo: return None
    edits = []; names = {}
    for k, c in enumerate(todo):
        v = ("frame" if not re.search(r"\bframe\b", body) else "frame_obj") + ("" if k == 0 else str(k + 1)); top = c[0]["off"]; mem = []
        for i, s in enumerate(c):
            if (top - s["off"]) % s["al"]: return None
            if i: 
                g = c[i - 1]["off"] - c[i - 1]["sz"] - s["off"]
                if g: mem.append("undefined1 pad_%x[%d];" % (c[i - 1]["off"] - c[i - 1]["sz"], g))
            mem.append(s["txt"] + ";"); names[s["nm"]] = v
        edits.append((c[0]["span"][0], c[0]["span"][1], "  struct {\n" + "".join("    %s\n" % x for x in mem) + "  } %s;\n" % v))
        for s in c[1:]: edits.append((s["span"][0], s["span"][1], ""))
    for a, b, n in sorted(edits, reverse=True): body = body[:a] + n + body[b:]
    return sub_names(body, names)
def sub_names(body, names):
    # do not rewrite the member declarations inside the struct blocks
    out = []; pos = 0
    for m in re.finditer(r"  struct \{\n.*?\n  \} frame(?:_obj)?\d*;\n", body, re.S):
        out.append(re.sub(r"(?<![\w.])(%s)\b" % "|".join(map(re.escape, names)), lambda q: "%s.%s" % (names[q.group(1)], q.group(1)), body[pos:m.start()])); out.append(m.group(0)); pos = m.end()
    out.append(re.sub(r"(?<![\w.])(%s)\b" % "|".join(map(re.escape, names)), lambda q: "%s.%s" % (names[q.group(1)], q.group(1)), body[pos:]))
    return "".join(out)
def frame_fix(t, flagged):
    n = 0
    def f(m):
        nonlocal n
        r = merge_fn(m.group(0), flagged)
        if r: n += 1
        return r or m.group(0)
    t2 = re.sub(r"^\{\n.*?^\}\n", f, t, flags=re.M | re.S)
    return t2, n
def fix(path):
    """edits that touch the same line go in successive rounds (lint again after each)"""
    ks = []
    t = open(path, errors="replace").read()      # Ghidra wraps a long definition head (`undefined4 * __cdecl\nname(a,`): diffemu / core.defs need the name on the type's line
    t2 = re.sub(r"^((?!extern|static)[A-Za-z_][\w \*]*(?:__cdecl|__stdcall|__fastcall))[ \t]*\n(\w+\()", r"\1 \2", t, flags=re.M)
    if t2 != t: open(path, "w").write(t2); ks.append("headjoin")
    t3 = isnan(re.sub(r"\.s\.(LowPart|HighPart)\b", r".\1", t2))      # kind `member`: Ghidra's LARGE_INTEGER has `.s.LowPart` (newer SDK), our header has the flat union-less struct
    if t3 != t2: open(path, "w").write(t3); ks.append("member/isnan")
    t4 = re.sub(r"^/\*[0-9a-f]{8}\*/ extern [^;\n]*?\b(\w+)\([^;\n]*\);\n", lambda m: "" if len(re.findall(r"\b%s\b" % re.escape(m.group(1)), re.sub(r"/\*.*?\*/", "", t3, flags=re.S))) == 1 else m.group(0), t3, flags=re.M)      # kind `unused`: an extern nobody calls (autocompile declares names that only occur in the comment; defectlint then flags `start`)
    if t4 != t3: open(path, "w").write(t4); ks.append("unused")
    for _ in range(3):
        k = _fix1(path)
        if not k: break
        ks += k
    return ks
def _fix1(path):
    import defectlint
    t = open(path, errors="replace").read(); defectlint.load_gt()
    t, tn = tailfwd(t, os.path.basename(path)[:-2])
    t, an = argsite(t, os.path.basename(path)[:-2])
    if tn or an: open(path, "w").write(t)
    if not hasattr(fix, "gd"): fix.gd = defectlint.load_decls()
    L = defectlint.Lint(os.path.relpath(path, G), t, fix.gd); fnd = L.run()
    kinds = ["tailfwd"] * tn + ["argsite"] * an; ctr = [len(re.findall(r'\bfloat fbits\d+;', t))]; ln = t.split("\n"); off = [0]
    for l in ln: off.append(off[-1] + len(l) + 1)
    edits = []      # (start, end, new)
    for k, line, sym, args in L.hints:
        if k == "fbits":
            m = re.compile(r"\b%s\s*\(" % re.escape(sym)).search(t, off[line - 1])
            if not m: continue
            al, _ = _args(t, m.end() - 1)
            for s, e in al:
                a = t[s:e]
                if a.strip() in args and not any(e[0] <= s + len(a) and e[1] >= s for e in edits if e[0] != e[1]) and not a.strip().startswith(("FBITS(", "(fbits", "*(int *)&")) : edits.append((s + len(a) - len(a.lstrip()), e - (len(a) - len(a.rstrip())), fbits_expr(t, s, a.strip(), edits, ctr))); kinds.append("fbits")
        elif k == "fcast":      # (float)piVar[-0x1a] of a dword element converts (fild); the original copies the bits
            a, b = off[line - 1], off[line]; seg = t[a:b]; n = seg.replace(sym, "*(float *)&" + args[0])
            if n != seg and not any(e[0] == a for e in edits): edits.append((a, b, n)); kinds.append("fcast")
        elif k == "fstore":     # dword object = (int)(float expr): the original stores the float bits
            a, b = off[line - 1], off[line]; seg = t[a:b]
            m = re.match(r"^(\s*)(.+?) = \(int\)(.*);\n?$", seg)
            if m and not m.group(2).startswith("*(float") and not any(e[0] == a for e in edits): edits.append((a, b, "%s*(float *)&%s = %s;\n" % (m.group(1), m.group(2), m.group(3)))); kinds.append("fstore")
        elif k == "dropextra":      # f(a, b, stale1, stale2) where f takes n parameters (orig_stackargs reads n, no flags): Ghidra keeps the dead pushes as arguments
            m = re.compile(r"\b%s\s*\(" % re.escape(sym)).search(t, off[line - 1])
            if not m: continue
            al, close = _args(t, m.end() - 1); n = args[0]
            if len(al) > n and orig_slots(ext_va(t, sym)) == n and all(re.fullmatch(r"\s*[\w.]+\s*", t[s:e]) for s, e in al[n:]):
                edits.append((al[n - 1][1], close, "")); kinds.append("dropextra")
            elif len(al) > n and "addr" in (orig().get(ext_va(t, sym) or "", (0, [], []))[2]) and orig()[ext_va(t, sym)][0] == n:      # the original takes the address of its argument area: a variadic function (RxLockedPipeAddFragment(pipe, idx, def, ..., NULL))
                d = re.search(r"^/\*%s\*/ extern [^;\n]*\b%s\(([^)]*)\);" % (ext_va(t, sym), re.escape(sym)), t, re.M)
                if d and "..." not in d.group(1) and not any(e[0] == d.end(1) for e in edits): edits.append((d.end(1), d.end(1), ", ...")); kinds.append("varargs")
        elif k == "sgn":
            u = re.escape(sym); a, b = off[line - 1], off[line]
            seg = t[a:b]
            n = re.sub(r"(?<![\w)\]])(%s)(?=\s*(?:<=|>=|<|>)(?![<>]))" % u, r"(int)\1", seg)
            n = re.sub(r"((?<![<>])(?:<=|>=|<|>)(?![<>])\s*)(%s)(?![\w\[.(])" % u, r"\1(int)\2", n)
            if n != seg and not any(e[0] == a for e in edits): edits.append((a, b, n)); kinds.append("sgn")
    nm = os.path.basename(path)[:-2]; ext = {}
    for m in re.finditer(r"^/\*([0-9a-f]{8})\*/ extern [^;]*?\b(\w+)\(([^)]*)\);", t, re.M): ext[m.group(2)] = (m.group(1), m.group(3).strip())
    cands = {}
    for m in re.finditer(r"(?<![\w.>])(\w+)\(\s*\)", t):
        ls = t.rfind("\n", 0, m.start()) + 1
        if m.group(1) in ext and m.group(1) != nm and not t[ls:m.start()].lstrip().startswith(("extern", "/*", "*")): cands.setdefault(m.group(1), []).append(m)
    if cands:
        ca = orig_const_args(nm, {ext[c][0] for c in cands})
        for c, ms in cands.items():
            a = ca.get(ext[c][0])
            if a:
                for m in ms: edits.append((m.start(), m.end(), "%s(%s)" % (c, ", ".join(str(int(x, 16)) if False else x for x in a)))); kinds.append("argfill")
    for s, e, n in sorted(edits, reverse=True): t = t[:s] + n + t[e:]
    defs = []
    body = re.sub(r"#define .*", "", t)
    for nme, v in enums().items():
        if re.search(r"\b%s\b" % nme, body) and not re.search(r"#define\s+%s\b|\b%s\s*=" % (nme, nme), t): defs.append("#define %s (%s)\n" % (nme, v)); kinds.append("enum")
    if defs:
        m = list(re.finditer(r'^#include .*\n', t, re.M))
        p = m[-1].end() if m else 0
        t = t[:p] + "".join(defs) + t[p:]
    t, nf = frame_fix(t, {a["symbol"] for a in fnd if a["cls"] == 9}); kinds += ["frame"] * nf
    if kinds: open(path, "w").write(t)
    return kinds
if __name__ == "__main__":
    for f in sys.argv[1:]: print(f, fix(f))
