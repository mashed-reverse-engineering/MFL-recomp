#!/usr/bin/env python3
"""Globals that Ghidra left as 1 byte `undefined` although the original accesses them with 4 (or 2) byte instructions: a C store to such a variable compiles to `mov BYTE PTR` instead of
`mov DWORD PTR` (found by the matching agent pilot: text_pages_clear, course_race_start).  Looks at every absolute memory operand `ds:ADDR` in the original code (explicit BYTE/WORD/DWORD PTR, or the register width of
`mov reg, ds:ADDR`), widens `undefined` globals to undefined4 / undefined2 when the widest access starting exactly there is that wide and no other symbol lies inside the widened range.
Appends `name type` to names/MFL_global_types.txt (applied by tools/autocompile.py).  Existing entries are kept.  Usage: global_widths.py"""
import collections, os, re, subprocess
HERE = os.path.dirname(os.path.abspath(__file__))
EXE = os.path.join(HERE, "..", "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")
dis = subprocess.run(["objdump", "-d", "-M", "intel", "--start-address=0x401000", "--stop-address=0x5d7000", EXE], capture_output=True, text=True).stdout
W = collections.defaultdict(set)
REG = {"eax": 4, "ecx": 4, "edx": 4, "ebx": 4, "esi": 4, "edi": 4, "ebp": 4, "esp": 4, "ax": 2, "cx": 2, "dx": 2, "bx": 2, "si": 2, "di": 2, "al": 1, "cl": 1, "dl": 1, "bl": 1, "ah": 1, "ch": 1, "dh": 1, "bh": 1}
for m in re.finditer(r"(BYTE|WORD|DWORD) PTR ds:0x([0-9a-f]+)", dis):
    W[int(m.group(2), 16)].add({"BYTE": 1, "WORD": 2, "DWORD": 4}[m.group(1)])
for m in re.finditer(r"\bmov\s+(\w+),ds:0x([0-9a-f]+)|\bmov\s+ds:0x([0-9a-f]+),(\w+)\s*$", dis, re.M):
    a, r = (m.group(2), m.group(1)) if m.group(2) else (m.group(3), m.group(4))
    if r in REG: W[int(a, 16)].add(REG[r])
names = collections.defaultdict(list)
for l in open(os.path.join(HERE, "..", "src", "symbols.txt")):
    p = l.split()
    if len(p) == 2:
        try: names[int(p[1], 16)].append(p[0])
        except ValueError: pass
addrs = sorted(names)
decl = {}
for l in open(os.path.join(HERE, "..", "output", "decls.h"), errors="replace"):
    m = re.match(r"/\*([0-9a-f]{8})\*/ extern ([^;(\[]*?)(\w+);", l)
    if m: decl[int(m.group(1), 16)] = (m.group(2).strip(), m.group(3))
P = os.path.join(HERE, "..", "names", "MFL_global_widths.txt")      # own file: rule "widths" (FIX_RULES)
T = os.path.join(HERE, "..", "names", "MFL_global_types.txt")
cur = {}
for l in open(T):
    p = l.split()
    if len(p) == 2 and not l.startswith("#"): cur[p[0]] = p[1]
new_only = {}
import bisect
new = 0
for a, (t, nm) in sorted(decl.items()):
    if t != "undefined" or nm in cur or a < 0x5f0000 or a not in W: continue
    w = max(W[a])
    if w < 2: continue
    nxt = addrs[bisect.bisect_right(addrs, a)] if bisect.bisect_right(addrs, a) < len(addrs) else 1 << 32
    if nxt < a + w: continue                           # another symbol inside the widened range
    new_only[nm] = "undefined4" if w == 4 else "undefined2"; new += 1
with open(P, "w") as o:
    o.write("# globals widened from 1 byte `undefined` by tools/global_widths.py (rule 'widths'): name type\n")
    for g, t in sorted(new_only.items()): o.write("%s %s\n" % (g, t))
print(new, "widened globals ->", P)
