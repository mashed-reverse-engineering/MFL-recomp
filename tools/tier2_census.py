#!/usr/bin/env python3
"""tier2_census.py: every game function that is NOT yet C (tier_ledger.csv) with the reason it is not tier 2 yet, no game run.
Usage: tier2_census.py <autocompile dir> <automatch.csv> [--build]
  <autocompile dir> + <automatch.csv>: output of an existing `autocompile.py` / `automatch.py` run (autoloop.sh rN/ac, rN/automatch.csv); NOTHING is recompiled here (object cache = that run).
                    Without them: run `autoloop.sh <dir> 1` first.
  --build:          also promote the compiled-not-identical functions into output/tier2_candidates/ (PROMOTE_OUT, src/ untouched), build an overlay exe from them
                    (T2_SHADOW_DIR=..., RELCHECK=warn, cached objects) and run staticcheck.py with STRONG=1 on it (a build, no game run); without --build the static column stays empty.
Output: output/tier2_census.csv (name, va, size, compiles, static_ok, blocker, bytes, detail) + summary (counts and bytes per blocker class, biggest first).
Blocker classes (first that applies): ledger-done / thunk / usercall (register inputs: reginputs.csv or decompiler pseudo registers) / unresolved-call (FUN_xxxx callee without prototype) /
plain-regcallee[-noadapter] (cdecl function with reg_e?? locals, calls register functions through F_r adapters; tools/usercall_adapters.py) / x87-intrinsic (fsin/fcos/fpatan/ABS only, AC_X87=1) /
label (LAB_ goto / jump table) / float-literal (unprototyped call with float literal) / compile-fail:<pattern> / unresolved-symbol / reg-callee (calls a register convention function) /
byte-match-uninstalled (tier 1 candidate) / static-fail / candidate-OK (compiles, passes staticcheck) / candidate-unchecked (compiles, --build not run)."""
import csv, collections, glob, os, re, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__)); G = os.path.join(HERE, "..")
args = [a for a in sys.argv[1:] if not a.startswith("--")]
ac, amcsv = args[0], args[1]
BUILD = "--build" in sys.argv
CAND = os.path.join(G, "output", "tier2_candidates")      # final candidate set (top N, an_*.c like src/automatch_near)
WORK = os.path.join(G, "output", "tier2_census_build")      # promote / build / staticcheck of all compiled candidates
TOP = int(os.environ.get("TOP", "40"))

_cur, _cp = os.path.join(G, "src", "auto", "ghidra_types.h"), os.path.join(ac, "ghidra_types.h")
if not os.path.exists(_cp) or open(_cp, "rb").read() != open(_cur, "rb").read(): print("WARNING: %s is not the current src/auto/ghidra_types.h (stale round: compile-fail counts are too high; rerun autocompile.py, it refreshes the header itself)" % _cp, file=sys.stderr)
ledger = {r["name"] for r in csv.DictReader(open(os.path.join(G, "output", "tier_ledger.csv")))}
own = {}
for r in csv.DictReader(open(os.path.join(G, "output", "delink_sdk", "ownership_items.csv"))):
    if r["kind"] == "code" and r["owner"] == "game": own[r["name"]] = (r["lo"], int(r["hi"], 16) - int(r["lo"], 16))
regin = {}
for l in open(os.path.join(G, "output", "reginputs.csv")).read().splitlines()[1:]:
    p = l.split(",")
    if len(p) >= 3 and p[2].strip("|"): regin[p[0]] = p[2].strip("|")
REG = re.compile(r"\b(reg_e(?:ax|bx|cx|dx|si|di|bp)|in_[A-Z]{2,3}|unaff_[A-Za-z]+|extraout_\w+|stack0x\w+|param_\d+_00\w+|unaff_retaddr|[iu]?Stack[0-9a-f]{8}|in_stack_\w+|POPCOUNT|SBORROW\d|CARRY\d|halt_baddata|ABS|fpatan|fsin|fcos|fptan|fyl2x|f2xm1)\b")   # same as autocompile.py
text = open(os.path.join(G, "output", "named", "MFL_app_named.c"), errors="replace").read()
funcs = {}
for m in re.finditer(r"// ==== (\S+) @ ([0-9a-f]+)\n(.*?)(?=\n// ==== |\Z)", text, re.S):
    funcs.setdefault(m.group(1), (m.group(2), __import__("gsfix").strip(m.group(3))))
ok = {}
if os.path.exists(os.path.join(ac, "addrs.txt")):
    for l in open(os.path.join(ac, "addrs.txt")): n, a = l.split(); ok[n] = a
failed = dict(l.rstrip("\n").split("\t", 1) for l in open(os.path.join(ac, "failed.txt")) if "\t" in l)
am = {}
for r in csv.reader(open(amcsv)):
    if len(r) >= 4: am[r[1]] = (r[0], r[2], r[3])

def errclass(e):
    for pat, c in ((r"is not a member", "struct-member"), (r"C2065", "undeclared-id"), (r"C2371|C2373|C2381|C2086|redefinition", "redeclaration"), (r"C2440|C2446|C2450|C2059|C2106|C2064|C2036|C2143|C2146|C2148|C2275", "type/syntax"),
                   (r"C2198|C2064|C2065", "call"), (r"C2037|C2039", "struct-member"), (r"unattributed", "unattributed"), (r"C2137|C2078|C2117|C2057|C2133", "decl-shape")):
        if re.search(pat, e): return c
    return "other"

import bridgegen as bg      # F_r call adapters: which register callees can a plain C function call
sys.path.insert(0, HERE)
_bl, _sg, _ri, _man = bg.load()
_ad = sorted(a for a, _ in _bl.values()); _nx = {a: _ad[i + 1] if i + 1 < len(_ad) else a + 0x400 for i, a in enumerate(_ad)}
ADAPTER = bg.adapter_ok(_bl, _sg, _ri, _man, _nx)
X87 = {"ABS", "fpatan", "fsin", "fcos"}      # x87 intrinsics that tools/autocompile.py AC_X87=1 writes as inline-asm helpers

def regcallee(body):
    return sorted({n for n in set(re.findall(r"\b([A-Za-z_]\w*)\s*\(", body)) if n in regin})

bad = {l.split(None, 1)[0]: l.split(None, 1)[1].strip() for l in open(os.path.join(G, 'output', 'tier2_known_bad.txt')) if l.strip()}      # functions that failed a sweep / shadow test as tier 2 earlier
nodec = [0, 0]
inst = set()      # functions defined in src/automatch_near/*.c (the names list there is stale)
for g_ in glob.glob(os.path.join(G, "src", "automatch_near", "an_*.c")):
    inst |= set(re.findall(r"^[A-Za-z_][\w\s\*]*?\b(\w+)\s*\([^;{}]*\)\s*\n\s*\{", open(g_, errors="replace").read(), re.M))
CALLEE_OK = set()      # FUN_ callees that have a prototype in names/MFL_callee_protos.txt (hand named ones are renamed, so they never show up as FUN_)
if "callees" in os.environ.get("FIX_RULES", "callees").split(",") and os.path.exists(os.path.join(G, "names", "MFL_callee_protos.txt")):
    CALLEE_OK = {"FUN_" + m.group(1) for m in re.finditer(r"^/\*([0-9a-f]{8})\*/ extern ", open(os.path.join(G, "names", "MFL_callee_protos.txt")).read(), re.M)}
rows = []
for n, (lo, size) in own.items():
    if n in ledger: continue
    if n not in funcs: nodec[0] += 1; nodec[1] += size; continue      # gap_* / FUN_* ownership items without a named decompilation: not game functions of the census
    va, body = funcs.get(n, (lo, ""))
    flat = re.sub(r"\s+", " ", body)
    unsafe_f = any(re.search(r"(?<![\w.])\d+\.\d+(?![\w.])", a) and c not in ("if", "while", "for", "switch", "sizeof", "return") for c, a in re.findall(r"\b(\w+) ?\(([^;{}]*)\);", flat))
    detail, comp, stat, blk = "", "", "", ""
    rf = set(REG.findall(body)); rloc = {x for x in rf if x.startswith("reg_e")}
    only = bool(rf) and not (rf - rloc - X87) and n not in regin      # `reg_eax` is Ghidra's name for a LOCAL passed in a register to a callee with register parameters: the function itself is plain cdecl
    if n in regin: blk, detail = "usercall", "register inputs " + regin[n]
    elif rf and not only: blk, detail = "usercall", "decompiler pseudo register " + sorted(rf - rloc - X87)[0]
    elif [c for c in re.findall(r"\b(FUN_[0-9a-f]{8}) ?\(", flat) if c not in CALLEE_OK]: blk, detail = "unresolved-call", ",".join(sorted({c for c in re.findall(r"\b(FUN_[0-9a-f]{8}) ?\(", flat) if c not in CALLEE_OK}))[:120]      # callees with a prototype (tools/callee_protos.py) do not block
    elif "LAB_" in body: blk, detail = "label", "goto / jump table"
    elif unsafe_f: blk, detail = "float-literal", "float literal passed through unprototyped call"
    elif only:
        rc = [c for c in regcallee(body) if c != n]; miss = [c for c in rc if c not in ADAPTER]
        if not rloc: blk, detail = "x87-intrinsic", ",".join(sorted(rf & X87))
        else: blk, detail = ("plain-regcallee-noadapter" if miss else "plain-regcallee"), "calls register functions %s%s" % (",".join(rc[:4]), "; no adapter for " + ",".join(miss[:3]) if miss else "")
    elif n in failed: comp, blk, detail = "no", "compile-fail:" + errclass(failed[n]), failed[n][:120]
    elif n in ok:
        comp = "yes"; st = am.get(n, ("", "", ""))[2]
        rc = regcallee(body)
        if n in bad and st not in ("MATCH", "THUNK"): blk, detail = "known-bad", bad[n]
        elif st == "MATCH": blk = "byte-match-uninstalled"
        elif st == "THUNK": blk = "thunk"
        elif st.startswith("symbol") or st == "": blk, detail = "unresolved-symbol", st or "not compared by automatch"
        elif rc: blk, detail = "reg-callee", "calls register convention " + ",".join(rc[:3])
        else: blk, detail = "candidate-unchecked", st
    else: blk, detail = "not-compiled", "no autocompile result (wrong dir?)"
    rows.append(dict(name=n, va=va, size=size, compiles=comp, static_ok=stat, blocker=blk, bytes=size, detail=detail))

if BUILD:      # promote -> build -> staticcheck
    env = dict(os.environ, PROMOTE_OUT=WORK)
    names = [r["name"] for r in rows if r["blocker"] == "candidate-unchecked"]
    nf = os.path.join(WORK + ".names.txt"); os.makedirs(WORK, exist_ok=True)
    unres = {}      # function -> missing symbol: overlay.py cannot link a function that uses a symbol without an address (symbols.txt); the function is dropped and the build repeated
    for _ in range(6):
        open(nf, "w").write("\n".join(n for n in names if n not in unres) + "\n")
        print(subprocess.run([sys.executable, os.path.join(HERE, "automatch_promote.py"), "--tier2", "--names", nf, ac, amcsv], env=env, capture_output=True, text=True).stdout.strip().splitlines()[-1])
        got = {l.split()[1] for l in open(os.path.join(WORK, "names.txt"))}
        exe = os.path.join(WORK, "census.exe")
        if os.path.exists(exe): os.unlink(exe)
        b = subprocess.run([sys.executable, os.path.join(HERE, "build_overlay.py"), exe, "--automatch-near"], env=dict(os.environ, T2_SHADOW_DIR=WORK, RELCHECK="warn"), capture_output=True, text=True)
        open(os.path.join(WORK, "build.log"), "w").write(b.stdout + b.stderr)
        if os.path.exists(exe): break
        miss = set(re.findall(r"'([^']+)'\([^)]*\)", next((l for l in (b.stdout + b.stderr).splitlines() if l.startswith("unresolved symbols:")), "")))
        new = {n: m for n in got for m in miss if re.search(r"\b%s\b" % re.escape(m), funcs[n][1])}
        if not new: sys.exit("build failed, see %s/build.log:\n" % WORK + "\n".join(l[:200] for l in (b.stdout + b.stderr).splitlines() if re.search(r"rror|LNK|nresolved", l))[:3000])
        unres.update(new)
    only = os.path.join(WORK, "static_names.txt"); open(only, "w").write("\n".join(sorted(got)) + "\n")
    s = subprocess.run([sys.executable, os.path.join(HERE, "staticcheck.py"), exe, exe + ".map", only], env=dict(os.environ, STRONG="1"), capture_output=True, text=True)
    why = {}
    for l in s.stdout.splitlines()[1:]:
        p = l.split(None, 1)
        if len(p) == 2: why[p[0]] = p[1]
    rel = collections.defaultdict(list)
    for l in b.stdout.splitlines() + b.stderr.splitlines():
        m = re.match(r"RELCHECK(?:-\w+)? (\w+)", l)
        if m: rel[m.group(1)].append(l[:100])
    for r in rows:
        if r["blocker"] != "candidate-unchecked": continue
        n = r["name"]
        if n in unres: r["blocker"], r["detail"] = "unresolved-symbol", "no address for " + unres[n]
        elif n not in got: r["blocker"], r["detail"] = ("tier2-installed", "already in src/automatch_near (ledger stale)") if n in inst else ("dup-symbol", "promote refuses it: its body mentions a name that exists at several addresses in symbols.txt (or it is defined elsewhere in src/)")
        elif n in why: r["static_ok"], r["blocker"], r["detail"] = "no", "static-fail", why[n][:160]
        elif n in rel: r["static_ok"], r["blocker"], r["detail"] = "no", "static-fail", rel[n][0]      # relocation check warning
        else: r["static_ok"], r["blocker"] = "yes", "candidate-OK"
    top = [r["name"] for r in sorted(rows, key=lambda r: -r["size"]) if r["blocker"] == "candidate-OK"][:TOP]      # the biggest static-clean functions: a separate candidate set, src/ untouched
    open(nf, "w").write("\n".join(top) + "\n"); os.makedirs(CAND, exist_ok=True)
    print(subprocess.run([sys.executable, os.path.join(HERE, "automatch_promote.py"), "--tier2", "--names", nf, ac, amcsv], env=dict(os.environ, PROMOTE_OUT=CAND), capture_output=True, text=True).stdout.strip().splitlines()[-1])
    cf = sorted(glob.glob(os.path.join(CAND, "an_*.c")))      # portable / linkable source forms (candidates only): ptrcast.py (typed byte pointers), rawaddr.py --rewrite (raw data addresses -> symbols)
    if cf:
        subprocess.run([sys.executable, os.path.join(HERE, "ptrcast.py")] + cf, capture_output=True); subprocess.run([sys.executable, os.path.join(HERE, "rawaddr.py"), "--rewrite"] + cf, capture_output=True)
    open(os.path.join(CAND, "ok_names.txt"), "w").write("\n".join(r["name"] for r in sorted(rows, key=lambda r: -r["size"]) if r["blocker"] == "candidate-OK") + "\n")

rows.sort(key=lambda r: -r["size"])
with open(os.path.join(G, "output", "tier2_census.csv"), "w", newline="") as f:
    w = csv.DictWriter(f, ["name", "va", "size", "compiles", "static_ok", "blocker", "bytes", "detail"], extrasaction="ignore"); w.writeheader(); w.writerows(rows)
cnt, byt = collections.Counter(), collections.Counter()
for r in rows: cnt[r["blocker"]] += 1; byt[r["blocker"]] += r["size"]
print("%d game functions not in C, %d bytes" % (len(rows), sum(byt.values())))
print("(%d ownership items gap_* / FUN_* without a named decompilation, %d bytes: not counted)" % tuple(nodec))
for k, v in byt.most_common(): print("%-34s %5d fns %8d bytes  %5.1f%%" % (k, cnt[k], v, 100.0 * v / sum(byt.values())))
