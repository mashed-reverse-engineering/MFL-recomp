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
  fret  callee that the ORIGINAL call sites read from ST0 (fstp / fmul after the call) but the text declares int / undefined / void / not at all: extern float (diffemu core.float_returners).
  scalararr  `p = &g_x;` of a scalar extern followed by p[k] / p + k / p++: the extern becomes an array (the optimiser dropped the fill loops of scalar globals).
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
        sites[va].append(tuple(ps) if len(ps) == k else None)      # ps walks backwards from the call: the LAST push is arg0, i.e. already in argument order (was ps[::-1]: asymmetric constants (0, 0, &str) came out reversed; oil_load_assets crashed)
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
            if d > 0 and len(al2) >= d + k:      # d == 0: a K&R `()` callee, its arguments are real, not stale (load_ghost_model: (i, clump, slot) were moved into clump_load_by_name's out-pointer parameters)
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
def _CHK():
    import cmpfn
    return set(cmpfn._symtab().get("__chkstk", ())) | set(cmpfn._symtab().get("_chkstk", ()))
def _FTOLV():
    import cmpfn
    return set(cmpfn._symtab().get("_ftol", ())) | set(cmpfn._symtab().get("__ftol", ()))
_FRVS = []
def _FRV():
    """addresses (hex) of callees the original reads ST0 from (diffemu core.float_returners); empty when core is not importable"""
    if not _FRVS:
        try: import core; _FRVS.append({"%08x" % k for k in core.float_returners()})
        except Exception: _FRVS.append(set())
    return _FRVS[0]
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
        late = {}; snaps = {}; fall = True; fp = []; fsn = {}      # fp: symbolic x87 stack (tools/t2_x87.py), None = unknown
        mem, regs, d, out, prev, ok, dmap, cont = {}, {}, 0, {}, None, True, {}, True      # mem: entry relative stack slot -> canonical value; dmap: esp delta at jump targets
        dbp = [0, False]      # esp delta at `mov ebp, esp`, stack aligned (`and esp, -16`: entry relative slots are meaningless)
        def slot(o): return mem.get(o) or ("M:[S%d]" % o if o > 0 else "?")
        for a in order:
            op, ar = ins[a]
            if a in tg:      # control flow join: registers all incoming forward edges agree on (a loop head, target of a backward jump, keeps nothing)
                cand = snaps.get(a, []) + ([regs] if fall else [])
                regs = {} if a in back or not cand else {r: v for r, v in cand[0].items() if v is not None and all(c.get(r) == v for c in cand)}
                if a not in back and cand and fall and snaps.get(a):      # `if (!p) p = q;` (test; jne L; mov r,[slot]; L:): Ghidra assigns the parameter variable, so the join value is the value of the jump edge (a parameter slot) when every incoming value is a stack parameter
                    for r, v in snaps[a][0].items():
                        if r not in regs and v and v.startswith("M:[S") and all((c.get(r) or "").startswith("M:[S") for c in cand): regs[r] = v
                fc = fsn.get(a, []) + ([fp] if fall else [])
                if a in back: fp = fp if fp == [] else None
                elif not fc or any(x is None or len(x) != len(fc[0]) for x in fc): fp = None
                else:      # same depth: positions that differ between the incoming paths become an opaque value (a clamp `if (x > c) x = c` leaves the stack below untouched)
                    import t2_x87 as X
                    fp = [fc[0][i] if all(x[i] == fc[0][i] for x in fc) else X.mk(("u", 0)) for i in range(len(fc[0]))]
            fall = True
            for r_, (wa, wc) in single.items():
                if a > wa and r_ not in regs: regs[r_] = wc
            if a in dmap: d = dmap[a]
            ops = split_args(ar)
            if op.startswith("j") and ops and re.fullmatch(r"0x[0-9a-f]+", ops[0]):
                t_ = int(ops[0], 16); dmap.setdefault(t_, d); snaps.setdefault(t_, []).append(dict(regs)); fsn.setdefault(t_, []).append(None if fp is None else list(fp))
                if op == "jmp": fall = False
                if op == "jmp" and not (lo <= t_ < lo + sz) and t_ in vas_i: out.setdefault("%08x" % t_, []).append(dict(addr=a, stack=[slot(d + 4 + 4 * j) for j in range(8)], regs=dict(regs), prev=prev, tail=True, al=dbp[1]))
            if op[0] == "f" and op != "fnstsw":
                import t2_x87 as X
                fp = X.step(op, ops, fp, regs, d, mem, lambda o_, d_=d: _canon(o_, regs, d_), None)
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
                if va in vas: out.setdefault(va, []).append(dict(addr=a, stack=[slot(d + 4 * j) for j in range(8)], regs=dict(regs), prev=prev, al=dbp[1], fp=None if fp is None else list(fp)))
                k = _STD.get(va, 0) if va else 0
                d += k * 4
                if va and int(va, 16) in _CHK() and (regs.get("eax") or "").startswith("I:"): d -= int(regs["eax"][2:], 16)      # __chkstk(eax = frame size) allocates the frame: esp relative slots shift (vehicle_*_collect_triangles, 0x1080 bytes)
                regs = {r: v for r, v in regs.items() if r not in ("eax", "ecx", "edx")}
                if va: regs["eax"] = "ret:" + va
                if va and int(va, 16) in _FTOLV():      # __ftol: pops ST0 (a tree), eax = (int) of it; other float returners make the stack unknown
                    if fp: regs["eax"] = "T:%d" % fp[-1]; fp = fp[:-1]
                    else: fp = None
                elif va and fp is not None and va in _FRV():
                    import t2_x87 as X
                    fp = fp + [X.mk(("u", va))]      # a float result of an unknown call: renders to nothing, but `fstp st(0)` after it is fine
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
def stale_locals(t, nm):
    """locals of function nm (plain declarations `type name;` at the top) that are never assigned, never address-taken, never indexed / member-accessed: only read (passed on)"""
    h = re.search(r"^[A-Za-z_][^;{}]*?\b%s\s*\([^)]*\)\s*\{" % re.escape(nm), t, re.M)
    if not h: return set()
    body = t[h.end():]; out = set()
    for m in re.finditer(r"^  (?:undefined\d?|int|uint|float|RwReal|char|uchar|short|ushort|byte|bool) (\w+);", body, re.M):
        v = m.group(1); rest = body[:m.start()] + " " * (m.end() - m.start()) + body[m.end():]
        if not re.search(r"\b%s\b" % v, rest): continue
        if re.search(r"(?<![\w.])%s\s*(?:[-+*/|&^]|<<|>>)?=(?!=)|&\s*%s\b|\b%s\s*(?:\[|\.|->|\+\+|--)|(?:\+\+|--)\s*%s\b" % (v, v, v, v), rest): continue
        out.add(v)
    return out
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
        if ext[n][0] in tails and not ext[n][1] and ext[n][2] == 0 and re.search(r"^/\*%s\*/ extern [^;\n]*\b%s\((?:void)?\);" % (ext[n][0], re.escape(n)), t, re.M): want[n] = max(tails[ext[n][0]], (orig().get(ext[n][0]) or (0,))[0]); want_t.add(n)      # `f();` K&R extern: the call is the tail jump (arguments = the rewritten frame slots)
    for n, cs in calls.items():      # `f()` K&R extern called with fewer arguments than the original reads / pushes: the calls get them, the extern stays unprototyped
        k = pushmin_slots(ext[n][0])
        if k and not ext[n][1] and ext[n][2] == 0 and n not in want_t and re.search(r"^/\*%s\*/ extern [^;\n]*\b%s\((?:void)?\);" % (ext[n][0], re.escape(n)), t, re.M) and max(len(a) for _, _, a in cs) <= k and min(len(a) for _, _, a in cs) < k: want[n] = k      # mixed argument counts: the complete calls stay, the short ones get the rest
    need = {n for n, cs in calls.items() if any(len(a) < want[n] for _, _, a in cs)}
    uninit = stale_locals(t, nm)      # kind `stalearg`: a local nobody assigns passed as an argument (Ghidra lost the definition: a parameter of the caller read after a `mov edx, [esp+..]`): the argument comes from the original call site
    stale = {n for n, cs in calls.items() if uninit and any(a_ in uninit for _, _, a in cs for a_ in a)}
    need |= stale
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
            if len(args) >= tot and n in stale:
                al_, _c = _args(t, p + len(n)); _XP.update(tail=bool(ss[i].get("tail")), own=ow[0], max=0, ret=lambda c, p=p: retvar(c, p))
                for j, a in enumerate(args[:tot]):
                    if a in uninit and j < len(canon[i]) and not canon[i][j].startswith(("R:", "?")):
                        x = _cname(canon[i][j], t, glob, names, nm)
                        if x and x != a: edits.append((al_[j][0] + len(t[al_[j][0]:al_[j][1]]) - len(t[al_[j][0]:al_[j][1]].lstrip()), al_[j][1] - (len(t[al_[j][0]:al_[j][1]]) - len(t[al_[j][0]:al_[j][1]].rstrip())), x)); done += 1
                continue
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
        if n not in grow and len(edits) > e0:      # a `(void)` extern that now gets arguments stays unprototyped `()` (too many actual parameters otherwise)
            mv = re.search(r"^/\*%s\*/ extern [^;\n]*\b%s\((void)\);" % (ext[n][0], re.escape(n)), t, re.M)
            if mv: edits.append((mv.start(1), mv.end(1), ""))
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
_STALE = {}
def stale_regs():
    """va -> parameter count of sigs.txt functions that carry a `reg_e??` register parameter (Ghidra project signature made from the first, over-inclusive reginputs) although output/regsigs.csv
    (the strict register inputs) has no row: the register parameter does not exist, its argument is a stale value (voice_set_volume(reg_eax, idx, vol), 17 DIFF functions)"""
    if not _STALE:
        import csv
        reg = {r["addr"] for r in csv.DictReader(open(os.path.join(G, "output", "regsigs.csv")))}
        for l in open(os.path.join(G, "output", "sigs.txt")):
            q = l.rstrip("\n").split("|")
            if len(q) >= 7 and re.search(r"(?:^|;)[^;]*:reg_e\w\w:", q[5]) and q[1] not in reg: _STALE[q[1]] = (len([x for x in q[5].split(";") if x]), len([x for x in q[5].split(";") if x and ":reg_e" in x]))
    return _STALE
def stalereg(t):
    """kind `stalereg`: extern of a stale-register function loses its first (register) parameter, every call in the text loses its first argument (only calls that pass exactly the old count).  -> (text, n)"""
    n = 0; st = stale_regs()
    for m in list(re.finditer(r"^/\*([0-9a-f]{8})\*/ extern ([^;\n]*?)\b(\w+)\(([^;\n]*)\);\n", t, re.M)):
        va, nm = m.group(1), m.group(3)
        if va not in st: continue
        ps = [x.strip() for x in m.group(4).split(",")] if m.group(4).strip() not in ("", "void") else []
        tot, nreg = st[va]; k0 = len(ps) - (tot - nreg)      # stale leading parameters still in the text (all nreg, or fewer when the export already lost some: draw_sprite_uv 13 of 14, 12 stack)
        if not ps or not 0 < k0 <= nreg: continue      # K&R `()` or already corrected
        new_ext = "/*%s*/ extern %s%s(%s);\n" % (va, m.group(2), nm, ", ".join(ps[k0:]) or "void")
        t = t.replace(m.group(0), new_ext, 1); k = 0; pos = 0
        while True:
            c = re.compile(r"(?<![\w.>])%s\s*\(" % re.escape(nm)).search(t, pos)
            if not c: break
            pos = c.end(); ls = t.rfind("\n", 0, c.start()) + 1
            if t[ls:c.start()].lstrip().startswith(("extern", "/*", "*")) or re.match(r"\s*(?:\w+ )+\**$", t[ls:c.start()]) and t[ls:c.start()].strip() and not t[ls:c.start()].strip().startswith(("return", "else")): continue      # a definition head
            al, close = _args(t, c.end() - 1)
            if len(al) == len(ps) and t[al[0][0]:al[0][1]].strip():
                t = t[:al[0][0]] + (t[al[k0][0]:] if len(al) > k0 else t[al[-1][1]:]); k += 1; pos = c.start() + 1
        n += 1
    return t, n
PRIM = r"(?:undefined[124]?|int|uint|short|ushort|char|uchar|byte|word|dword|float|bool)"
def scalararr(t):
    """kind `scalararr` (REAL, not a harness limit): `p = &g_x;` with `extern undefined4 g_x;` (a SCALAR) followed by `p[k]` / `p + k` / `*p++` walks over an array: the compiler believes the loop writes only the scalar and drops it
    when g_x is stored afterwards (scene_tables_clear: the 0x28 dword fill of g_scene_tables_651198 vanished; 17+ memory-write / call-trace DIFFs take `&g_` pointers).  The extern becomes `T g_x[0x1000]`,
    `&g_x` -> `g_x`, other uses `g_x` -> `g_x[0]`.  -> (text, number of globals converted)"""
    n = 0
    for m in list(re.finditer(r"\b(\w+) = &(g_\w+);", t)):
        pv, g = m.group(1), m.group(2)
        d = re.search(r"^(/\*[0-9a-f]{8}\*/ extern %s) %s;\n" % (PRIM, re.escape(g)), t, re.M)
        if not d: continue
        rest = t[m.end():]
        if not re.search(r"\b%s\s*\[|\b%s\s*[-+]|\b%s\s*=\s*%s\s*[-+]|\*\s*%s\s*\+\+|\b%s\s*\+\+" % ((re.escape(pv),) * 6), rest): continue
        decl = d.group(1) + " %s[0x1000];\n" % g
        t = t.replace(d.group(0), "\0DECL\0", 1)
        t = re.sub(r"&\s*%s\b(?!\s*\[)" % re.escape(g), "\0ARR\0", t)
        t = re.sub(r"(?<![\w.])%s\b(?!\s*\[)" % re.escape(g), g + "[0]", t)
        t = t.replace("\0ARR\0", g).replace("\0DECL\0", decl, 1); n += 1
    return t, n
_SECT = []
def pushfwd(t):
    """kind `pushfwd` (REAL): `v = f(a, s1, s2, s3); g(v);` where f really takes len(declared) < len(args) stack slots and g reads 1 + 3 slots (output/orig_stackargs.txt): the compiler pushed the arguments of g
    BEFORE the call of f (push 2; push -1; push esi; push a; call f; add esp,4; push eax; call g; add esp,16); Ghidra attached them to f.  -> `g(f(a), s1, s2, s3)`-equivalent `v = f(a); g(v, s1, s2, s3);`, the extern of g gets the slots.  -> (text, n)"""
    n = 0
    for m in re.finditer(r"^([ \t]*)(\w+) = (\w+)\(([^;\n]*)\);[ \t]*\n[ \t]*(\w+)\(\2\);", t, re.M):
        f, g = m.group(3), m.group(5)
        fe = re.search(r"^/\*([0-9a-f]{8})\*/ extern [^;\n]*?\b%s\(([^)]*)\);" % re.escape(f), t, re.M)
        ge = re.search(r"^(/\*([0-9a-f]{8})\*/ extern [^;\n]*?\b%s\()([^)]*)(\);)" % re.escape(g), t, re.M)
        if not fe or not ge: continue
        fk = fe.group(2).strip(); fn = 0 if fk in ("", "void") else fk.count(",") + 1
        al = split_args(m.group(4)); gs = orig_slots(ge.group(2)); gk = ge.group(3).strip(); gn = 0 if gk in ("", "void") else gk.count(",") + 1
        if gn != 1 or not gs or fn < 1 or orig_slots(fe.group(1)) != fn or len(al) - fn != gs - 1 or not all(re.fullmatch(r"\s*[\w.\-]+\s*", a) for a in al[fn:]): continue
        t = t[:m.start()] + "%s%s = %s(%s);\n%s%s(%s);" % (m.group(1), m.group(2), f, ", ".join(al[:fn]), m.group(1), g, ", ".join([m.group(2)] + [a.strip() for a in al[fn:]])) + t[m.end():]
        return pushfwd(t.replace(ge.group(0), ge.group(1) + ", ".join("undefined4 p%d" % i for i in range(gs)) + ge.group(4), 1))[0], 1 + n
    return t, n
_RI = {}
def fastcall(t, nm):
    """kind `fastcall` (REAL): Ghidra typed a stack-argument cdecl function `__fastcall f(undefined4 param_1, ...)` (a `push ecx` slot read as ECX): output/reginputs.csv has no register input and orig_stackargs
    reads exactly (parameters - 1) slots.  The first parameter goes, `__fastcall` becomes `__cdecl`, its only use `g(param_1)` becomes `g()`.  -> (text, n)"""
    if not _RI:
        import csv
        for r in csv.DictReader(open(os.path.join(G, "output", "reginputs.csv"))): _RI[r["name"]] = r["reg_inputs"]
    m = re.search(r"^(\w[\w \*]*?)__fastcall (%s)\(undefined4 param_1(?:,([^)]*))?\)" % re.escape(nm), t, re.M)
    if not m or _RI.get(nm, "x") != "": return t, 0
    orig_const_args(nm, set())
    if nm not in _FN or orig_slots("%08x" % _FN[nm][0]) != (len(m.group(3).split(",")) if m.group(3) else 0) or len(re.findall(r"\bparam_1\b", t)) != 2: return t, 0
    t = t[:m.start()] + "%s__cdecl %s(%s)" % (m.group(1), nm, (m.group(3) or "void").strip()) + t[m.end():]
    return re.sub(r"\((param_1)\)", "()", t, count=1), 1
def istore(t):
    """kind `istore` (REAL): `p[2] = (RwReal)(<integer bit expression>);` converts the integer to float; the original stores the bits (packed colour) -> `*(uint *)&p[2] = <expr>;`"""
    n = 0
    def r(m):
        nonlocal n
        e = m.group(3)
        if "float10" in e or not re.search(r"<<|>>|&", e): return m.group(0)
        n += 1; return "%s*(uint *)&%s = %s;" % (m.group(1), m.group(2), e)
    return re.sub(r"^([ \t]*)([\w\[\]\.\->\*]+) = \(RwReal\)\(((?:[^;]|\n)*?)\);", r, t, flags=re.M), n
def _rodata(va):
    """the bytes at va when it lies in a NON-writable section of MFL.exe (float constants), else None"""
    import struct, cmpfn
    if not _SECT:
        d = open(cmpfn.EXE, "rb").read(0x1000); e = struct.unpack_from("<I", d, 0x3C)[0]; ns = struct.unpack_from("<H", d, e + 6)[0]; osz = struct.unpack_from("<H", d, e + 20)[0]
        base = struct.unpack_from("<I", d, e + 24 + 28)[0]
        for i in range(ns):
            o = e + 24 + osz + i * 40; vs, vaa, rs, rp = struct.unpack_from("<IIII", d, o + 8); ch = struct.unpack_from("<I", d, o + 36)[0]
            _SECT.append((base + vaa, base + vaa + max(vs, rs), ch))
    for lo, hi, ch in _SECT:
        if lo <= va < hi: return cmpfn.exe_bytes(va, 8) if not ch & 0x80000000 else None
    return None
def ftol(t, nm):
    """kind `ftol`: `_ftol()` (Ghidra lost the x87 expression feeding `call __ftol`) -> `((int)(expr))` with expr recovered from the ORIGINAL function by the symbolic x87 stack of asm_sites (tools/t2_x87.py);
    all or nothing per function (the n-th `_ftol()` of the text is the n-th call __ftol of the original).  -> (text, number of calls rewritten)"""
    t0 = t
    occ = [m for m in re.finditer(r"(?<![\w.>])_ftol2?\(\s*\)", t) if not t[t.rfind("\n", 0, m.start()) + 1:m.start()].lstrip().startswith(("extern", "/*"))]
    if not occ: return t, 0
    import cmpfn, struct, t2_x87 as X
    fv = _FTOLV()
    if not fv: return t, 0
    sites = asm_sites(nm, {"%08x" % v for v in fv})
    if not sites: return t, 0
    ss = [x for v in fv for x in sites.get("%08x" % v, [])]; ss.sort(key=lambda x: x["addr"])
    if len(ss) != len(occ) or any(not x.get("fp") for x in ss): return t, 0
    glob = {}
    for m in re.finditer(r"^/\*([0-9a-f]{8})\*/ extern [^;(]*?\b(\w+)\s*(?:\[[^\]]*\])?;", t, re.M): glob[m.group(1)] = m.group(2)
    rev = {}
    for n_, vs in cmpfn._symtab().items():
        for v in vs: rev.setdefault("%08x" % v, n_)
    add = {}
    def isflt(x): return add.get(x, "").startswith("/*") and " float " in add[x] or re.search(r"\b(?:float|RwReal)\s+(?:\w+\s*,\s*)*%s\b" % re.escape(x), t) is not None
    calls = {}      # float temp name -> (statement start, end, callee name, args): `name(args);` whose result the expression uses
    def leaf(c, kind):
        if c.startswith("U:"):
            nmc = glob.get(c[2:]) or rev.get(c[2:])
            if not nmc: return None
            ms = [m for m in re.finditer(r"^([ \t]*)%s(\([^;\n]*\));[ \t]*$" % re.escape(nmc), t, re.M)]
            if len(ms) != 1: return None
            v = "fc%d" % (len(calls) + 1); calls[v] = (ms[0].start(), ms[0].end(), ms[0].group(1), nmc, ms[0].group(2)); return v
        mm = re.fullmatch(r"M:ds:0x([0-9a-f]+)", c)
        if mm:
            va = int(mm.group(1), 16); ro = _rodata(va)
            if ro is not None and kind in ("f", "d"):
                v = struct.unpack("<f", ro[:4])[0] if kind == "f" else struct.unpack("<d", ro[:8])[0]
                return X.lit(v, kind)
            if mm.group(1).rjust(8, "0") not in glob:
                n_ = rev.get(mm.group(1).rjust(8, "0"))
                if not n_ or not re.fullmatch(r"[A-Za-z_]\w*", n_): return "(*(%s *)0x%s)" % ({"f": "float", "d": "double"}.get(kind, "int"), mm.group(1))
                glob[mm.group(1).rjust(8, "0")] = n_; add[n_] = "/*%s*/ extern %s %s;\n" % (mm.group(1).rjust(8, "0"), {"f": "float", "d": "double"}.get(kind, "int"), n_)
        _XP.update(tail=False, own=0, max=0, ret=None)
        x = _cname(c, t, glob, {}, nm)
        if x is None: return None
        if kind in ("f", "d"):
            ty = "float" if kind == "f" else "double"
            if re.fullmatch(r"[A-Za-z_]\w*(?:\.\w+)*", x): return x if isflt(x) and kind == "f" else "*(%s *)&%s" % (ty, x)
            if x.startswith("*(int *)"): return "*(%s *)%s" % (ty, x[len("*(int *)"):])
            if x.startswith("*(undefined4 *)"): return "*(%s *)%s" % (ty, x[len("*(undefined4 *)"):])
            return "*(%s *)&%s" % (ty, x)
        return "(short)%s" % x if kind == "i2" else x
    rep = []
    for m, x in zip(occ, ss):
        e = X.render(x["fp"][-1], leaf)
        if not e: return t, 0
        rep.append((m.start(), m.end(), "((int)%s)" % (e if e.startswith("(") else "(%s)" % e)))
    for v, (st, en, ind, nmc, ar) in calls.items(): rep.append((st, en, "%s%s = %s%s;" % (ind, v, nmc, ar)))
    for st, en, x in sorted(rep, reverse=True): t = t[:st] + x + t[en:]
    if calls:
        mb = re.search(r"^\{[ \t]*\n", t, re.M)
        if not mb: return t0, 0
        t = t[:mb.end()] + "".join("  float %s;\n" % v for v in calls) + t[mb.end():]
    if add:
        inc = list(re.finditer(r'^(#include .*|/\*[0-9a-f]{8}\*/ extern .*)\n', t, re.M)); p = inc[-1].end() if inc else 0
        t = t[:p] + "".join(add.values()) + t[p:]
    return t, len(rep)
def fret(t):
    """kind `fret`: a callee whose ORIGINAL call sites consume ST0 (core.float_returners: `call f; fstp [esp]`) but that the text declares int / undefined / void (or not at all: `func_0x004757e0(0)`)
    returns a float: the extern becomes `float` (else C converts EAX with fild and never pops ST0).  -> (text, number of changes)"""
    try: import core
    except Exception: return t, 0
    fr = {k: v for k, v in core.float_returners().items() if k not in core.real_helpers()}; n = 0
    def retype(m):
        nonlocal n
        if int(m.group(2), 16) in fr and m.group(3) in ("int", "uint", "undefined4", "undefined", "void", "long", "ulong", "dword"): n += 1; return "%sextern float %s" % (m.group(1), m.group(4))
        return m.group(0)
    t = re.sub(r"^(/\*([0-9a-f]{8})\*/ )extern (\w+) ((?:__\w+ )?\w+\()", lambda m: retype(m), t, flags=re.M)
    for m in sorted({m.group(1) for m in re.finditer(r"(?<![\w])(func_0x[0-9a-f]{8})\s*\(", t)}):
        if int(m[7:], 16) in fr and not re.search(r"extern [^;\n]*\b%s\(" % m, t):
            inc = list(re.finditer(r'^(#include .*|/\*[0-9a-f]{8}\*/ extern .*)\n', t, re.M)); p = inc[-1].end() if inc else 0
            t = t[:p] + "extern float %s();\n" % m + t[p:]; n += 1
    return t, n
def stmt_end(t, a):
    """end offset of the statement that starts at a: the first `;` / `{` outside parentheses (at most 900 characters)"""
    d = 0
    for i in range(a, min(len(t), a + 900)):
        c = t[i]
        if c == "(": d += 1
        elif c == ")": d -= 1
        elif c in ";{" and d <= 0: return i + 1
    return min(len(t), a + 900)
def frame_fix(t, flagged):
    n = 0
    def f(m):
        nonlocal n
        r = merge_fn(m.group(0), flagged)
        if r: n += 1
        return r or m.group(0)
    t2 = re.sub(r"^\{\n.*?^\}\n", f, t, flags=re.M | re.S)
    return t2, n
_DECL = {}
def addrarith(t):
    """kind `addrarith` (REAL): `(&g + i * 4)` / `(&g)[i]` of a scalar extern that autocompile widened (undefined4) while Ghidra's type (output/decls.h) is 1 byte: the pointer arithmetic scaled by the wrong element size -> `((undefined *)&g)`."""
    if not _DECL:
        for l in open(os.path.join(G, "output", "decls.h"), errors="replace"):
            m = re.match(r"/\*\w+\*/ extern ([\w ]+?)\s+(\w+)(\[.*?\])?;", l)
            if m and not m.group(3): _DECL[m.group(2)] = m.group(1)
    n = [0]
    def r(m):
        g = m.group(1); e = re.search(r"^/\*\w+\*/ extern ([\w ]+?)\s+%s;" % g, t, re.M)
        if e and _DECL.get(g) in ("undefined", "char", "byte", "uchar") and e.group(1) != _DECL[g]: n[0] += 1; return "((%s *)&%s)" % (_DECL[g], g)
        return m.group(0)
    return re.sub(r"&(\w+)(?=\s*\+|\)\[)", r, t), n[0]
def intvar(t):
    """kind `intvar` (REAL): Ghidra typed an INTEGER local (loop counter in ebx) float because one callee takes it as a float-ish argument: `v = (float)((int)v + 1)`, `*(int *)&v`, `v = 0.0;`.  A local / frame member that occurs as `(float)((int)v ..)` becomes `int`; the conversions around it go away."""
    n = 0
    for v in sorted(set(re.findall(r"\(float\)\(\(int\)([A-Za-z_][\w.]*)\b", t))):
        b = re.escape(v.split(".")[-1]); e = re.escape(v)
        d = re.findall(r"\b(?:float|RwReal)(\s+)%s(?=\s*;)" % b, t)
        if len(d) != 1 or re.search(r"\b(?:float|RwReal|double)\s+\**%s\s*\[" % b, t): continue
        t = re.sub(r"\b(?:float|RwReal)(\s+%s\s*;)" % b, r"int\1", t)
        t = re.sub(r"\(float\)(\((?:[^()]|\([^()]*\))*\(int\)%s\b(?:[^()]|\([^()]*\))*\))" % e, r"\1", t)
        t = re.sub(r"\*\(int \*\)&%s\b" % e, v, t); t = re.sub(r"\(int\)%s\b" % e, v, t)
        t = re.sub(r"\b%s = (-?\d+)\.0f?;" % e, r"%s = \1;" % v, t); n += 1
    return t, n
def fix(path):
    """edits that touch the same line go in successive rounds (lint again after each)"""
    ks = []
    t = open(path, errors="replace").read()      # Ghidra wraps a long definition head (`undefined4 * __cdecl\nname(a,`): diffemu / core.defs need the name on the type's line
    t2 = re.sub(r"^((?!extern|static)[A-Za-z_][\w \*]*(?:__cdecl|__stdcall|__fastcall))[ \t]*\n(\w+\()", r"\1 \2", t, flags=re.M)
    t2b = re.sub(r"^((?!extern|static|return|else|typedef)[A-Za-z_][\w \*]*)\n([A-Za-z_]\w*)\n[ \t]+\(", r"\1 \2(", t2, flags=re.M)      # `void __cdecl` / `name` / `    (params` (long signatures): defectlint.functions() skipped these functions entirely = no lint hint at all
    t2b = re.sub(r"^((?!extern|static|return|else|typedef)[A-Za-z_][\w \*]*[\w\*])[ \t]*\n[ \t]+(\([^;{}\n]*(?:\n[ \t]+[^;{}\n]*)?\))[ \t]*\n\s*\{", r"\1\2\n{", t2b, flags=re.M)      # `void name` / `    (int param_1,..)` / `{`: the 2-line wrapped head (defectlint.functions() needs the `(` on the line of the name)
    t2 = t2b
    if t2 != t: open(path, "w").write(t2); ks.append("headjoin")
    t3 = isnan(re.sub(r"\.s\.(LowPart|HighPart)\b", r".\1", t2))      # kind `member`: Ghidra's LARGE_INTEGER has `.s.LowPart` (newer SDK), our header has the flat union-less struct
    if t3 != t2: open(path, "w").write(t3); ks.append("member/isnan")
    t3, an_ = addrarith(t3)
    if an_: open(path, "w").write(t3); ks.append("addrarith")
    t3, iv_ = intvar(t3)
    if iv_: open(path, "w").write(t3); ks.append("intvar")
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
    t, fn = fret(t)
    t, xn = ftol(t, os.path.basename(path)[:-2]); fn += xn
    t, sn = stalereg(t)
    t, cn = scalararr(t)
    t, fcn = fastcall(t, os.path.basename(path)[:-2]); t, isn = istore(t); t, pfn = pushfwd(t); isn += pfn
    if tn or an or fn or sn or cn or fcn or isn: open(path, "w").write(t)
    if not hasattr(fix, "gd"): fix.gd = defectlint.load_decls()
    L = defectlint.Lint(os.path.relpath(path, G), t, fix.gd); fnd = L.run()
    kinds = ["tailfwd"] * tn + ["argsite"] * an + ["fret"] * fn + ["stalereg"] * sn + ["scalararr"] * cn + ["fastcall"] * fcn + ["istore"] * isn; ctr = [len(re.findall(r'\bfloat fbits\d+;', t))]; ln = t.split("\n"); off = [0]
    for l in ln: off.append(off[-1] + len(l) + 1)
    edits = []      # (start, end, new)
    for k, line, sym, args in L.hints:
        if k == "fbits":
            m = re.compile(r"\b%s\s*\(" % re.escape(sym)).search(t, off[line - 1])
            if not m: continue
            al, _ = _args(t, m.end() - 1)
            for s, e in al:
                a = t[s:e]
                if a.strip() in args and not re.search(r"\b(?:float|RwReal|double|undefined4|int)\s+%s\s*\[" % re.escape(a.strip().split(".")[-1]), t) and not any(e[0] <= s + len(a) and e[1] >= s for e in edits if e[0] != e[1]) and not a.strip().startswith(("FBITS(", "(fbits", "*(int *)&")) : edits.append((s + len(a) - len(a.lstrip()), e - (len(a) - len(a.rstrip())), fbits_expr(t, s, a.strip(), edits, ctr))); kinds.append("fbits")
        elif k == "fcast":      # (float)piVar[-0x1a] of a dword element converts (fild); the original copies the bits
            a = off[line - 1]; ix = t.find(sym, a, a + 600)      # the statement may span several lines (`if (a ||` / `(float)p[6] < k`)
            if ix >= 0 and not any(e[0] <= ix < e[1] for e in edits if e[1] > e[0]): edits.append((ix, ix + len(sym), "*(float *)&" + args[0])); kinds.append("fcast")
        elif k == "flit":      # g_float = 0xbf800000; converts the integer: the original stores the bit pattern = the float literal (hint from defectlint class 2)
            import t2_x87 as X, struct
            a, b = off[line - 1], off[line]; lb = re.match(r"[ \t]*LAB_\w+:\s*", t[a:]); a += lb.end() if lb else 0; b = max(b, stmt_end(t, a) + 1) if lb else b; seg = t[a:b]; v = int(args[0], 16)      # a statement behind a `LAB_x:` label starts on the label's line
            f = struct.unpack("<f", struct.pack("<I", v & 0xffffffff))[0]; lt = X.lit(f, "f")
            n = re.sub(r"=\s*%s\s*;" % re.escape(args[0]), "= %s;" % lt, seg) if lt else seg
            if n != seg and not any(e[0] == a for e in edits): edits.append((a, b, n)); kinds.append("flit")
        elif k == "fstore":     # dword object = (int)(float expr): the original stores the float bits
            a, b = off[line - 1], off[line]; lb = re.match(r"[ \t]*LAB_\w+:\s*", t[a:]); a += lb.end() if lb else 0; b = max(b, stmt_end(t, a) + 1) if lb else b; seg = t[a:b]
            m = re.match(r"^(\s*)(.+?) = (?:\(int\))?(.*);\n?$", seg)      # explicit `(int)` or the implicit conversion `param_1[10] = fVar2;`
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
            u = re.escape(sym); a = off[line - 1]; b = stmt_end(t, a)      # the compare may sit on a later line of a multi-line `if (` condition
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
