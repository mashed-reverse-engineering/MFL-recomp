#!/usr/bin/env python3
"""t2_auto.py [NAMES_FILE | --census CLASS] [--max N] [--text FILE] [--cases N] [--no-candidates] [--force] [--run DIR]
t2_auto.py --publish DIR      (coordinator only: merges a finished run into the shared files, see below)
ISOLATED: a run writes ONLY below DIR (default output/t2_auto/runs/<YYYYmmdd_HHMMSS>, gitignored): results.csv summary.txt failures/<name>.md candidates/an_hk_<n>.c (numbered from 1) + names.txt
attempts/<name>.md overlay/ (extra externs / aliases / callee protos, T2_OVERLAY_DIR, read by autocompile / callee_auto / cmpfn / rawaddr in addition to the shared files) diffemu/ (DIFFEMU_OUT: results, cache, diffs)
and uses its own compile container mfl_t2_<run id> (removed at the end).  The shared files are only read.  --publish DIR copies a finished run into output/t2_auto_results.csv (merge by name),
output/t2_auto_summary.txt, output/tier2_candidates2 (renumbered to the next free an_hk_<n>, names.txt appended), names/MFL_extern_fix.txt, src/aliases.txt, names/MFL_callee_protos_auto.txt,
output/t2_failures, output/attempts.  Functions already defined in src/*.c, bridges, tier1, automatch*, automatch_agent, automatch_near never enter the pool.
Scripted tier 2 candidate pipeline: a pool of functions goes through generate -> compile -> diffemu -> lint -> ptrcast re-verify without an LLM; only the failures need one.
Default pool: output/t2_auto/pool.txt (rows `bytes name va ...`, the batch 5 pool of 613 wave 0/1 functions <= 700 B); NAMES_FILE = such a file or one name per line;
--census CLASS = names of output/tier2_census.csv with that blocker; --text FILE = another decompilation (same `// ==== name @ va` format; a Ghidra export of FUN_ functions, default
output/named/MFL_app_named.c has bodies for named functions only).  Needs the diffemu venv (re-execs itself), docker container `ac` (VC7_CONTAINER), no Ghidra, no game.
Stages (one CSV row per function, output/t2_auto_results.csv):
  gen      autocompile.py (chunk 1, AC_FIX=all, libprotos / callee protos / all rules) with scripted body fixes: denormal float literal compares (`x == 1.4013e-45` is the dword 1), float-to-pointer
           casts (`(T *)fvar` -> `*(T **)&fvar`), void indirect call used as a value, mid-block declarations hoisted to the block top, C2065 data names via undeclared_fix.py; up to 3 rounds,
           each round only on the functions that failed (compiled functions are never touched).  Skipped before generation (not scriptable): register inputs, labels / jump tables, unprototyped callee.
  compile  cl 13.00 /nologo /c /Gy /O2 /Ob0 (diffemu flags), first error with its text
  diffemu  tools/diffemu (3 processes), EQUAL >= 20 cases AND coverage >= 0.8 on both sides
  lint     defectlint (any finding), cheatgrep, register callees, duplicate symbol names; then ptrcast.rewrite + rawaddr --rewrite and a second diffemu run on the rewritten text
Pass = candidate: output/tier2_candidates2/an_hk_<n>.c (+ names.txt line, batches of <= 40 numbered consecutively), NOT installed.
Failure = output/t2_failures/<name>.md (<= 40 lines, for one small Haiku task).  Summary: output/t2_auto_summary.txt."""
import os, sys
if "--publish" in sys.argv:
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__))); import t2_publish; sys.exit(t2_publish.main(sys.argv[sys.argv.index("--publish") + 1]))
try: import unicorn
except ImportError:
    py = os.environ.get("DIFFEMU_PY") or os.path.expanduser("~/.cache/mfl_diffemu_venv/bin/python"); os.execv(py, [py] + sys.argv)
import atexit, collections, csv, glob, json, re, shutil, struct, subprocess, tempfile, time
from concurrent.futures import ThreadPoolExecutor
from multiprocessing import Pool
HERE = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(HERE, ".."))
sys.path[:0] = [HERE, os.path.join(HERE, "diffemu")]
P = lambda *a: os.path.join(G, *a)
# the run directory and everything that depends on it must be set before core / diffemu / cmpfn are imported (they read the environment at import)
_a = sys.argv[1:]
RUN = os.path.abspath(_a[_a.index("--run") + 1]) if "--run" in _a else P("output", "t2_auto", "runs", time.strftime("%Y%m%d_%H%M%S"))
RUNID = re.sub(r"[^A-Za-z0-9_.-]", "_", os.path.basename(RUN.rstrip("/")))
os.makedirs(RUN, exist_ok=True)
os.environ.update(T2_OVERLAY_DIR=os.path.join(RUN, "overlay"), DIFFEMU_OUT=os.path.join(RUN, "diffemu"), DEFECTLINT_OUT=os.path.join(RUN, "defectlint.csv"), ATTEMPTS_DIR=os.path.join(RUN, "attempts"), VC7_CONTAINER="t2_" + RUNID)
os.makedirs(os.environ["T2_OVERLAY_DIR"], exist_ok=True)
def _rm_container(): subprocess.run(["docker", "rm", "-f", "mfl_t2_" + RUNID], capture_output=True)      # exactly this run's container
atexit.register(_rm_container)
import core, diffemu, defectlint, cheatgrep, ptrcast, rawaddr, t2_postfix, callee_auto, gsfix, strictfix
HDR = os.path.join(RUN, "overlay", "ghidra_types.h")
W = RUN; FILES = os.path.join(W, "files"); FAILS = os.path.join(RUN, "failures"); CAND = os.path.join(RUN, "candidates"); POOL = P("output", "t2_auto", "pool.txt")
SHARED_CAND = P("output", "tier2_candidates2")
NAMED = P("output", "named", "MFL_app_named.c")
SYNC = lambda s: re.sub(r"\W", "_", s)[:40]

# ------------------------------------------------------------------------------------------------------------- scripted body fixes
FLT = 1.401298464324817e-45
def denormal(b):
    """Ghidra prints an integer dword compared / stored through a float object as a denormal literal (`x == 1.4013e-45`): the dword value k, written as an int access"""
    def k(s):
        v = float(s)
        if not 0 < abs(v) < 1.17549435e-38: return None
        n = round(v / FLT)
        return n if n and abs(struct.unpack("f", struct.pack("I", n & 0xffffffff))[0] - v) <= abs(v) * 1e-3 else None
    E = r"(\*?\w+(?:->\w+|\.\w+|\[[^\]\[]+\])*)"; L = r"(-?\d\.\d+e-\d\d)"
    b = re.sub(E + r" (==|!=|<=|>=|<|>) " + L + r"(?![\w.])", lambda m: "*(int *)&%s %s %d" % (m.group(1), m.group(2), k(m.group(3))) if k(m.group(3)) is not None and not m.group(1).startswith("(") else m.group(0), b)
    b = re.sub(r"\bcase (-?\d\.\d+(?:e-\d\d)?):", lambda m: "case %d:" % (k(m.group(1)) if k(m.group(1)) is not None else int(float(m.group(1)))), b)      # `case 1.4013e-45:` in a switch on an undefined4 / float object (C2052): the dword
    return re.sub(r"^(\s*)" + E + r" = " + L + r";", lambda m: "%s*(int *)&%s = %d;" % (m.group(1), m.group(2), k(m.group(3))) if k(m.group(3)) is not None else m.group(0), b, flags=re.M)
def strayassign(b):
    """Ghidra prints a statement inside a multi-line expression (`if (a <=\n p = &x[i];\n p->f) {`): the assignment moves in front of the statement the expression belongs to"""
    L = b.split("\n"); i = 1
    while i < len(L):
        if re.fullmatch(r"\s*\w+ = [^;\n(]*;", L[i]) and re.search(r"(?:<=|>=|<|>|==|!=|&&|\|\||[-+*/&|(,=])$", L[i - 1].rstrip()):
            k = i - 1
            while k > 0 and not re.search(r"[;{}]$", L[k - 1].rstrip()): k -= 1
            a = L.pop(i); L.insert(k, re.match(r"\s*", L[k]).group(0) + a.strip())
        i += 1
    return "\n".join(L)
def intbits(b):
    """`fvar = 0x3f800000;` (a float local gets an int bit pattern): copy the bits"""
    fl = set(re.findall(r"\b(?:float|RwReal) (\w+);", b))
    return re.sub(r"^(\s*)(\w+) = (0x[0-9a-f]{7,8});", lambda m: "%s*(undefined4 *)&%s = %s;" % (m.group(1), m.group(2), m.group(3)) if m.group(2) in fl else m.group(0), b, flags=re.M)
def floatptr(b):
    """C2440 float -> pointer: Ghidra's `(RpClump *)pfVar[-0x22]` copies the bits of a float object: `*(RpClump **)&pfVar[-0x22]`"""
    fl = set(re.findall(r"\b(?:float|RwReal) (\w+)\s*[;,)]", b)); fp = set(re.findall(r"\b(?:float|RwReal) \*+(\w+)\s*[;,)=\[]", b))
    def r(m):
        t, x = m.group(1), m.group(2); base = re.match(r"\*?(\w+)", x).group(1)
        ok = (x.startswith("*") and base in fp) or (re.fullmatch(r"\w+\[[^\]]+\]", x) and base in fp) or (x == base and base in fl) or ("." in x and "[" in x)
        return "*(%s*)&%s" % (t, x) if ok else m.group(0)
    return re.sub(r"\(((?:\w+ )*\w+ \*+)\)(\*?\w+(?:\[[^\]\[]+\])?(?:\.\w+)?)(?![\w\[(])", r, b)
def ptrfloat(b):
    """C2440: a float slot Ghidra typed `float *pfStack_54` that only ever receives `(float *)(float expression)` and is read back as `(float)x.pfStack_54` (never dereferenced) is a float."""
    for m in list(re.finditer(r"\bfloat \*(\w+);", b)):
        n = m.group(1)
        if re.search(r"\*\s*\w*\.?%s\b|\b%s\s*\[|%s->" % (n, n, n), b.replace(m.group(0), "")) or not re.search(r"\(float\)(?:\w+\.)?%s\b" % n, b): continue
        b = re.sub(r"\bfloat \*%s;" % n, "float %s;" % n, b); b = re.sub(r"(\b%s = )\(float \*\)" % n, r"\1", b); b = re.sub(r"\(float\)((?:\w+\.)?%s)\b" % n, r"\1", b)
    return b
def valcall(b):
    """C2120: `v = (**(code **)(p + 4))(x);` / `(*(code *)p)(x)` used as a value: the callee returns int, `code` is void"""
    return re.sub(r"(?:(?<=[=,!<>&|+\-] )|(?<=\())\(\*(\*?)\(code (\*+)\)", lambda m: "(*%s(int (%s)())" % (m.group(1), m.group(2)), b)
_TYPES = None
def typenames():
    global _TYPES
    if _TYPES is None:
        t = open(HDR if os.path.exists(HDR) else P("src", "auto", "ghidra_types.h"), errors="replace").read() + open(P("src", "auto", "prelude.h"), errors="replace").read()
        _TYPES = set(re.findall(r"typedef [^;{]*?\b(\w+)(?:\[\d+\])?\s*;", t)) | set(re.findall(r"\}\s*(\w+)\s*;", t)) | set(re.findall(r"\bstruct (\w+)", t)) | {"int", "uint", "char", "short", "long", "float", "double", "unsigned", "byte", "ushort", "ulong"}
    return _TYPES
def hoist(b):
    """C89 (VC7 C2143): a declaration after a statement of the same block moves to the top of that block (`T x = e;` -> `T x;` at the top, `x = e;` in place)"""
    ty = typenames(); L = b.split("\n"); stack = []; ins = []; out = {}
    DECL = re.compile(r"^(\s+)((?:const |struct |unsigned |signed )*)(\w+)((?: \*+| )\s*)(\w+)(\[\w*\])?(?: = (.*))?;$")
    for i, l in enumerate(L):
        s = l.strip(); m = DECL.match(l)
        if stack and m and m.group(3) in ty and m.group(3) not in ("return", "else", "goto") and (s.count("(") == s.count(")")):
            blk = stack[-1]
            if blk["seen"]:
                ins.append((blk["at"], "%s%s%s%s%s%s;" % (m.group(1), m.group(2), m.group(3), m.group(4), m.group(5), m.group(6) or "")))
                out[i] = ("%s%s = %s;" % (m.group(1), m.group(5), m.group(7))) if m.group(7) else None
            else: blk["at"] = i + 1
        elif s and not s.startswith(("/*", "//", "{", "}")) and stack: stack[-1]["seen"] = True
        for c in s:
            if c == "{": stack.append(dict(seen=False, at=i + 1))
            elif c == "}" and stack: stack.pop()
        if s.startswith("{") and stack: stack[-1]["at"] = i + 1
    if not ins: return b
    res = {}
    for at, d in ins: res.setdefault(at, []).append(d)
    o = []
    for i, l in enumerate(L):
        o += res.get(i, [])
        if i in out:
            if out[i] is not None: o.append(out[i])
        else: o.append(l)
    return "\n".join(o)
def floatcmp(b):
    """Ghidra prints x87 compares as `a < b != (a == b)` (= a <= b) and `a < b == (a == b)` (= a > b... neither less nor equal); the XOR / equality of the two flags is the plain compare"""
    O = r"(\w+(?:(?:->|\.)\w+)*|\(\w+\))"
    b = re.sub(O + r" < " + O + r" != \(\1 == \2\)", r"\1 <= \2", b)
    b = re.sub(O + r" < " + O + r" == \(\1 == \2\)", r"\1 > \2", b)
    b = re.sub(O + r" > " + O + r" != \(\1 == \2\)", r"\1 >= \2", b)
    return b
def fix_round(body, errs):
    """errs: first error text of the last attempt ('' = proactive round 0)"""
    b = t2_postfix.isnan(denormal(intbits(strayassign(floatcmp(body)))))
    if "C2440" in errs: b = ptrfloat(floatptr(b))
    if "C2036" in errs: b = re.sub(r"\(&(PTR_\w+)\)\[", r"((undefined4 *)\1)[", b)      # extern char PTR_x[] (jump table of code addresses) indexed through `&PTR_x`: pointer to an array of unknown size
    b = valcall(b)      # proactive: a statement call is not preceded by `= ` / `(`, only value uses change
    if re.search(r"C2143|C2059|C2146|C2106", errs): b = hoist(strayassign(b))
    m = re.search(r"C2065 '(\w+)' : undeclared", errs)
    if m and m.group(1) in t2_postfix.enums():      # an enum constant the header lacks (COURSE_ARCTIC): define every enum name the body uses (the compiler reports one per round)
        ds = "".join("#define %s (%s)\n" % (k, v) for k, v in t2_postfix.enums().items() if re.search(r"\b%s\b" % k, b) and not re.search(r"#define\s+%s\b" % k, b))
        b = ds + b
    return b

# ------------------------------------------------------------------------------------------------------------- already defined by hand / installed code
_DEF = set()
def defined_elsewhere():
    """names with a definition in hand written src/*.c, bridges, tier1, automatch, automatch_agent, automatch_near (definitions scanned, not the ledger): `f_c` of a bridge and its naked `f` count as `f`"""
    if not _DEF:
        for pat in ("src/*.c", "src/bridges/*.c", "src/tier1/*.c", "src/automatch/*.c", "src/automatch_agent/*.c", "src/automatch_near/*.c"):
            for f in glob.glob(P(pat)):
                _DEF.update(core.defs(f)); _DEF.update(re.findall(r"__declspec\(naked\)[\w \*]*?\b(\w+)\(", open(f, errors="replace").read()))
        _DEF.update(n[:-2] for n in list(_DEF) if n.endswith("_c"))
    return _DEF

# ------------------------------------------------------------------------------------------------------------- selection
def load_funcs(path):
    t = open(path, errors="replace").read(); f = {}
    for m in re.finditer(r"// ==== (\S+) @ ([0-9a-f]+)\n(.*?)(?=\n// ==== |\Z)", t, re.S): f.setdefault(m.group(1), (m.group(2), gsfix.clean(m.group(3))))
    return f
def select(a):
    rows = []
    if "--census" in a:
        c = a[a.index("--census") + 1]
        rows = [(int(r["size"]), r["name"], r["va"]) for r in csv.DictReader(open(P("output", "tier2_census.csv"))) if r["blocker"] == c]
    else:
        pos = [x for i, x in enumerate(a) if not x.startswith("--") and (i == 0 or a[i - 1] not in ("--max", "--text", "--cases", "--run"))]
        for l in open(pos[0] if pos else POOL):
            p = l.split()
            if len(p) >= 3 and p[0].isdigit(): rows.append((int(p[0]), p[1], p[2]))
            elif p: rows.append((0, p[0], ""))
    if "--max" in a: rows = rows[:int(a[a.index("--max") + 1])]
    return rows

_NOAD = []
def noadapt():
    """register callees without an F_r adapter (the set autocompile.py computes for AC_PLAIN)"""
    if not _NOAD:
        sys.path.insert(0, HERE); import bridgegen as bg
        bl, sg, ri, man = bg.load(); ad = sorted(a for a, _ in bl.values()); nx = {a: ad[i + 1] if i + 1 < len(ad) else a + 0x400 for i, a in enumerate(ad)}
        ok = set(bg.adapter_ok(bl, sg, ri, man, nx)) | set(bg.synth_adapters(bl, ri, {})[0])
        _NOAD.append({c for c, (r, _) in ri.items() if r.strip("|") and c not in ok})
    return _NOAD[0]
def _callee_names():
    return {"FUN_" + p[0]: p[1] for p in (l.rstrip("\n").split(",", 2) for l in open(P("names", "MFL_callee_names.csv")) if not l.startswith("#")) if len(p) >= 2}
WHY = {}      # FUN_ callee -> why tools/callee_auto.py could not write an exact prototype
_FNS = {}
def _fnsize():
    if not _FNS:
        for l in open(P("output", "fnsizes.txt")):
            q = l.split()
            if len(q) == 3: _FNS[q[1]] = int(q[2])
    return _FNS
def skip_class(n, body, regin, have, ok_protos):
    """reasons autocompile.py drops a function without trying (and tier2_census.py names them): -> (class, detail) or None"""
    REG = re.compile(r"\b(reg_e(?:ax|bx|cx|dx|si|di|bp)|in_[A-Z]{2,3}|unaff_[A-Za-z]+|extraout_\w+|stack0x\w+|param_\d+_00\w+|unaff_retaddr|[iu]?Stack[0-9a-f]{8}|in_stack_\w+|POPCOUNT|SBORROW\d|CARRY\d|halt_baddata|ABS|fpatan|fsin|fcos|fptan|fyl2x|f2xm1)\b")
    flat = re.sub(r"\s+", " ", body); rf = set(REG.findall(body))
    if n in have: return "gen-skip:manifest-C", "hand written C exists"
    if n in regin: return "gen-skip:usercall", "register inputs " + regin[n]
    sz = _fnsize().get(n)
    if sz and len(body.splitlines()) > 40 + sz // 2: return "gen-skip:text-mismatch", "%d lines of decompile text for %d bytes (Ghidra function boundary swallowed a neighbouring body: gamesave_update, trail_update_00414f00)" % (len(body.splitlines()), sz)
    if re.search(r"\breg_e", body.split("{", 1)[0]): return "gen-skip:usercall", "register parameter in its own definition (a register-convention function: plain C cannot be called with it)"
    if rf and all(r.startswith("reg_e") for r in rf):      # reg_e?? locals = arguments of register callees: generated through the F_r adapters (autocompile AC_PLAIN) unless a callee has none
        na = noadapt(); cn = _callee_names()
        miss = sorted({cn.get(c, c) for c in re.findall(r"\b(\w+) ?\(", flat)} & na - {n})
        if miss: return "gen-skip:plain-regcallee-noadapter", "no F_r adapter for " + ",".join(miss)[:150]
        rf = set()
    if re.search(r"\bswitchD_\w+::", body): return "gen-skip:switch-table", "jump table data referenced as switchD_xxx::switchdataD_xxx (not expressible in C)"
    rf -= {"fsin", "fcos", "fpatan", "ABS"}      # x87 intrinsics: autocompile AC_X87 writes them as sin / cos / atan2 / fabs with #pragma intrinsic (cl emits the same inline fsin / fcos / fpatan / fabs)
    if rf: return ("gen-skip:plain-regcallee", ",".join(sorted(rf))[:80]) if all(r.startswith("reg_e") for r in rf) else ("gen-skip:x87-intrinsic" if rf <= {"fsin", "fcos", "fpatan", "ABS"} else "gen-skip:usercall", ",".join(sorted(rf))[:80])
    un = sorted({c for c in re.findall(r"\b(FUN_[0-9a-f]{8}) ?\(", flat) if c not in ok_protos})
    if un:
        rg = [c for c in un if WHY.get(c) == "regin"]
        if rg: return "gen-skip:regin-callee", "register-input callee " + ",".join(rg)[:80]
        return "gen-skip:unproto-callee", ";".join("%s(%s)" % (c, WHY.get(c, "?")) for c in un)[:200]
    bad = [c for c, x in re.findall(r"\b(\w+) ?\(([^;{}]*)\);", flat) if c not in ("if", "while", "for", "switch", "sizeof", "return") and not callee_auto.float_literal_ok(c, x)]
    if bad: return "gen-skip:float-literal-call", "float literal passed to a callee without a float parameter there: " + ",".join(sorted(set(bad)))[:120]
    return None

# ------------------------------------------------------------------------------------------------------------- generate (autocompile rounds)
def run_ac(tag, text, names):
    d = os.path.join(W, tag); shutil.rmtree(d, ignore_errors=True); os.makedirs(d)
    nf = os.path.join(d, "names.txt"); open(nf, "w").write("".join(n + "\n" for n in names))
    env = dict(os.environ, AC_ONLY=nf, AC_FIX="all", AC_JOBS="3", AC_TEXT=text, AC_PLAIN="1", AC_X87="1")      # AC_PLAIN: calls of register callees go through the static F_r adapters (called plain they diverge in diffemu: 38 of the 124 call-trace-count DIFFs of the whole pool run)
    r = subprocess.run([sys.executable, os.path.join(HERE, "autocompile.py"), d, P("output", "decls.h"), "0", "ffffffff", "1"], env=env, capture_output=True, text=True)
    open(os.path.join(d, "log.txt"), "w").write(r.stdout + r.stderr)
    failed = dict(l.rstrip("\n").split("\t", 1) for l in open(os.path.join(d, "failed.txt")) if "\t" in l) if os.path.exists(os.path.join(d, "failed.txt")) else {}
    where = {}
    for f in sorted(glob.glob(os.path.join(d, "*.c"))):
        if os.path.basename(f) in ("ok.c", "chunk.c"): continue
        m = re.search(r"^/\* (\S+) \*/$", open(f, errors="replace").read(), re.M)
        if not m: continue
        b = os.path.basename(f)
        if b.startswith("ok_") or (m.group(1) not in where): where[m.group(1)] = (f, b.startswith("ok_"))
        if b.startswith("ok_"): where[m.group(1)] = (f, True)
    return failed, where

_STDC = {}
def stdcall_def(n, va, body):
    """the original pops its own arguments (`ret N`, output/stdcall.txt): the definition is __stdcall with N/4 parameters (unused trailing ones added when Ghidra found fewer)"""
    if not _STDC: _STDC.update({l.split()[0]: int(l.split()[1]) for l in open(P("output", "stdcall.txt")) if len(l.split()) == 2})
    if va not in _STDC: return body
    m = re.search(r"^([A-Za-z_][\w \*]*?\s)(%s)\s*\(([^)]*)\)(\s*\{)" % re.escape(n), body, re.M)
    if not m or "__stdcall" in m.group(1): return body
    k = _STDC[va] // 4; ps = [x for x in m.group(3).split(",") if x.strip() and x.strip() != "void"]
    ps += ["int unused%d" % i for i in range(len(ps) + 1, k + 1)]
    return body[:m.start()] + "%s__stdcall %s(%s)%s" % (m.group(1), n, ", ".join(ps) or "void", m.group(4)) + body[m.end():]

def generate(names, funcs, text_path):
    """-> (status {name: (ok, file, err, rounds)}); round 0 proactive fixes, then error driven rounds on the failures only"""
    status = {}; todo = list(names); errs = {n: "" for n in names}; cur = {n: funcs[n][1] for n in names}
    lab = {}      # `&LAB_00494320` = address of code that is no function (callback, thunk target): declare it before the first round (autocompile does not compile, the later compile stage cannot retry)
    for n in names:
        for x in re.findall(r"&\s*(LAB_[0-9a-f]{8})\b", cur[n]):
            if not re.search(r"^\s*%s:|goto %s;" % (x, x), cur[n], re.M): lab.setdefault(n, set()).add(x)
    if lab:
        ff = os.path.join(W, "failed_lab.txt"); open(ff, "w").write("".join("%s\tf.c(1) : error C2065 '%s' : undeclared identifier\n" % (n, x) for n, xs in lab.items() for x in sorted(xs)))
        subprocess.run([sys.executable, os.path.join(HERE, "undeclared_fix.py"), ff], capture_output=True)
    for rnd in range(12):      # autocompile reports ONE error per function: a body with several undeclared names (s_german__.., s_spanish__.. ) needs one round each
        if not todo: break
        txt = os.path.join(W, "text_r%d.c" % rnd)
        with open(txt, "w") as f:
            for n in todo:
                cur[n] = stdcall_def(n, funcs[n][0], fix_round(cur[n], errs[n])); f.write("// ==== %s @ %s\n%s\n" % (n, funcs[n][0], cur[n]))
        if rnd and any("C2039" in errs[n] and "field_0x" in errs[n] for n in todo):      # `field_0xNN` of a struct whose bytes are `_gap_` padding: split out of the gap, in this run's header copy (tools/struct_gaps.py)
            ff = os.path.join(W, "failed_g%d.txt" % (rnd - 1)); open(ff, "w").write("".join("%s\t%s\n" % (n, errs[n]) for n in todo))
            subprocess.run([sys.executable, os.path.join(HERE, "struct_gaps.py"), ff, HDR], capture_output=True)
        if rnd and any("C2065" in errs[n] for n in todo):      # data names without a declaration -> names/MFL_extern_fix.txt (+ src/aliases.txt), additive and idempotent
            ff = os.path.join(W, "failed_r%d.txt" % (rnd - 1)); open(ff, "w").write("".join("%s\t%s\n" % (n, errs[n]) for n in todo))
            subprocess.run([sys.executable, os.path.join(HERE, "undeclared_fix.py"), ff], capture_output=True)
        failed, where = run_ac("ac%d" % rnd, txt, todo)
        nxt = []
        for n in todo:
            if n in where and where[n][1] and n not in failed: status[n] = (True, where[n][0], "", rnd)
            else:
                e = failed.get(n, "not generated by autocompile (see ac%d/log.txt)" % rnd); status[n] = (False, where[n][0] if n in where else None, e, rnd)
                if e != errs[n] or rnd == 0: nxt.append(n)
                errs[n] = e
        print("gen round %d: %d tried, %d ok, %d failed" % (rnd, len(todo), sum(1 for n in todo if status[n][0]), sum(1 for n in todo if not status[n][0])), flush=True)
        todo = [n for n in nxt if not status[n][0]]
    return status

# ------------------------------------------------------------------------------------------------------------- compile / diffemu / lint
def compile_one(src):
    """-> (obj, key, error text): like diffemu.compile_obj, but the whole cl output of a failure"""
    try: o, k = diffemu.compile_obj(src); return o, k, ""
    except Exception as e:
        d = os.path.dirname(src); w = tempfile.mkdtemp(prefix="t2c_", dir=core.cmpfn.WORKROOT)
        try:
            for x in os.listdir(d):
                if x.endswith(".h"): shutil.copy(os.path.join(d, x), w)
            shutil.copy(src, os.path.join(w, "f.c"))
            r = subprocess.run([core.cmpfn.VC7, w, "cl.exe"] + diffemu.FLAGS + (["/GS"] if gsfix.GS_MARK in open(src).read() else []) + ["f.c"], capture_output=True, text=True, timeout=300)
            out = r.stdout + r.stderr
        finally: shutil.rmtree(w, ignore_errors=True)
        errs = [l.strip() for l in out.splitlines() if re.search(r"error C\d+|fatal error", l)]
        return None, None, "; ".join(errs[:3])[:400] or str(e)[:300]

def strict_out(src):
    """cl output (warnings level 3) of the file with the diffemu flags (object discarded)"""
    d = os.path.dirname(src); w = tempfile.mkdtemp(prefix="t2s_", dir=core.cmpfn.WORKROOT)
    try:
        for x in os.listdir(d):
            if x.endswith(".h"): shutil.copy(os.path.join(d, x), w)
        shutil.copy(src, os.path.join(w, "f.c"))
        r = subprocess.run([core.cmpfn.VC7, w, "cl.exe"] + diffemu.FLAGS + ["/W3"] + (["/GS"] if gsfix.GS_MARK in open(src).read() else []) + ["f.c"], capture_output=True, text=True, timeout=300)
        return r.stdout + r.stderr
    finally: shutil.rmtree(w, ignore_errors=True)
def strict_one(src, fixit=False):
    """the main gate's strict build rejects C4047 / C4022 / C4024 / C4133 (pointer level / type mismatches of arguments; round 1: candidates passed diffemu but failed there).
    -> '' when clean, else the first warnings; fixit: up to 3 rounds of casts to the parameter types (tools/strictfix.py) first"""
    for rnd in range(4 if fixit else 1):
        w = strictfix.parse(strict_out(src))
        if not w: return ""
        if not fixit or rnd == 3 or not strictfix.fix(src, w): break
    return "; ".join("f.c(%d) : %s" % (k, v[0][:110]) for k, v in sorted(w.items())[:3])

def dgroup(txt):
    """defect class guess of a diffemu divergence"""
    if "call trace differs" in txt:
        if re.search(r"\(\), \(\)|\(\) ours \(", txt) or "calls" in txt and re.search(r"orig (\d+) calls, ours (?!\1 )", txt): return "call-trace-count"
        return "call-trace-args"
    if re.search(r"\bEAX\b|return value|\beax\b", txt): return "return-value"
    if "ST0" in txt or "st0" in txt or "x87" in txt: return "float-result"
    if "esp" in txt: return "stack-delta (ret N / convention)"
    if "memory" in txt: return "memory-writes"
    return "other-diff"

CACHE = os.path.join(W, "cache.jsonl")
def load_cache():
    c = {}
    for cf in (P("output", "t2_auto", "cache.jsonl"), CACHE):      # the shared cache (earlier runs, content addressed) is only read
        if os.path.exists(cf):
            for l in open(cf):
                try:
                    r = json.loads(l)
                    if cf == CACHE or r["row"]["verdict"] == "EQUAL": c[r["key"]] = r["row"]      # a shared DIFF row has no diff text in this run's dir
                except Exception: pass
    return c
def run_diffemu(items, cases, force):
    """items: [(name, src, obj, objkey)] -> {name: row}"""
    cache = {} if force else load_cache(); res = {}; todo = []
    for n, src, obj, ok_ in items:
        key = core.sha(ok_, n, cases, 2, diffemu.TOOLHASH, core.T().hash)
        if key in cache: res[n] = cache[key]
        else: todo.append((n, src, obj, cases, 2, 90, key))
    print("diffemu: %d functions, %d cached" % (len(items), len(items) - len(todo)), flush=True)
    if todo:
        keys = {t[0]: t[6] for t in todo}
        with Pool(min(int(os.environ.get("T2_JOBS", "3")), len(todo))) as p, open(CACHE, "a") as cf:
            for row in p.imap_unordered(diffemu.job, [t[:6] for t in todo]):
                res[row["name"]] = row; cf.write(json.dumps(dict(key=keys[row["name"]], row=row)) + "\n"); cf.flush()
                print("  %-9s %-40s %s cases %s cov %s/%s" % (row["verdict"], row["name"], "", row["cases"], row["coverage_orig"], row["coverage_ours"]), flush=True)
    return res
def diff_text(n):
    p = os.path.join(diffemu.DIFFDIR, n + ".txt")
    return open(p).read() if os.path.exists(p) else ""
def passes(row): return row["verdict"] == "EQUAL" and int(row["cases"]) >= 20 and float(row["coverage_orig"]) >= 0.8 and float(row["coverage_ours"]) >= 0.8

_LINT = []
def lint(path, n):
    """defectlint findings of the file (in process, output/defectlint.csv untouched), cheatgrep hits, register callee warnings -> [text]"""
    if not _LINT:
        defectlint.load_gt(); _LINT.append(defectlint.load_decls())
        defectlint.ENABLED.update(l.strip() for l in open(P("src", "bridges", "enabled.txt")))
    t = open(path, errors="replace").read(); out = []
    for f in defectlint.Lint(os.path.relpath(path, G), t, _LINT[0]).run():
        if f["cls"] == 4 and f.get("symbol") in gsfix.ARITY: continue      # cdecl_args.txt counts the pushes of the caller's frame: RwStreamClose "takes 19" (it takes 2, gsfix.ARITY)
        out.append("defectlint class %s %s line %s: %s" % (f["cls"], f["sev"], f["line"], f.get("note") or f.get("text")))
    for k, fn, ln in cheatgrep.scan(path):
        if k == "asm" and (fn.endswith("_r") or re.search(r"__declspec\(naked\) .*\b\w+_r\(", "\n".join(t.split("\n")[max(0, ln - 12):ln]))): continue      # bridgegen F_r call adapters (naked, generated; static ones are not seen as functions by rawaddr: fn is empty)
        out.append("cheatgrep %s line %d in %s" % (k, ln, fn or "-"))
    return out

# ------------------------------------------------------------------------------------------------------------- failure files
HINTS = {
    "compile:struct-member": ["a field that Ghidra names is missing in src/auto/ghidra_types.h: tools/struct_gaps.py <failed.txt> adds field_0xNN out of `_gap_` padding, or cast: *(T *)((char *)p + off)", "keep every copy of ghidra_types.h under src/ identical", "never edit the original function name / signature"],
    "compile:undeclared": ["extern the global with its real element type next to the function (`extern float g;`) and check the width in names/MFL_global_widths.txt", "enum names (`WEAPON_MINE`) = their integer value from names/MFL_enums.txt", "`uint3` / `PTR_` / `UNK_` names: char array or the type of its use"],
    "compile:call-arity": ["Ghidra lost the arguments of a tail call / unprototyped callee: look at the ORIGINAL pushes (call_pushmin below) and pass the caller's own parameters", "declare the callee with that many parameters (names/MFL_callee_protos.txt)", "a callee that takes a register input is a usercall: stop, report it"],
    "compile:cast": ["Ghidra casts a float OBJECT to a pointer: copy the bits `*(T **)&x`, never `(T *)x`", "`(float)u` of an undefined4 converts, the original copies bits: `*(float *)&x`", "pointer arithmetic on (char *) not (int)"],
    "compile:syntax": ["VC7 is C89: declarations at the TOP of a block, no `//` after code, no C99 for-declarations", "keep the top-level typedefs of the file", "int-bit-pattern float literals: write the dword (`*(undefined4 *)&f = 0x3f800000`)"],
    "compile:other": ["compile the file alone: python3 tools/diffemu/diffemu.py --src <file>", "VC7 C89: declarations at block top", "check the `extern` lines against output/decls.h"],
    "diffemu:call-trace-args": ["a `()` callee pushes N dwords in the original (call_pushmin / cdecl_args below): the C call must pass the same number, in the same order", "a float argument to a `()` or undefined4 prototype is pushed as a double: pass the bits `*(int *)&f`", "stdcall callee (`ret N`): declare __stdcall, no `add esp`"],
    "diffemu:call-trace-count": ["a call is missing / extra: compare the branch structure with the original; a loop condition inverted or `a < b != (a == b)` is `<=`", "signed vs unsigned compare of an undefined4 (jb vs jl): cast `(int)`", "check early returns"],
    "diffemu:return-value": ["return type / width: byte / short results need the exact type (`ushort`), bool-as-int", "signed vs unsigned compare of an undefined4 global: `(int)g`", "float returned in ST0: declare `float` not `undefined4`"],
    "diffemu:float-result": ["`_ftol()` without argument or float stored into an int: `(int)expr`", "float literal as double vs float (suffix f)", "constant is an int bit pattern: `*(float *)&k`"],
    "diffemu:memory-writes": ["a store width differs (byte vs dword): look at the original's `mov byte/word/dword [..]`", "float stored into int object converts, original copies bits: `*(float *)&x = ..`", "array stride / struct offset: compare the displacement with the original"],
    "diffemu:stack-delta (ret N / convention)": ["the original is `ret N` (stdcall): declare the function __stdcall with N/4 parameters", "or it is cdecl but ours pops: remove __stdcall", "usercall (register input) -> stop and report"],
    "diffemu:other-diff": ["read the diverging case, compare the first differing observable with the original disassembly (tools/check_one.py)", "signed / unsigned undefined4 compares, float vs int copies", "keep the function name and signature"],
    "diffemu:INCONCLUSIVE": ["the emulation of the original trapped or looped: see the note; often a global read as a pointer (the C dereferences a garbage value the original also reads)", "unresolved relocation / unsupported instruction = harness limit: report, do not change the C", "check the parameter count against output/sigs.txt"],
    "diffemu:lowcov": ["EQUAL but a branch was never reached: cases cannot cover it; re-read the C for that branch against the disassembly instead", "otherwise report as low coverage", "do not change the C without a reason"],
    "lint": ["defectlint: unsigned compare of undefined4 -> `(int)`; float / int bit copies; `()` prototype with fewer arguments than the original pushes", "see README `defectlint` classes", "re-run: python3 tools/defectlint.py <file>"],
    "cheat": ["no __asm / #pragma / volatile / raw 0x4xxxxx pointers / big constant arrays", "use the extern symbol of the data, not its address", "tools/rawaddr.py --rewrite <file>"],
    "ptrcast": ["ptrcast.rewrite changed the text and the rewritten version is not EQUAL: report the exact rewrite (diff the two files) as a ptrcast.py bug, keep the un-rewritten C", "or fix by hand with typed byte pointers", "re-run diffemu --src"],
    "gen-skip": ["not scriptable: register inputs / labels / unprototyped callee need the usercall or cluster route", "see tools/usercall_adapters.py, diffemu --cluster", "report only"],
}
def hints_for(cls):
    for k in HINTS:
        if cls == k or cls.startswith(k + ":") or cls.startswith(k): return HINTS[k]
    return HINTS["compile:other"]
def cclass(err):
    for pat, c in ((r"is not a member", "compile:struct-member"), (r"C2065", "compile:undeclared"), (r"C2198|C2064|too (?:few|many) actual", "compile:call-arity"), (r"C2440|C2120|C2036|C2100|C2109|C2223|C2224", "compile:cast"),
                   (r"C2143|C2059|C2146|C2371|C2373|C2086|C2275|C2061|C2105", "compile:syntax")):
        if re.search(pat, err): return c
    return "compile:other"

_PM = {}
def pushmin():
    if not _PM:
        for f, nm in (("call_pushmin.txt", "min"), ("cdecl_args.txt", "cdecl")):
            for l in open(P("output", f)):
                p = l.split()
                if len(p) >= 2: _PM.setdefault(p[0], {})[nm] = " ".join(p[1:])
    return _PM
_RG = {}
def regcallees():
    if not _RG:
        for f in ("reginputs.csv", "reginputs2.csv"):
            for r in csv.DictReader(open(P("output", f))):
                if r["reg_inputs"].strip("|"): _RG[r["name"]] = (r["addr"], r["reg_inputs"].strip("|"))
    return _RG
def write_failure(n, va, size, stage, cls, detail, path):
    L = ["# %s @ %s (%s bytes) -- stage %s, class %s" % (n, va, size, stage, cls)]
    if path: L.append("C file: %s  (headers next to it; check: python3 tools/diffemu/diffemu.py --src %s ; flags cl /nologo /c /Gy /O2 /Ob0)" % (os.path.relpath(path, G), os.path.relpath(path, G)))
    L.append("Error / diverging case:")
    L += ["  " + x[:240] for x in detail.strip().splitlines()[:7]]
    body = open(path, errors="replace").read() if path else ""
    calls = sorted(set(re.findall(r"\b(\w+) ?\(", re.sub(r"^\s*extern.*$", "", body, flags=re.M))) - {n, "if", "while", "for", "switch", "return", "sizeof"})
    ext = [l.strip() for l in body.splitlines() if l.startswith(("extern", "/*")) and "(" in l and any(re.search(r"\b%s\(" % re.escape(c), l) for c in calls)][:8]
    if ext: L.append("Callee prototypes:"); L += ["  " + x[:200] for x in ext]
    pm = []
    for l in ext:
        m = re.match(r"/\*([0-9a-f]{8})\*/.*?(\w+)\(\)", l)
        if m and m.group(1) in pushmin(): pm.append("  %s %s: original pushes (call_pushmin min / sites; cdecl_args args sites) = %s" % (m.group(2), m.group(1), " ".join("%s=%s" % kv for kv in pushmin()[m.group(1)].items())))
    if pm: L.append("`()` callees, original pushes:"); L += pm[:6]
    rg = [(c, regcallees()[c]) for c in calls if c in regcallees()]
    if rg: L.append("WARNING register callees (called plain from C = wrong, usercall route needed): " + ", ".join("%s@%s regs %s" % (c, a, r) for c, (a, r) in rg[:4]))
    L.append("Recurring fixes for this class:"); L += ["  %d. %s" % (i + 1, h) for i, h in enumerate(hints_for(cls)[:3])]
    L.append("Rules: change ONLY this function, keep its name; no __asm/#pragma/volatile/raw addresses; no git; no Ghidra / game runs; report EQUAL or the new first divergence.")
    os.makedirs(FAILS, exist_ok=True); open(os.path.join(FAILS, n + ".md"), "w").write("\n".join(L[:40]) + "\n")
    os.makedirs(os.environ["ATTEMPTS_DIR"], exist_ok=True)      # attempts note of the run (tools/attempts.py format), merged by --publish
    with open(os.path.join(os.environ["ATTEMPTS_DIR"], n + ".md"), "a") as f: f.write("%s | t2_auto | NO | %s: %s\n" % (time.strftime("%F %H:%M"), cls, " ".join(detail.split())[:200]))

# ------------------------------------------------------------------------------------------------------------- main
def main():
    a = sys.argv[1:]; os.makedirs(FILES, exist_ok=True); os.makedirs(CAND, exist_ok=True); os.makedirs(FAILS, exist_ok=True)
    cases = int(a[a.index("--cases") + 1]) if "--cases" in a else 100; force = "--force" in a
    text_path = a[a.index("--text") + 1] if "--text" in a else NAMED
    rows = select(a); funcs = load_funcs(text_path); sym = core.T().sym
    fsz = {}
    for l in open(P("output", "fnsizes.txt")):
        q = l.split()
        if len(q) == 3: fsz[q[1]] = int(q[2])
    size = {n: s or fsz.get(n, 0) for s, n, _ in rows}; va_of = {n: v or ("%08x" % min(sym[n]) if n in sym else "") for _, n, v in rows}
    res = {}      # name -> dict(stage, verdict, detail, cls, file)
    def put(n, stage, verdict, detail, cls, f=""): res[n] = dict(name=n, va=va_of.get(n, ""), bytes=size.get(n, 0), stage=stage, verdict=verdict, detail=" ".join(detail.split())[:300], cls=cls, file=os.path.relpath(f, G) if f else "")
    # --- selection / skips
    regin = {r["name"]: r["reg_inputs"].strip("|") for r in csv.DictReader(open(P("output", "reginputs.csv"))) if r["reg_inputs"].strip("|")}
    have = {l.split()[1] for l in open(P("src", "manifest.txt")) if len(l.split()) > 3 and l.split()[0] not in ("#", "(none)")}
    import callee_auto      # exact prototypes for the unprototyped FUN_ callees of the pool (names/MFL_callee_protos_auto.txt)
    ap, WHY_ = callee_auto.build(funcs); callee_auto.save(ap); WHY.update(WHY_)      # T2_OVERLAY_DIR: into this run's overlay
    protos = {"FUN_" + v for v in callee_auto.protoed()}
    first = {}
    for l in open(P("src", "symbols.txt")):
        p = l.split()
        if len(p) == 2: first.setdefault(p[0], []).append(p[1])
    done = {l.strip() for l in open(os.path.join(SHARED_CAND, "names.txt"))}
    todo = []; nodec = []; dfd = defined_elsewhere()
    for _, n, _v in rows:
        if n in done: put(n, "done", "ALREADY-CANDIDATE", "already in output/tier2_candidates2/names.txt", "done"); continue
        if n in dfd: put(n, "done", "ALREADY-DEFINED", "defined in hand written / installed C (src/*.c, bridges, tier1, automatch*, automatch_agent, automatch_near)", "done"); continue
        if n not in funcs: put(n, "gen", "NO-DECOMPILATION", "no Ghidra C for this function in %s (FUN_ functions are not exported)" % os.path.basename(text_path), "gen-skip:no-decomp"); nodec.append(n); continue
        sc = skip_class(n, funcs[n][1], regin, have, protos)
        if sc: put(n, "gen", "SKIP", sc[1], sc[0]); continue
        if len(first.get(n, [])) != 1: put(n, "gen", "SKIP", "name at %d addresses in symbols.txt" % len(first.get(n, [])), "gen-skip:dup-symbol"); continue
        todo.append(n)
    open(os.path.join(W, "need_decomp.txt"), "w").write("".join("%s %s %s\n" % (va_of[n], size.get(n, 0), n) for n in nodec))
    print("pool %d: %d without decompilation, %d skipped, %d to generate" % (len(rows), len(nodec), sum(1 for r in res.values() if r["verdict"] == "SKIP"), len(todo)), flush=True)
    # --- generate
    subprocess.run([sys.executable, os.path.join(HERE, "spec_header.py"), P("src", "auto", "ghidra_types.h"), HDR], capture_output=True)      # struct specs newer than the exported header (autocompile reads it from the overlay)
    # arrays of structs of the struct specs whose symbol decls.h still declares as a scalar (`extern undefined4 g_voices;`): the named C indexes them (`g_voices[i].voice`) -> override declaration in the overlay
    dec = open(P("output", "decls.h"), errors="replace").read(); fx = os.path.join(W, "overlay", "MFL_extern_fix.txt"); ov = []; nm_at = collections.defaultdict(list)
    for l_ in open(P("src", "symbols.txt")):
        q = l_.split()
        if len(q) == 2: nm_at[int(q[1], 16)].append(q[0])
    for f_ in glob.glob(P("names", "MFL_structs*.txt")):
        for l_ in open(f_):
            q = l_.strip().split(",")
            if q[0] == "apply" and len(q) >= 4 and int(q[3]) > 1:
                ov += ["extern struct %s %s[%s];" % (q[2], nm, q[3]) for nm in nm_at[int(q[1], 16)] if re.search(r"extern (?:undefined4|undefined|int|uint) %s;" % nm, dec)]
    have_ = open(fx).read() if os.path.exists(fx) else ""
    if [l_ for l_ in ov if l_ not in have_]: open(fx, "a").write("".join(l_ + "\n" for l_ in ov if l_ not in have_))
    st = generate(todo, funcs, text_path) if todo else {}
    shutil.copy(HDR, os.path.join(FILES, "ghidra_types.h")); shutil.copy(P("src", "auto", "prelude.h"), os.path.join(FILES, "prelude.h"))
    items = []; srcs = {}
    for n in todo:
        ok, f, err, rnd = st[n]
        if f:
            shutil.copy(f, os.path.join(FILES, n + ".c")); srcs[n] = os.path.join(FILES, n + ".c")
            try: pk = t2_postfix.fix(srcs[n])      # float bit copies, signed compares, enum defines (tools/t2_postfix.py)
            except Exception as ex: pk = []; print("postfix %s: %s" % (n, ex))
            if pk and not ok:      # a compile error may be gone now (enum names)
                o_, k_, e_ = compile_one(srcs[n])
                if not e_: st[n] = ok = True, f, "", rnd
                else: err = e_      # the error after the scripted fixes (not the stale gen error)
            if pk:
                print("postfix %s: %s" % (n, ",".join(sorted(set(pk)))), flush=True)
                os.makedirs(os.environ["ATTEMPTS_DIR"], exist_ok=True)      # attempts note (tools/attempts.py format)
                with open(os.path.join(os.environ["ATTEMPTS_DIR"], n + ".md"), "a") as af: af.write("%s | t2_postfix | NOTE | scripted text fixes applied before compile: %s\n" % (time.strftime("%F %H:%M"), ",".join(sorted(set(pk)))))
        if not ok: put(n, "gen", "COMPILE-ERROR", err, cclass(err), srcs.get(n, "")); continue
    # --- compile with the diffemu flags
    cand = [n for n in todo if st[n][0]]
    def comp1(n):
        r = compile_one(srcs[n])
        if r[2]: return r
        t0 = open(srcs[n], errors="replace").read()
        if gsfix.GS_MARK in t0:      # the buffer is split into scalars / small members: cl /GS emits no cookie check (or lays the frame out differently from the original: several buffers)
            t1 = gsfix.gsbuf(t0)
            if t1 != t0:
                open(srcs[n], "w").write(t1); r1 = compile_one(srcs[n])
                if r1[2] or b"__security_check_cookie" not in open(r1[0], "rb").read(): open(srcs[n], "w").write(t0)      # no gain: back
                else: r = r1
        t0 = open(srcs[n], errors="replace").read(); strict_one(srcs[n], True)      # casts to the parameter types so that the strict build accepts the file
        return compile_one(srcs[n]) if open(srcs[n], errors="replace").read() != t0 else r
    with ThreadPoolExecutor(2) as ex: comp = dict(zip(cand, ex.map(comp1, cand)))
    for n in cand:
        o, k, e = comp[n]
        if e: put(n, "compile", "COMPILE-ERROR", e, cclass(e), srcs[n])
        else: items.append((n, srcs[n], o, k))
    # --- diffemu
    dres = run_diffemu(items, cases, force) if items else {}
    ok1 = []
    for n, src, o, k in items:
        r = dres[n]; txt = diff_text(n) if r["verdict"] == "DIFF" else ""
        if passes(r): ok1.append((n, src))
        elif r["verdict"] == "DIFF": put(n, "diffemu", "DIFF", (r["note"] + "\n" + txt)[:900], "diffemu:" + dgroup(txt + r["note"]), src)
        elif r["verdict"] == "EQUAL": put(n, "diffemu", "LOWCOV", "EQUAL %s cases, coverage %s/%s; %s" % (r["cases"], r["coverage_orig"], r["coverage_ours"], r["note"]), "diffemu:lowcov", src)
        else: put(n, "diffemu", "INCONCLUSIVE", r["note"], "diffemu:INCONCLUSIVE", src)
    # --- ptrcast / rawaddr rewrite + re-verify, then lint (cheatgrep sees the rewritten raw addresses)
    ok2 = list(ok1); rg = regcallees(); rew = {}
    for n, src in ok2:
        t = open(src, errors="replace").read(); shutil.copy(src, os.path.join(W, n + ".orig.c")); nt = ptrcast.rewrite(t)
        if nt != t: open(src, "w").write(nt)
        subprocess.run([sys.executable, os.path.join(HERE, "rawaddr.py"), "--rewrite", src], capture_output=True)
        rew[n] = open(src, errors="replace").read() != t
    chg = [(n, src) for n, src in ok2 if rew[n]]
    items2 = []
    with ThreadPoolExecutor(2) as ex: comp2 = dict(zip([n for n, _ in chg], ex.map(lambda x: compile_one(x[1]), chg)))
    d2 = {}
    for n, src in chg:
        o, k, e = comp2[n]
        if e: put(n, "ptrcast", "COMPILE-ERROR", "after ptrcast/rawaddr rewrite: " + e, "ptrcast", src); shutil.copy(os.path.join(W, n + ".orig.c"), src)
        else: items2.append((n, src, o, k))
    if items2: d2 = run_diffemu(items2, cases, force)
    for n, src, o, k in items2:
        r = d2[n]
        if not passes(r): put(n, "ptrcast", "DIFF-AFTER-REWRITE", "rewritten text: %s %s cases cov %s/%s %s" % (r["verdict"], r["cases"], r["coverage_orig"], r["coverage_ours"], r["note"]), "ptrcast", src); shutil.copy(os.path.join(W, n + ".orig.c"), src)
    passed = []
    for n, src in ok2:
        if n in res: continue
        probs = lint(src, n); body = open(src, errors="replace").read()
        rc = [c for c in set(re.findall(r"\b(\w+) ?\(", re.sub(r"^extern void \w+\(void\);$", "", body, flags=re.M))) if c in rg and c != n]      # the F_r adapters' own `extern void F(void);` is no plain call
        if rc: probs.append("register callee called plain: " + ",".join("%s@%s regs %s" % (c, rg[c][0], rg[c][1]) for c in rc))
        if not probs and (se := strict_one(src)): probs.append("strict build: " + se)
        if probs: put(n, "lint", "LINT", "\n".join(probs), "cheat" if probs[0].startswith("cheatgrep") else "lint", src); continue
        passed.append((n, src))
    # --- candidates
    nxt = 1; batches = []; names_txt = []
    for n, src in list(passed):      # candidate whose function is defined elsewhere (hand written / installed / another candidate): dropped with a note
        dup = n in defined_elsewhere() or n in done
        if dup: passed.remove((n, src)); put(n, "done", "ALREADY-DEFINED", "dropped at the candidate stage: %s is defined elsewhere" % n, "done", src)
    if "--no-candidates" not in a:
        for i, (n, src) in enumerate(sorted(passed, key=lambda x: va_of.get(x[0], ""))):      # numbering local to the run; --publish renumbers
            shutil.copy(src, os.path.join(CAND, "an_hk_%03d.c" % (nxt + i))); names_txt.append(n)
            put(n, "pass", "PASS", "an_hk_%03d.c" % (nxt + i), "pass", src)
        open(os.path.join(CAND, "names.txt"), "w").write("".join(n + "\n" for n in names_txt))
        batches = [(nxt + i, min(nxt + i + 39, nxt + len(passed) - 1)) for i in range(0, len(passed), 40)]
    else:
        for n, src in passed: put(n, "pass", "PASS", "(not copied)", "pass", src)
    # --- failure files, csv, summary
    for n, r in res.items():
        if r["stage"] not in ("pass", "done") and r["cls"] != "gen-skip:no-decomp": write_failure(n, r["va"], r["bytes"], r["stage"], r["cls"], r["detail"], os.path.join(G, r["file"]) if r["file"] else "")
    out = sorted(res.values(), key=lambda r: -int(r["bytes"]))
    with open(os.path.join(RUN, "results.csv"), "w", newline="") as f:
        w = csv.DictWriter(f, ["name", "va", "bytes", "stage", "verdict", "detail", "cls", "file"]); w.writeheader(); w.writerows(out)
    summarize(out, batches)

COST = {"lint": 1.0, "cheat": 1.5, "ptrcast": 2.0, "compile:cast": 1.5, "compile:call-arity": 1.5, "compile:undeclared": 1.5, "compile:syntax": 1.2, "compile:struct-member": 2.0, "compile:other": 3.0,
        "diffemu:call-trace-args": 2.0, "diffemu:call-trace-count": 3.0, "diffemu:return-value": 2.0, "diffemu:memory-writes": 2.5, "diffemu:float-result": 2.5, "diffemu:lowcov": 4.0, "diffemu:INCONCLUSIVE": 4.0}
def summarize(out, batches):
    L = []; tot = sum(int(r["bytes"]) for r in out)
    L.append("t2_auto: %d functions, %d bytes" % (len(out), tot))
    p = [r for r in out if r["stage"] == "pass"]
    L.append("PASS all stages: %d functions, %d bytes; candidate batches (an_hk numbers, <= 40 each): %s" % (len(p), sum(int(r["bytes"]) for r in p), ", ".join("%03d-%03d" % b for b in batches) or "-"))
    L.append("by stage reached / verdict:"); c = collections.Counter((r["stage"], r["verdict"]) for r in out); bt = collections.Counter()
    for r in out: bt[(r["stage"], r["verdict"])] += int(r["bytes"])
    for k, v in sorted(c.items(), key=lambda kv: -bt[kv[0]]): L.append("  %-9s %-20s %4d fns %7d B" % (k[0], k[1], v, bt[k]))
    L.append("failure classes (count / bytes):"); fc = collections.Counter(); fb = collections.Counter()
    for r in out:
        if r["stage"] != "pass": fc[r["cls"]] += 1; fb[r["cls"]] += int(r["bytes"])
    for k, v in sorted(fc.items(), key=lambda kv: -fb[kv[0]]): L.append("  %-34s %4d fns %7d B" % (k, v, fb[k]))
    fl = [r for r in out if r["stage"] not in ("pass", "done") and r["cls"] in COST]
    L.append("20 most promising failures for a Haiku agent (bytes / estimated cost, class cost table in t2_auto.py):")
    for r in sorted(fl, key=lambda r: -int(r["bytes"]) / COST[r["cls"]])[:20]: L.append("  %-40s %4s B %-26s %s" % (r["name"], r["bytes"], r["cls"], r["detail"][:90]))
    open(os.path.join(RUN, "summary.txt"), "w").write("\n".join(L) + "\n"); print("\n".join(L))

if __name__ == "__main__": main()
