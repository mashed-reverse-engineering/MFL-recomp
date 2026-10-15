#!/usr/bin/env python3
"""callee_protos.py: prototypes (and, where hand determined, readable names) for the FUN_ callees that keep functions from compiling ("unresolved-call" in tools/tier2_census.py).
Reads output/named/MFL_app_named.c (every call site), output/fnsizes.txt, output/reginputs.csv, output/stdcall.txt and names/MFL_callee_hand.tsv; writes
  names/MFL_callee_protos.txt   one declaration per callee (`/*va*/ extern ret [__conv] name(params);`), inferred from the call sites (argument count by majority, `int` result when any caller
                                uses it, `double` when a caller casts it (float10), `float` argument positions where a caller passes a (float) / (double) expression) unless the hand table gives the types
  names/MFL_callee_names.csv    `address,name,comment` for the hand named callees (scripts/RenameFns.java input; apply in Ghidra, then FUN_ -> name in the named C)
  output/callee_dossier.csv     one row per callee: va, callers, size, register inputs (bridge candidates, NOT plain C: no prototype is written for them), ret N, nargs, consistency, flags
  src/aliases.txt            `name va` for the hand named callees (idempotent append)
tools/autocompile.py reads protos + names: a call to a FUN_ callee with a prototype no longer makes its caller "unresolved-call"; with a hand name the body is renamed first.
Usage: callee_protos.py [--check]   (--check: print the summary only, write nothing)"""
import collections, csv, os, re, sys
G = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
P = lambda *a: os.path.join(G, *a)
text = open(P("output", "named", "MFL_app_named.c"), errors="replace").read()
hand = {}
for l in open(P("names", "MFL_callee_hand.tsv")):
    f = l.rstrip("\n").split("\t")
    if len(f) >= 4 and not l.startswith("#"): hand[f[0]] = dict(name=f[1], ret=f[2], params=[p.strip() for p in f[3].split(",") if p.strip()], comment=f[4] if len(f) > 4 else "")
regin, retn = {}, {}
for l in open(P("output", "reginputs.csv")).read().splitlines()[1:]:
    p = l.split(",")
    if len(p) >= 4: regin[p[1]] = p[2].strip("|"); retn[p[1]] = p[3]
std = {l.split()[0]: int(l.split()[1]) for l in open(P("output", "stdcall.txt")) if len(l.split()) == 2}
size = {}
for l in open(P("output", "fnsizes.txt")):
    p = l.split()
    if len(p) == 3: size[p[0]] = int(p[2])
defined = set(re.findall(r"// ==== (\S+) @", text))

def split_args(s):
    out, d, cur = [], 0, ""
    for ch in s:
        if ch in "([{": d += 1
        elif ch in ")]}": d -= 1
        if ch == "," and d == 0: out.append(cur.strip()); cur = ""
        else: cur += ch
    if cur.strip(): out.append(cur.strip())
    return out

sites = collections.defaultdict(list)       # va -> [(caller, args, used, floatret)]
callers = collections.defaultdict(set)
for m in re.finditer(r"// ==== (\S+) @ ([0-9a-f]+)\n(.*?)(?=\n// ==== |\Z)", text, re.S):
    fn, body = m.group(1), m.group(3)
    for c in re.finditer(r"\bFUN_([0-9a-f]{8}) ?\(", body):
        i, d = c.end(), 1
        while i < len(body) and d:
            d += {"(": 1, ")": -1}.get(body[i], 0); i += 1
        args = split_args(body[c.end():i - 1])
        pre = body[max(0, c.start() - 40):c.start()]
        ls = body.rfind("\n", 0, c.start()) + 1
        used = bool(body[ls:c.start()].strip()) and not re.fullmatch(r"\s*(?:/\*.*\*/)?\s*", body[ls:c.start()])
        flt = bool(re.search(r"\((?:float10|double|float)\)\s*$", pre))
        sites[c.group(1)].append((fn, args, used, flt)); callers[c.group(1)].add(fn)
FLOATARG = re.compile(r"\((?:double|float)\)|\b\d+\.\d+\b")
rows, protos, names = [], [], []
for va, ss in sorted(sites.items(), key=lambda kv: -len(callers[kv[0]])):
    h = hand.get(va)
    ri = regin.get(va, "")
    nargs = collections.Counter(len(a) for _, a, _, _ in ss).most_common()
    nz = [kv for kv in nargs if kv[0]] or nargs      # a call printed without arguments is a register / lost argument artifact: the majority is taken among the calls that have arguments
    n = nz[0][0]
    if va in std: n = std[va] // 4
    cons = "ok" if len(nargs) == 1 else "majority" if nz[0][1] >= 0.75 * sum(c for _, c in nz) else "mixed:" + "/".join("%d x%d" % kv for kv in nargs)      # decompiler argument counts wobble (register callers print FUN_x() without arguments): trust a 75% majority
    used = any(u for _, _, u, _ in ss); flt = any(f for _, _, _, f in ss)
    fl = [i for i in range(n) if any(len(a) == n and FLOATARG.search(a[i]) for _, a, _, _ in ss)]
    flags = []
    if ri: flags.append("REGISTER-ARGS")
    if va in std: flags.append("stdcall")
    if cons not in ("ok", "majority"): flags.append("argcount-" + cons)
    if fl: flags.append("float-arg%s" % fl)
    conv = "__stdcall " if va in std else ""
    if h:
        pn = ", ".join("%s p%d" % (t, i) for i, t in enumerate(h["params"])) or "void"
        d = "/*%s*/ extern %s %s%s(%s);" % (va, h["ret"], conv, h["name"], pn)
        names.append((va, h["name"], h["comment"]))
    elif ri: d = ""      # bridge candidate: plain C cannot express register inputs
    else:
        pn = ", ".join(("float" if i in fl else "undefined4") + " p%d" % i for i in range(n)) or "void"
        d = "/*%s*/ extern %s %sFUN_%s(%s);" % (va, "double" if flt else "int" if used else "void", conv, va, pn)
    if d and not (cons.startswith("mixed") and not h): protos.append(d)       # mixed argument counts without a hand prototype: not trusted
    rows.append([va, len(callers[va]), size.get(va, ""), ri, retn.get(va, "") if retn.get(va, "") != "0" else (std.get(va, "")), n, cons, "used" if used else "unused", "float" if flt else "", h["name"] if h else "", ";".join(flags)])
if "--check" in sys.argv:
    print(len(rows), "callees,", len(protos), "protos,", sum(1 for r in rows if r[3]), "register-input (bridge candidates),", sum(1 for r in rows if r[6].startswith("mixed")), "mixed argument counts,", len(names), "named"); sys.exit()
open(P("names", "MFL_callee_protos.txt"), "w").write("# generated by tools/callee_protos.py (names/MFL_callee_hand.tsv for the hand determined ones)\n" + "\n".join(protos) + "\n")
open(P("names", "MFL_callee_names.csv"), "w").write("# address,name,comment  (MFL.exe): FUN_ callees named from their disassembly + callers (tools/callee_protos.py); RenameFns.java input\n" + "".join("%s,%s,%s\n" % (v, n, c.replace(",", ";")) for v, n, c in names))
with open(P("output", "callee_dossier.csv"), "w", newline="") as f:
    w = csv.writer(f); w.writerow("va,callers,size,reg_inputs,ret_n,nargs,argcount,result,floatret,name,flags".split(",")); w.writerows(rows)
al = open(P("src", "aliases.txt")).read()
add = "".join("%s %s\n" % (n, v) for v, n, _ in names if not re.search(r"^%s " % re.escape(n), al, re.M))
if add: open(P("src", "aliases.txt"), "a").write(add if al.endswith("\n") else "\n" + add)
print(len(rows), "callees,", len(protos), "protos,", len(names), "named,", sum(1 for r in rows if r[3]), "register-input")
