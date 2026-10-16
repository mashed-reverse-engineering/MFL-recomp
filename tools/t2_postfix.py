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
    for _ in range(3):
        k = _fix1(path)
        if not k: break
        ks += k
    return ks
def _fix1(path):
    import defectlint
    t = open(path, errors="replace").read(); defectlint.load_gt()
    t, tn = tailfwd(t, os.path.basename(path)[:-2])
    if tn: open(path, "w").write(t)
    if not hasattr(fix, "gd"): fix.gd = defectlint.load_decls()
    L = defectlint.Lint(os.path.relpath(path, G), t, fix.gd); fnd = L.run()
    kinds = ["tailfwd"] * tn; ctr = [len(re.findall(r'\bfloat fbits\d+;', t))]; ln = t.split("\n"); off = [0]
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
