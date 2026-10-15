#!/usr/bin/env python3
"""usercall_plain.py: candidates for the census classes plain-regcallee (cdecl functions with `reg_e??` locals, calls to register functions through F_r adapters) and x87-intrinsic
(fsin / fcos / fpatan / ABS as inline-asm helpers).  Nothing enabled, nothing in src/.  Runs tools/autocompile.py with AC_PLAIN=1 AC_X87=1 AC_ONLY=<those functions> in a scratch dir,
then writes <outdir>/b###.c (one translation unit per passing chunk) + prelude.h / ghidra_types.h + names.txt + classes.csv (name, class, compiles, reason).
(<outdir> must be fresh or hold only an earlier run with at least as many chunks: stale b###.c are not deleted.)  Usage: usercall_plain.py <scratch dir> [outdir=output/usercall_candidates2]   (needs output/tier2_census.csv from the current tools/tier2_census.py; VC7_CONTAINER / AC_JOBS pass through)"""
import csv, glob, os, shutil, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__)); G = os.path.join(HERE, "..")
work = os.path.abspath(sys.argv[1]); out = os.path.abspath(sys.argv[2] if len(sys.argv) > 2 else os.path.join(G, "output", "usercall_candidates2"))
rows = [r for r in csv.DictReader(open(os.path.join(G, "output", "tier2_census.csv"))) if r["blocker"] in ("plain-regcallee", "x87-intrinsic")]
os.makedirs(work, exist_ok=True); os.makedirs(out, exist_ok=True)
env = dict(os.environ, AC_PLAIN="1", AC_X87="1", AC_JOBS=os.environ.get("AC_JOBS", "2"))
def run(names, tag):
    d = os.path.join(work, tag); os.makedirs(d, exist_ok=True)
    for h in ("prelude.h", "ghidra_types.h"): shutil.copy(os.path.join(G, "src", "auto", h), d)
    open(os.path.join(work, tag + ".txt"), "w").write("\n".join(names) + "\n")
    subprocess.run([sys.executable, os.path.join(HERE, "autocompile.py"), d, os.path.join(G, "output", "decls.h"), "401000", "5d7000", "40"], env=dict(env, AC_ONLY=os.path.join(work, tag + ".txt")), capture_output=True, text=True)
    return d, dict(l.rstrip("\n").split("\t", 1) for l in open(os.path.join(d, "failed.txt")) if "\t" in l)
ac, failed = run([r["name"] for r in rows], "ac")
extra = []      # a failing function hides its successor in the chunk (the error line lands in the next function): every failure is retried alone
for n in sorted(failed):
    d, f1 = run([n], "single_" + n.replace(".", "_"))
    if n not in f1 and any(l.split()[0] == n for l in open(os.path.join(d, "addrs.txt"))): extra.append(d); del failed[n]
ok = [l.split()[0] for l in open(os.path.join(ac, "addrs.txt"))]
ok += [l.split()[0] for d in extra for l in open(os.path.join(d, "addrs.txt"))]
for i, f in enumerate(sorted(glob.glob(os.path.join(ac, "ok_*.c"))) + [os.path.join(d, "ok_001.c") for d in extra]): shutil.copy(f, os.path.join(out, "b%03d.c" % (i + 1)))
for h in ("prelude.h", "ghidra_types.h"): shutil.copy(os.path.join(ac, h), out)
subprocess.run([sys.executable, os.path.join(HERE, "rawaddr.py"), "--rewrite", "--quiet"] + sorted(glob.glob(os.path.join(out, "b*.c"))), capture_output=True)      # raw data address literals -> (int)&symbol (cheatgrep)
open(os.path.join(out, "names.txt"), "w").write("\n".join(ok) + "\n")
with open(os.path.join(out, "classes.csv"), "w", newline="") as f:
    w = csv.writer(f); w.writerow(["name", "class", "size", "compiles", "reason"])
    for r in rows: w.writerow([r["name"], r["blocker"], r["size"], "yes" if r["name"] in ok else "no", failed.get(r["name"], "")])
print("%d census functions, %d compile, %d fail" % (len(rows), len(ok), len(rows) - len(ok)))
