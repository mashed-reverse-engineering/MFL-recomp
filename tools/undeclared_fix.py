#!/usr/bin/env python3
"""undeclared_fix.py <failed.txt>...: C2065 `'X' : undeclared identifier` in autocompile failures -> declarations that autocompile.py emits from names/MFL_extern_fix.txt (additive, idempotent).
  X is an enum of names/MFL_enums.txt used as a type (`CourseId`, `WeaponId`; the header predates the enums)   -> `typedef int X;`
  X is an address named data blob (`PTR_..._005df7f0`, `s_..._00617970`, `UNK_005df51c`, `DAT_...`)            -> `extern char X[];` (+ `X <va>` in src/aliases.txt when no symbol exists: the suffix is the address)
  X is a symbol / alias without a declaration (`g_x`)                                                           -> `extern struct S g_x[N];` when its address is an `apply,<addr>,S,N` array of the struct specs, else only reported
Prints one line per name; the rest stays an error.  Usage: undeclared_fix.py failed.txt [more failed.txt]"""
import os, re, sys
G = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
P = lambda *a: os.path.join(G, *a)
want = {}
for f in sys.argv[1:]:
    for l in open(f, errors="replace"):
        m = re.search(r"C2065 '(\w+)' : undeclared identifier", l)
        if m: want.setdefault(m.group(1), set()).add(l.split("\t")[0])
sym = {}
for f in [P("src", "symbols.txt"), P("src", "aliases.txt")] + ([os.path.join(os.environ["T2_OVERLAY_DIR"], "aliases.txt")] if os.environ.get("T2_OVERLAY_DIR") and os.path.exists(os.path.join(os.environ["T2_OVERLAY_DIR"], "aliases.txt")) else []):
    for l in open(f):
        p = l.split()
        if len(p) == 2: sym[p[0]] = p[1]
APPLY = {}      # address -> (struct, count) of the arrays of structs in the struct specs
import glob
for _f in glob.glob(P("names", "MFL_structs*.txt")):
    for l in open(_f):
        p = l.strip().split(",")
        if p[0] == "apply" and len(p) >= 4 and int(p[3]) > 1: APPLY[int(p[1], 16)] = (p[2], int(p[3]))
enums = {l.split(",")[1] for l in open(P("names", "MFL_enums.txt")) if l.startswith("enum,")}
OV = os.environ.get("T2_OVERLAY_DIR")      # t2_auto run: new declarations / aliases go to <dir>/MFL_extern_fix.txt / aliases.txt (the shared files are only read)
fixp = os.path.join(OV, "MFL_extern_fix.txt") if OV else P("names", "MFL_extern_fix.txt"); alp = os.path.join(OV, "aliases.txt") if OV else P("src", "aliases.txt")
if OV: os.makedirs(OV, exist_ok=True)
have = set(open(fixp).read().split("\n")) if os.path.exists(fixp) else set()
if OV: have |= set(open(P("names", "MFL_extern_fix.txt")).read().split("\n"))
new, al, rep = [], [], []
for x in sorted(want):
    if x == "sbyte": new.append("typedef signed char sbyte;")
    elif x in enums: new.append("typedef int %s;" % x)
    elif re.search(r"(?:^|_)([0-9a-f]{8})$", x) and re.match(r"(?:PTR_|s_|UNK_|DAT_|u_|LAB_|switchD_)", x) and not x.startswith("switchD_"):
        new.append("extern char %s[];" % x)
        if x not in sym: al.append("%s %s" % (x, re.search(r"([0-9a-f]{8})$", x).group(1)))
    elif x in sym and int(sym[x], 16) in APPLY: new.append("extern struct %s %s[%d];" % (*APPLY[int(sym[x], 16)][:1], x, APPLY[int(sym[x], 16)][1]))      # g_x named at the start of an array of structs (names/MFL_structs*.txt `apply`); the struct comes from tools/spec_header.py
    else: rep.append(x)
new = [n for n in new if n not in have]
if new:
    with open(fixp, "a") as o:
        if not os.path.exists(fixp) or os.path.getsize(fixp) == 0: o.write("# declarations for identifiers the Ghidra export lacks (tools/undeclared_fix.py); autocompile.py emits the ones a chunk uses\n")
        o.write("\n".join(new) + "\n")
if al:
    with open(alp, "a") as o: o.write("\n".join(al) + "\n")
print("declared %d (%d new, %d aliases added); not fixable: %s" % (len(want) - len(rep), len(new), len(al), " ".join(rep)))
