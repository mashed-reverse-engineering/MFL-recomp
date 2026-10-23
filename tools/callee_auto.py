#!/usr/bin/env python3
"""callee_auto.py: exact-confidence prototypes for unprototyped FUN_ callees of a decompilation (used by t2_auto.py, writes names/MFL_callee_protos_auto.txt, read by autocompile.py).
build(funcs {name: (va, body)}) -> (protos {va: line}, reasons {FUN_va: why not}); only callees of the given bodies that have no prototype in names/MFL_callee_protos*.txt.
Exact means: orig_stackargs.txt row without tail / addr / dyn / ijmp / w* flags; no register inputs (reginputs*.csv); `ret N` => __stdcall with N/4 params (body reads <= N/4);
else __cdecl with n = body stack reads == minimum pushes of the callers (call_pushmin.txt); result `int` (`double` when st0 is returned and a caller casts it to float/double; edx without eax: skipped).
A parameter is `float` when the caller passes a float variable / (float) cast, else `int`; sites that disagree (or float-typed by some, int by others) => not exact."""
import collections, csv, os, re
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
P = lambda *a: os.path.join(G, *a)
RELAX = bool(os.environ.get("CALLEE_RELAX"))      # CALLEE_RELAX=1: prototypes of callees with tail / ijmp / addr flags or body reads != caller pushes too (names/MFL_callee_protos_auto.txt of the RUN overlay only; diffemu checks the argument count at every call)
OV = os.environ.get("T2_OVERLAY_DIR")      # a t2_auto run writes its prototypes / aliases below this dir (read in addition to the shared files); tools/t2_auto.py --publish merges them
AUTO = os.path.join(OV, "MFL_callee_protos_auto.txt") if OV else P("names", "MFL_callee_protos_auto.txt")
ALIASES = os.path.join(OV, "aliases.txt") if OV else P("src", "aliases.txt")
def _rows(f): return {l.split()[0]: l.split() for l in open(P("output", f)) if l.split()}
def protoed():
    s = set()
    for f in (P("names", "MFL_callee_protos.txt"), P("names", "MFL_callee_protos_auto.txt"), AUTO):
        if os.path.exists(f): s |= set(re.findall(r"^/\*([0-9a-f]{8})\*/ extern ", open(f).read(), re.M))
    return s
def split_args(s):
    out, d, cur = [], 0, ""
    for ch in s:
        d += {"(": 1, "[": 1, ")": -1, "]": -1}.get(ch, 0)
        if ch == "," and d == 0: out.append(cur.strip()); cur = ""
        else: cur += ch
    if cur.strip(): out.append(cur.strip())
    return out
_FP = {}
def float_protos():
    """callee name / FUN_va -> parameter type list, from output/decls.h, names/MFL_callee_protos*.txt and this run's overlay (tools/py)"""
    if not _FP:
        for f in (P("output", "decls.h"), P("names", "MFL_callee_protos.txt"), P("names", "MFL_callee_protos_auto.txt"), AUTO):
            if not os.path.exists(f): continue
            for l in open(f, errors="replace"):
                m = re.match(r"(?:/\*([0-9a-f]{8})\*/ )?extern [^()]*?(\w+)\(([^()]*)\);", l)
                if not m: continue
                ps = [] if m.group(3).strip() in ("", "void") else split_args(m.group(3))
                _FP[m.group(2)] = ps
                if m.group(1): _FP["FUN_" + m.group(1)] = ps
    return _FP
def float_literal_ok(c, x):
    """a call with float literal arguments is fine when the callee's prototype is known and each such parameter is float / double (the literal is then converted exactly as in the original; `f()` / int parameters would make an 8 byte double or a wrong int)"""
    lit = re.compile(r"(?<![\w.])\d+\.\d+(?![\w.])")
    if not lit.search(x): return True
    ps = float_protos().get(c)
    if ps is None or any(p.strip() == "..." for p in ps): return False
    for k, a in enumerate(split_args(x)):
        if not lit.search(a): continue
        if re.search(r"\w\s*\(", a) or k >= len(ps) or not re.match(r"(?:const )?(?:float|double|RwReal)\s*\w*$", ps[k].strip()): return False
    return True
def sites_of(funcs):
    S = collections.defaultdict(list)       # va -> [(args, floatvars-of-caller, cast_float_result)]
    for n, (_, body) in funcs.items():
        fv = set(re.findall(r"\bfloat\s+(\w+)\s*;", body)) | set(re.findall(r"\bfloat\s+\w+\s*,\s*(\w+)", body))
        for c in re.finditer(r"\bFUN_([0-9a-f]{8}) ?\(", body):
            i, d = c.end(), 1
            while i < len(body) and d: d += {"(": 1, ")": -1}.get(body[i], 0); i += 1
            S[c.group(1)].append((split_args(body[c.end():i - 1]), fv, bool(re.search(r"\((?:float10|double|float)\)\s*$", body[max(0, c.start() - 20):c.start()]))))
    return S
def build(funcs):
    so, pm = _rows("orig_stackargs.txt"), _rows("call_pushmin.txt"); ri = {}
    for f in ("reginputs.csv", "reginputs2.csv"):
        for r in csv.DictReader(open(P("output", f))): ri[r["addr"]] = r
    have = protoed(); protos, why = {}, {}
    for va, ss in sites_of(funcs).items():
        if va in have: continue
        nm = "FUN_" + va; r = ri.get(va)
        if r and r["reg_inputs"].strip("|"): why[nm] = "regin"; continue
        if va not in so: why[nm] = "no orig_stackargs row"; continue
        _, slots, retn, tail, flags = so[va][:5]; slots, retn = int(slots), int(retn)
        if (tail != "-" or flags != "-") and not RELAX: why[nm] = "orig flags tail=%s %s" % (tail, flags); continue
        if retn:
            if slots > retn // 4: why[nm] = "ret N < reads"; continue
            n, conv = retn // 4, "__stdcall "
        else:
            if va not in pm or (int(pm[va][1]) != slots and not RELAX): why[nm] = "pushes %s != body reads %d" % (pm.get(va, ["", "?"])[1], slots); continue
            n, conv = (int(pm[va][1]) if RELAX else slots), "__cdecl "      # RELAX: the callers' minimum pushes win; a wrong count shows up as a diffemu call-trace DIFF
        rr = (r["ret_regs"] if r else "").split("|")
        if "edx" in rr and "eax" not in rr: why[nm] = "edx-only result"; continue
        cf = any(x[2] for x in ss)
        if "st0" in rr and not cf and "eax" not in rr: why[nm] = "st0 result without a float caller"; continue
        ty = []; bad = False
        for k in range(n):
            ks = {("float" if re.search(r"\((?:float|double)\)", a[k]) or a[k] in fv or re.fullmatch(r"-?\d+\.\d+f?", a[k]) else "int") for a, fv, _ in ss if len(a) == n}
            if not ks: bad = True; break
            if len(ks) > 1: bad = True; break
            ty.append(ks.pop())
        if bad: why[nm] = "caller arg counts / float-int types disagree"; continue
        protos[va] = "/*%s*/ extern %s %s%s(%s);" % (va, "double" if "st0" in rr and cf else "int", conv, nm, ", ".join("%s a%d" % (t, i + 1) for i, t in enumerate(ty)) or "void")
    return protos, why
def save(protos):
    old = {}; os.makedirs(os.path.dirname(AUTO), exist_ok=True)
    if os.path.exists(AUTO):
        for l in open(AUTO):
            m = re.match(r"/\*([0-9a-f]{8})\*/ extern ", l)
            if m: old[m.group(1)] = l.rstrip("\n")
    old.update(protos)
    open(AUTO, "w").write("# generated by tools/callee_auto.py (exact-confidence prototypes of FUN_ callees, from orig_stackargs / call_pushmin / reginputs)\n" + "".join(v + "\n" for k, v in sorted(old.items())))
    al = open(ALIASES).read() if os.path.exists(ALIASES) else ""; known = {l.split()[0] for l in open(P("src", "aliases.txt")).read().splitlines() + al.splitlines() if l.split()} | {l.split()[0] for l in open(P("src", "symbols.txt")) if l.split()}
    add = "".join("FUN_%s %s\n" % (v, v) for v in protos if "FUN_" + v not in known)
    if add: open(ALIASES, "a").write(add if al.endswith("\n") or not al else "\n" + add)
