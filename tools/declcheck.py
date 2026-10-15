#!/usr/bin/env python3
"""declcheck.py: consistency of the declarations across our own translation units.  A function that WE define (any tier, hand written, bridges) is declared in other files by `extern` lines; if the calling
convention, the number of arguments or the size class of the return type differ between the definition and a declaration, the callers push / pop differently than the callee: byte identical callers still
crash (found with `copters_load`: matched the original byte for byte, but called our stdcall `load_copter_model` as cdecl).  Usage: declcheck.py   (prints the mismatches, exit 1 if any)"""
import glob, os, re, sys
HERE = os.path.dirname(os.path.abspath(__file__)); R = os.path.join(HERE, "..", "src")
files = [f for pat in ("*.c", "automatch/*.c", "automatch_agent/*.c", "automatch_near/*.c", "bridges/*.c") for f in glob.glob(os.path.join(R, pat))]
if os.environ.get("DECLCHECK_EXTRA"): files += glob.glob(os.path.join(os.environ["DECLCHECK_EXTRA"], "b*.c"))     # e.g. output/usercall_candidates (not part of src/)
def split_params(s):
    s = s.strip()
    if s in ("", "void"): return []
    out, d, cur = [], 0, ""
    for ch in s:
        if ch == "(": d += 1
        if ch == ")": d -= 1
        if ch == "," and d == 0: out.append(cur); cur = ""
        else: cur += ch
    return out + [cur]
def sizeclass(r):
    r = re.sub(r"\b(const|volatile|struct|__declspec\(\w+\)|extern)\b", "", r).strip()
    if "*" in r: return "ptr"
    if r in ("void",): return "void"
    if r in ("float", "double", "float10"): return "fp"
    if r in ("undefined", "byte", "char", "uchar", "undefined1", "bool", "BOOL8"): return "byte"
    return "int"
def parse(sig):
    sig = re.sub(r"__declspec\(\w+\)\s*", "", sig)          # dllimport / noinline / noreturn are not part of the type
    m = re.match(r"^(?:extern\s+)?(?:/\*\w+\*/\s*)?(.*?)\b(__cdecl|__stdcall|__fastcall)?\s*\b(\w+)\((.*)\)\s*;?$", sig.strip())
    if not m: return None
    ret, conv, name, params = m.group(1), m.group(2) or "__cdecl", m.group(3), m.group(4)
    return name, conv, sizeclass(ret), split_params(params), params.strip() == ""
defs, decls = {}, {}
bodies = {}      # file -> text of everything that is not an extern line: a mismatching declaration only matters when a function of that file calls the name
for f in files:
    L = open(f, errors="replace").read().split("\n")
    bodies[os.path.relpath(f, R)] = "\n".join(x for x in L if not re.match(r"^(/\*\w+\*/ )?extern", x))
    for i, l in enumerate(L):
        if re.match(r"^\s", l) or l.startswith(("#", "/*", "}", "{", "typedef", "static")): 
            if not l.startswith("/*"): continue
        if l.startswith("extern") or re.match(r"^/\*\w+\*/ extern", l):
            p = parse(l)
            if p and "(" in l: decls.setdefault(p[0], []).append((os.path.relpath(f, R), p))
        elif re.match(r"^[A-Za-z_][\w \*]*\w\(.*\)$", l) and "{" in "".join(L[i + 1:i + 3])[:3]:
            p = parse(l)
            if p: defs.setdefault(p[0], []).append((os.path.relpath(f, R), p))
bad = 0
for name, dl in sorted(defs.items()):
    for df, dp in dl[:1]:
        for xf, xp in decls.get(name, []):
            if not re.search(r"\b%s\(" % re.escape(name), bodies.get(xf, "")): continue      # declared but never called in that file
            if xp[4]:                                           # unprototyped `f()`: cl assumes cdecl, a stdcall definition pops its own arguments -> stack imbalance
                if dp[1] == "__stdcall" and xp[1] != "__stdcall" and dp[3]: bad += 1; print("%-34s declared in %-34s (unprototyped, cdecl assumed) defined %s in %s" % (name, xf, dp[1], df))
                continue
            why = []
            if xp[1] != dp[1]: why.append("convention %s vs %s" % (xp[1], dp[1]))
            if len(xp[3]) != len(dp[3]): why.append("%d args vs %d" % (len(xp[3]), len(dp[3])))
            if xp[2] != dp[2] and "void" not in (xp[2], dp[2]): why.append("return %s vs %s" % (xp[2], dp[2]))
            hard = xp[1] != dp[1] or len(xp[3]) < len(dp[3])      # warnings: more arguments pushed than read (cdecl: harmless), return size class
            if why: bad += hard; print("%s %-34s declared in %-34s (%s) defined in %s" % ("HARD" if hard else "warn", name, xf, "; ".join(why), df))
# 2. functions of the NOT byte identical tiers (automatch_near) that call a register convention function (output/reginputs.csv) as a plain C call: the caller pushes the argument, the callee reads a register
import csv
reg = {r["name"] for r in csv.DictReader(open(os.path.join(HERE, "..", "output", "reginputs.csv"))) if r["reg_inputs"]}
regcallers = []
for f in glob.glob(os.path.join(R, "automatch_near", "an_*.c")):
    t = open(f, errors="replace").read()
    for m in re.finditer(r"^([A-Za-z_][\w \*]*?)\b(\w+)\([^;{]*\)\s*\n\s*\{", t, re.M):
        i, depth = m.end(), 1
        while depth and i < len(t): depth += (t[i] == "{") - (t[i] == "}"); i += 1
        c = [x for x in set(re.findall(r"\b(\w+)\(", t[m.end():i])) if x in reg and x != m.group(2)]
        if c: regcallers.append(m.group(2)); bad += 1; print("%-34s tier 2 function calls register convention function(s) as plain C: %s" % (m.group(2), ", ".join(sorted(c))))
open(os.path.join(HERE, "..", "output", "t2_regcallers.txt"), "w").write("\n".join(sorted(regcallers)) + "\n")
print(bad, "hard mismatches")
sys.exit(1 if bad else 0)
