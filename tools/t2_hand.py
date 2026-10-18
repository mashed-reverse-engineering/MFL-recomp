"""t2_hand.py: patterns learnt from the 45 hand repaired functions (scratch dir hand1): text -> text kinds for tools/t2_auto.py.
 fix(path)       deterministic kinds, decided by the ORIGINAL asm / the symbol table (no code change when right); called from t2_postfix.fix
   k0         `(float10)(90.0)` = Ghidra read the qword at 0x5ddc28 as a double; the original compares against the float 0.0 `k_f_0` -> `k_f_0` (+ extern at the address the original reads)
   loopbound  `(int)p < 0x657298` (raw data address, cheatgrep `rawdata`) -> `(int)p < (int)&sym + 0x4000`; the immediate must occur in the original
 stalefield(path, err)   error driven (like t2_mwfix.fix): C2039 `field_0x2c` is not a member of S (header newer than the text) -> `*(undefined4 *)((char *)&obj + 0x2c)`
 variants(t, nm, comp)   list of (label, text) that t2_auto tries one after the other AFTER a diffemu DIFF (kept only when diffemu is EQUAL, same gate as tailtrial)
"""
import os, re, subprocess, sys, bisect
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
_A = {}
def orig_asm(nm):
    """-> [(addr, op, args)] of the original function nm ([] when unknown)"""
    if nm in _A: return _A[nm]
    import t2_postfix as P, reginputs2 as R
    P.orig_const_args(nm, set())
    out = []
    if nm in P._FN:
        lo, sz = P._FN[nm]
        o = subprocess.run(["objdump", "-w", "-M", "intel", "-D", "--no-show-raw-insn", "--start-address=0x%x" % lo, "--stop-address=0x%x" % (lo + sz), R.EXE], capture_output=True, text=True).stdout
        for l in o.splitlines():
            m = re.match(r"\s*([0-9a-f]+):\t(\S+)\s*(.*)$", l)
            if m: out.append((int(m.group(1), 16), m.group(2), m.group(3).strip()))
    _A[nm] = out; return out
def _decls(t):
    return {m.group(3): (int(m.group(1), 16), m.group(2)) for m in re.finditer(r"^/\*([0-9a-f]{8})\*/ (extern [^;\n]*?)\b(\w+)(?:\[[^\]]*\])?;", t, re.M)}
_SY = {}
def _symnames():
    if not _SY:
        for l in open(os.path.join(G, "src", "symbols.txt")):
            q = l.split()
            if len(q) == 2:
                try: _SY.setdefault(int(q[1], 16), set()).add(q[0])
                except ValueError: pass
    return _SY
def _add_extern(t, line):
    ms = list(re.finditer(r"^/\*[0-9a-f]{8}\*/ extern [^\n]*\n", t, re.M))
    p = ms[-1].end() if ms else list(re.finditer(r"^#include [^\n]*\n", t, re.M))[-1].end()
    return t[:p] + line + "\n" + t[p:]

def k0(t, nm):
    """kind `k0` (REAL): see module doc.  -> (text, edits)"""
    if "(90.0)" not in t: return t, 0
    refs = []
    for a, op, args in orig_asm(nm):
        for m in re.finditer(r"DWORD PTR (?:ds:|\[)0x([0-9a-f]+)", args):
            va = int(m.group(1), 16)
            if op.startswith("f") and va not in refs: refs.append(va)
    syms = _symnames(); zs = [va for va in refs if "k_f_0" in syms.get(va, ())]
    if not zs: return t, 0
    n = len(re.findall(r"\(float10\)\(90\.0\)", t))
    if not n: return t, 0
    t = re.sub(r"\(float10\)\(90\.0\)", "k_f_0", t)
    dm = re.search(r"^/\*([0-9a-f]{8})\*/ extern float k_f_0;", t, re.M)
    if dm:
        if int(dm.group(1), 16) not in zs: t = t[:dm.start(1)] + "%08x" % zs[0] + t[dm.end(1):]
    else: t = _add_extern(t, "/*%08x*/ extern float k_f_0;" % zs[0])
    return t, n

def loopbound(t, nm):
    """kind `loopbound` (REAL): the base symbol is the nearest declared (extern) data symbol below the literal.  -> (text, edits)"""
    imm = {int(m.group(1), 16) for a, op, args in orig_asm(nm) for m in re.finditer(r"\b0x([0-9a-f]{6})\b", args)}
    if not imm: return t, 0
    dec = {(a, s) for s, (a, d) in _decls(t).items() if "(" not in d}
    byadr = sorted(dec); keys = [a for a, s in byadr]; n = 0
    def r(m):
        nonlocal n
        v = int(m.group(2), 16)
        if not ({v, v + 1, v - 1} & imm) or not (0x400000 <= v < 0x1000000): return m.group(0)
        i = bisect.bisect_right(keys, v) - 1
        if i < 0: return m.group(0)
        a, s = byadr[i]
        if v == a or v - a > 0x40000: return m.group(0)
        n += 1; return "%s(int)&%s + 0x%x" % (m.group(1), s, v - a)
    t = re.sub(r"(\(int\)[\w.\[\]>-]+\s*(?:<=|>=|<|>)\s*)0x([0-9a-f]{6})\b", r, t)
    t = re.sub(r"()\b0x([0-9a-f]{6})(?=\s*(?:<=|>=|<|>)\s*\(int\)[\w.\[\]>-]+)", r, t)
    return t, n

def nanlit(t, nm):
    """kind `nanlit` (REAL): `x = -NAN;` (Ghidra prints a NaN bit pattern as NAN) -> `*(undefined4 *)&x = 0xffffffff;`, the pattern is the NaN immediate (sign as printed) of the original"""
    nans = {"-": set(), "": set()}
    for a, op, args in orig_asm(nm):
        for m in re.finditer(r"\b0x([0-9a-f]{8})\b", args):
            v = int(m.group(1), 16)
            if (v >> 23) & 0xff == 0xff and v & 0x7fffff: nans["-" if v >> 31 else ""].add(v)
    n = 0
    def r(m):
        nonlocal n
        c = nans[m.group(3)]
        if len(c) != 1: return m.group(0)
        n += 1; return "%s*(undefined4 *)&%s = 0x%08x;" % (m.group(1), m.group(2), next(iter(c)))
    t = re.sub(r"^([ \t]*)([\w.\[\]>*()-]+?) = (-?)NAN;", r, t, flags=re.M)
    return t, n

def stalefield(path, err):
    t = open(path, errors="replace").read(); n = 0
    for m in re.finditer(r"error C2039: 'field_0x([0-9a-f]+)' : is not a member of '(\w+)'", err):
        off = m.group(1)
        pat = re.compile(r"(?<![\w.>])((?:\w+(?:\[[^\]\n]*\])?)(?:(?:\.|->)\w+(?:\[[^\]\n]*\])?)*)(\.|->)field_0x%s\b" % off)
        t2 = pat.sub(lambda mm: "(*(undefined4 *)((char *)%s%s + 0x%s))" % ("&" if mm.group(2) == "." else "", mm.group(1), off), t)
        if t2 != t: t = t2; n += 1
    if n: open(path, "w").write(t)
    return n

def fix(path):
    """deterministic kinds -> list of kinds applied"""
    nm = os.path.basename(path)[:-2]
    t = open(path, errors="replace").read(); ks = []
    for k, f in (("k0", k0), ("loopbound", loopbound), ("nanlit", nanlit)):
        t2, n = f(t, nm)
        if n and t2 != t: t = t2; ks += [k] * n
    if ks: open(path, "w").write(t)
    return ks


# ================================================================ variants (verified by diffemu, kept only when EQUAL)
def _w4(text, src):
    """cl /W4 output of `text` (compiled next to the headers of src)"""
    import core, diffemu, tempfile, shutil
    d = os.path.dirname(src); w = tempfile.mkdtemp(prefix="t2h_", dir=core.cmpfn.WORKROOT)
    try:
        for x in os.listdir(d):
            if x.endswith(".h"): shutil.copy(os.path.join(d, x), w)
        open(os.path.join(w, "f.c"), "w").write(text)
        r = subprocess.run([core.cmpfn.VC7, w, "cl.exe"] + diffemu.FLAGS + ["/W4", "f.c"], capture_output=True, text=True, timeout=300)
        return r.stdout + r.stderr
    finally: shutil.rmtree(w, ignore_errors=True)
_INT = r"(?:undefined4|undefined2|undefined1|undefined|uint|int|unsigned int|ushort|short|char|byte|ulong|long|unsigned long|unsigned short|unsigned char|bool)"
def implconv(t, nm, ctx):
    """kind `implconv` (REAL): an implicit conversion float -> integer lvalue (`g_int = fvalue;`, C4244 at /W4) where the original has fewer fistp / __ftol than the text has conversions: the original copies the bits ->
    `*(float *)&lhs = e;`.  Integer -> float lvalue likewise `*(undefined4 *)&lhs = e;` when the original has fewer fild.  -> text"""
    import t2_postfix as P
    oc = P.orig_conv(nm)
    if oc is None: return t
    nf2i, ni2f = oc
    out = _w4(t, ctx["src"]); ls = t.split("\n")
    f2i, i2f = [], []
    for m in re.finditer(r"f\.c\((\d+)\) : warning C4244: '=' : conversion from '(float|double|float10|long double)' to '([^']+)'", out):
        if re.fullmatch(_INT, m.group(3)): f2i.append(int(m.group(1)))
    for m in re.finditer(r"f\.c\((\d+)\) : warning C4244: '=' : conversion from '([^']+)' to '(float|RwReal)'", out):
        if re.fullmatch(_INT, m.group(2)): i2f.append(int(m.group(1)))
    body = re.sub(r"/\*.*?\*/", "", t, flags=re.S)
    expl = len(re.findall(r"\((?:int|uint|short|ushort|char|undefined4)\)", body)); expl_f = len(re.findall(r"\((?:float|float10|double|RwReal)\)", body))      # explicit casts: upper bound of the real conversions
    off = [0]
    for l in ls: off.append(off[-1] + len(l) + 1)
    eds = []
    def edit(lines, cast):
        for L in lines:
            while L > 1 and not re.search(r"[;{}:]\s*$|^\s*$", ls[L - 2]): L -= 1      # cl reports the LAST line of a wrapped statement
            a_ = off[L - 1]; b_ = P.stmt_end(t, a_); seg = t[a_:b_]
            m = re.match(r"^([ \t]*)(?!\*\(\w+ \*\)&)((?:\(&\w+\)|\w+)(?:\[[^\n;=]*\]|\.\w+|->\w+)*) = ([^;]+);$", seg)
            if m: eds.append((a_, b_, "%s*(%s *)&%s = %s;" % (m.group(1), cast, m.group(2), m.group(3))))
    if len(f2i) > max(0, nf2i - expl): edit(f2i, "float")
    if len(i2f) > max(0, ni2f - expl_f): edit(i2f, "undefined4")
    for a_, b_, n_ in sorted(set(eds), reverse=True): t = t[:a_] + n_ + t[b_:]
    return t

def negu2f(t, nm, ctx):
    """kind `negu2f`: `(float)-g` of an unsigned (undefined4 / uint) object negates unsigned and converts unsigned; the original has fild + neg = signed: `(float)(-(int)g)`"""
    import t2_mwfix as M
    if M._uns(nm) is not False: return t
    body = re.sub(r"/\*.*?\*/", "", t, flags=re.S)
    un = set(re.findall(r"\b(?:undefined4|uint)\s+(\w+)\s*[;,)=\[]", body)) | set(re.findall(r"^extern (?:undefined4|uint) (\w+);", t, re.M))
    un -= set(re.findall(r"\b(?:undefined4|uint)\s*\*\s*(\w+)", body))
    return re.sub(r"\((float10|float|double)\)-(\w+)\b(?!\s*[\[(.])", lambda m: "(%s)(-(int)%s)" % (m.group(1), m.group(2)) if m.group(2) in un else m.group(0), t)

def sgn(t, nm, ctx):
    """kind `sgn` (variant): unsigned objects (undefined4 / uint) inside a comparison are signed in the original (jl / jg, no jb / ja): `(int)` casts on every such identifier of the comparing statement"""
    body = re.sub(r"/\*.*?\*/", "", t, flags=re.S)
    un = set(re.findall(r"\b(?:undefined4|uint)\s+(\w+)\s*[;,)=\[]", body)) | set(re.findall(r"^extern (?:undefined4|uint) (\w+);", t, re.M))
    un -= set(re.findall(r"\b(?:undefined4|uint)\s*\*\s*(\w+)", body))
    un -= set(re.findall(r"\b(\w+)\s*\[", body))
    if not un: return t
    out = []
    for l in t.split("\n"):
        if re.search(r"(?<![<>=-])(?:<=|>=|<|>)(?![<>=])", l) and not l.lstrip().startswith(("#", "/*", "extern")):
            l = re.sub(r"(?<![\w.>&*])(%s)\b(?!\s*[\[(.]|\s*=[^=])" % "|".join(sorted(map(re.escape, un))), lambda m: m.group(0) if l[max(0, m.start() - 5):m.start()].endswith("(int)") else "(int)" + m.group(1), l)
        out.append(l)
    return "\n".join(out)

def _stack_alloc(insns):
    """stack bytes allocated before the first call / branch (`sub esp,N`, early `push ecx`) and the callee-saved registers pushed"""
    g = 0; saved = set()
    for i, (m, o) in enumerate(insns[:24]):
        if m == "call" or m == "jmp" or m.startswith("j") or m == "ret": break
        if m == "push" and o in ("ebx", "esi", "edi", "ebp"): saved.add(o)
        elif m == "push" and o == "ecx" and i < 3: g += 4
        elif m == "sub" and re.fullmatch(r"esp,\s*(0x[0-9a-f]+|\d+)", o): g += int(o.split(",")[1], 0)
    return g, saved
def pad(t, nm, ctx):
    """kind `pad` (REAL): stubs write by absolute address, so the frame size must equal the original's `sub esp,N`: a pad member at the top of the frame struct for the difference"""
    import core, diffemu
    m = None
    for m in re.finditer(r"^(  \} frame;)", t, re.M): pass
    if not m: return t
    go, so = _stack_alloc([(m_, o_) for a, m_, o_ in orig_asm(nm)])
    open(ctx["src"], "w").write(t); obj, _ = diffemu.compile_obj(ctx["src"]); code, *_ = core.our_code(obj, nm)
    ins = [(x.mnemonic, x.op_str) for x in core.CS.disasm(bytes(code[:80]), core.OURS)]
    gu, su = _stack_alloc(ins)
    if go <= gu or (go - gu) % 4: return t
    k = (go - gu) // 4
    return t[:m.start()] + "    int pad_top%s;\n" % ("" if k == 1 else "[%d]" % k) + t[m.start():]

def crtalias(t, nm, ctx):
    """kind `crtalias` (variant): `extern double acos(double)` at an address inside the exe is the game's own function (own clamping / rounding), the C name stands for the host libm in diffemu: the name becomes FUN_<va>"""
    for m in re.finditer(r"^/\*([0-9a-f]{8})\*/ extern double (\w+)\(", t, re.M):
        if m.group(2) in ("acos", "asin", "floor", "ceil", "fmod", "pow", "atan2", "atan", "tan"): t = re.sub(r"(?<![\w.])%s\(" % m.group(2), "FUN_%s(" % m.group(1), t)
    return t

def retdouble(t, nm, ctx):
    """kind `retdouble` (REAL): ST0 live at the original's `ret` (diffemu `x87 stack top orig 1 ours 0`): `void f` -> `double f` returning the last assigned float local"""
    if "x87 stack top orig 1 ours 0" not in ctx.get("dtx", ""): return t
    h = re.search(r"^void (?:__cdecl )?%s\s*\(" % re.escape(nm), t, re.M)
    m = re.search(r"\n  return;\n\}\s*$", t)
    if not h or not m: return t
    fl = set(re.findall(r"^  (?:float10|double|float|RwReal) (\w+);", t, re.M))
    last = None
    for a in re.finditer(r"(?<![\w.>])(\w+) = ", t[h.end():m.start()]):
        if a.group(1) in fl: last = a.group(1)
    if not last: return t
    return t[:h.start()] + "double" + t[h.start() + 4:m.start()] + "\n  return %s;\n}\n" % last

def intparam(t, nm, ctx):
    """kind `intparam` (variant): `float param_N` that is an integer in the original (`(int)param_N`) -> `*(int *)&param_N`; `(float)v != param_N` -> `v != *(int *)&param_N`"""
    ps = set(re.findall(r"\bfloat (param_\d+)\b", t))
    for p in ps:
        t = re.sub(r"\(int\)%s\b" % p, "*(int *)&%s" % p, t)
        t = re.sub(r"\(float\)(\w+) (!=|==) %s\b" % p, r"\1 \2 *(int *)&%s" % p, t)
    return t

def ternary(t, nm, ctx):
    """kind `ternary` (variant): `if (c) { v = A; } else { v = B; }  f(v, ..);` (Ghidra merges two call sites with constant arguments through a variable) -> `if (c) f(A, ..); else f(B, ..);`"""
    def r(m):
        ind, c, v, A, B, ind2, call = m.groups()
        sub = lambda k: re.sub(r"\b%s\b" % v, k, call)
        return "%sif (%s) {\n%s  %s\n%s}\n%selse {\n%s  %s\n%s}\n" % (ind, c, ind, sub(A), ind, ind, ind, sub(B), ind) + ""
    return re.sub(r"([ \t]*)if \((.+)\) \{\n\s*(\w+) = (0x[0-9a-f]+|\d+);\n\s*\}\n\s*else \{\n\s*\3 = (0x[0-9a-f]+|\d+);\n\s*\}\n([ \t]*)(\w+\([^;\n]*\b\3\b[^;\n]*\);)\n", r, t)

def storereload(t, nm, ctx):
    """kind `storereload` (variant): `v = P * s; *(T *)((char *)B + *(int *)((char *)&C + v) * 4 + v) = k; *(int *)((char *)&C + v) = *(int *)((char *)&C + v) + 1;` (append to the array that
    starts at B with its count at C = B + 4c) -> `pt = (int *)((char *)B + P * s); pt[pt[c]] = k; pt[c] = pt[c] + 1;`: the original reloads the count after the store"""
    dec = _decls(t); n = [0]
    def r(m):
        ind, v, e, ty, B, C, k = m.group(1), m.group(2), m.group(3), m.group(4), m.group(5), m.group(6), m.group(7)
        if B not in dec or C not in dec or (dec[C][0] - dec[B][0]) % 4 or not 0 < dec[C][0] - dec[B][0] < 64: return m.group(0)
        c = (dec[C][0] - dec[B][0]) // 4; n[0] += 1
        return "%spt = (int *)((char *)%s + %s);\n%spt[pt[%d]] = %s;\n%spt[%d] = pt[%d] + 1;\n" % (ind, B, e, ind, c, k, ind, c, c)
    pat = (r"([ \t]*)(\w+) = ([^;\n]+);\n\s*\*\((\w+) \*\)\(\(char \*\)(\w+) \+ \*\(int \*\)\(\(char \*\)&(\w+) \+ \2\) \* 4 \+ \2\) = ([^;]+?)\s*;\n"
           r"\s*\*\(int \*\)\(\(char \*\)&\6 \+ \2\) = \*\(int \*\)\(\(char \*\)&\6 \+ \2\) \+ 1;\n")
    t2 = re.sub(pat, r, t)
    if not n[0]: return t
    return re.sub(r"(^(?!static|extern)[^\n]*\b%s\([^;{]*\)\s*\n+\{\n)" % re.escape(nm), r"\1  int *pt;\n", t2, count=1, flags=re.M)

def f10(t, nm, ctx):
    """kind `f10` (variant, one per local): a float local that never reaches memory keeps the extended x87 value in the original: `float fVar2;` -> `float10 fVar2;` (all locals of this kind; search() also tries each alone)"""
    only = ctx.get("f10only")
    def r(m):
        return "%sfloat10 %s;" % (m.group(1), m.group(2)) if only is None or m.group(2) in only else m.group(0)
    return re.sub(r"^(  )float (fVar\d+);", r, t, flags=re.M)

def bitops(t, nm, ctx):
    """kind `bitops` (variant): bit arithmetic on float objects: `(uint)*(float *)(p + 0x20) & 2` -> `*(uint *)(p + 0x20) & 2`; `L = (float)((uint)A | C);` -> `*(uint *)&L = *(uint *)&A | C;` (the original has no fild / fistp there)"""
    t = re.sub(r"\(\((uint|int|undefined4)\)\*\(float \*\)(\([^()\n]*\)|\w+)\s*([&|^])", r"(*(\1 *)\2 \3", t)
    t = re.sub(r"\((uint|int|undefined4)\)\*\(float \*\)(\([^()\n]*\)|\w+)(?=\s*[&|^])", r"*(\1 *)\2", t)
    lv = r"[\w.\[\]>-]+"
    t = re.sub(r"^([ \t]*)(%s) = \(float\)\(\(uint\)(%s) \| (0x[0-9a-f]+)\);" % (lv, lv), r"\1*(uint *)&\2 = *(uint *)&\3 | \4;", t, flags=re.M)
    return re.sub(r"^([ \t]*)([\w.>-]+(?:\[[^\]\n]*\])+) = \((?:uint|int|undefined4)\)(\(float\)[^;\n]+);", r"\1*(float *)&\2 = \3;", t, flags=re.M)      # `p[k] = (uint)(float)x;` stores the float bits

KINDS = [("implconv", implconv), ("negu2f", negu2f), ("sgn", sgn), ("pad", pad), ("crtalias", crtalias), ("retdouble", retdouble), ("intparam", intparam), ("ternary", ternary), ("storereload", storereload), ("bitops", bitops), ("f10", f10)]
def search(t, nm, ctx, check, maxtry=6):
    """ctx: src (path the candidate texts are written to), dtx (diffemu text of the failing run).  check(text) -> True when compiled + diffemu EQUAL + verified.  Tries: all applicable kinds together, each left out, each alone.
    -> (labels, text) of the first accepted combination, else None"""
    app = []
    for lab, f in KINDS:
        try: t2 = f(t, nm, ctx)
        except Exception as ex: print("t2_hand %s %s: %s" % (nm, lab, ex), flush=True); continue
        if t2 != t: app.append((lab, f))
    if not app: return None
    combos = [tuple(range(len(app)))]
    if len(app) == 2: combos += [(0,), (1,)]
    elif len(app) > 2: combos += [tuple(j for j in range(len(app)) if j != i) for i in range(len(app))] + [(i,) for i in range(len(app))]
    seen = set(); tried = 0
    for c in combos:
        if tried >= maxtry: break
        cur = t; labs = []
        for j in c:
            try: t2 = app[j][1](cur, nm, ctx)
            except Exception: continue
            if t2 != cur: cur = t2; labs.append(app[j][0])
        if cur == t or cur in seen: continue
        seen.add(cur); tried += 1
        if check(cur): return labs, cur
    return None
