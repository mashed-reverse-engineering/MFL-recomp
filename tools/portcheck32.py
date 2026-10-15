#!/usr/bin/env python3
"""Portability report: compile every src/*.c with `gcc -m32 -fsyntax-only -ffreestanding` (an x86 code quality check; the real build stays cl 13.00.9466).
Counts errors per file and per kind (naked/inline asm, __declspec, VC7 extensions ...).  Usage: portcheck.py [out.txt]   (default output/portability.txt)"""
import collections, glob, os, re, subprocess, sys
R = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "src")
out = sys.argv[1] if len(sys.argv) > 1 else os.path.join(R, "..", "output", "portability.txt")
kinds, rows = collections.Counter(), []
for f in sorted(glob.glob(R + "/*.c")):
    r = subprocess.run(["gcc", "-m32", "-fsyntax-only", "-ffreestanding", "-w", "-fmax-errors=0", f], capture_output=True, text=True, cwd=R)
    errs = [l for l in r.stderr.split("\n") if ": error:" in l]
    src = open(f, encoding="latin-1").read()
    notes = []
    if "__declspec(naked)" in src: notes.append("naked asm x%d" % src.count("__declspec(naked)"))
    if "__asm" in src.replace("__declspec(naked)", ""): notes.append("inline asm")
    for e in errs:
        kinds[re.sub(r"[‘’'][^’']*[’']", "X", e.split("error:", 1)[1].strip())[:90]] += 1
    rows.append((os.path.basename(f), len(errs), ", ".join(notes)))
with open(out, "w") as o:
    o.write("# gcc -m32 -fsyntax-only report (tools/portcheck.py): file, errors, notes\n")
    for n, e, nt in rows: o.write("%-28s %4d  %s\n" % (n, e, nt))
    o.write("\n# error kinds\n")
    for k, c in kinds.most_common(): o.write("%5d  %s\n" % (c, k))
print("%d files, %d with errors, %d errors -> %s" % (len(rows), sum(1 for r in rows if r[1]), sum(r[1] for r in rows), out))
