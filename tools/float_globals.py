#!/usr/bin/env python3
"""Globals that the original accesses with x87 float instructions (`fld DWORD PTR ds:ADDR`, fadd / fmul / fcom ... with a 4 or 8 byte memory operand at an absolute address) are floats / doubles.
Ghidra often leaves them `undefined4` (then C code compiled from the decompilation uses `fild` / integer compares).  Appends `name float|double` to names/MFL_global_types.txt, which
tools/autocompile.py applies to the declarations.  Existing entries are kept.  Usage: float_globals.py"""
import collections, os, re, subprocess
HERE = os.path.dirname(os.path.abspath(__file__))
EXE = os.path.join(HERE, "..", "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")
dis = subprocess.run(["objdump", "-d", "-M", "intel", "--start-address=0x401000", "--stop-address=0x5d7000", EXE], capture_output=True, text=True).stdout
kinds = collections.defaultdict(collections.Counter)
for m in re.finditer(r"\b(?:fld|fst|fstp|fadd|fsub|fsubr|fmul|fdiv|fdivr|fcom|fcomp)\s+(DWORD|QWORD) PTR ds:0x([0-9a-f]+)", dis):
    kinds[int(m.group(2), 16)][m.group(1)] += 1
names = collections.defaultdict(list)
for l in open(os.path.join(HERE, "..", "src", "symbols.txt")):
    p = l.split()
    if len(p) == 2:
        try: names[int(p[1], 16)].append(p[0])
        except ValueError: pass
decl = {}
for l in open(os.path.join(HERE, "..", "output", "decls.h"), errors="replace"):
    m = re.match(r"/\*([0-9a-f]{8})\*/ extern ([^;(\[]*?)(\w+);", l)
    if m: decl[int(m.group(1), 16)] = m.group(2).strip()
P = os.path.join(HERE, "..", "names", "MFL_global_types.txt")
cur = {}
if os.path.exists(P):
    for l in open(P):
        p = l.split()
        if len(p) == 2 and not l.startswith("#"): cur[p[0]] = p[1]
new = 0
for a, c in sorted(kinds.items()):
    if a not in names or len(names[a]) != 1: continue
    nm = names[a][0]
    if nm.startswith(("FUN_", "LAB_", "switch", "case")): continue
    typ = "double" if c["QWORD"] > c["DWORD"] else "float"
    d = decl.get(a, "")
    if "float" in d or "double" in d or nm in cur: continue
    if a < 0x5f0000: continue                        # code / read-only text is not a data variable
    cur[nm] = typ; new += 1
with open(P, "w") as o:
    o.write("# global variable type overrides for the autocompile pipeline (variants.py rule gsign, tools/float_globals.py): name type\n")
    for g, t in sorted(cur.items()): o.write("%s %s\n" % (g, t))
print(new, "new float / double globals, total overrides", len(cur), "->", P)
