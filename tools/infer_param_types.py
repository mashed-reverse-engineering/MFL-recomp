#!/usr/bin/env python3
"""infer_param_types.py [named.c] [out.csv]: find function parameters that are struct pointers.
A parameter gets type S* when >=3 call sites (or >=1 site and >=3 field accesses in the body) pass a struct-S array element / known S pointer (global array of S from output/decls.h,
local assigned from one, or an already inferred parameter), no site disagrees, and every `*(T*)(p + off)` in the body is inside sizeof(S).
Output names/MFL_param_types.csv: fn,param_index(0-based, signature order),type[,new_name (only for reg_eXX params)]."""
import re, sys, glob, collections
import os
R = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..") + "/"
src = open(sys.argv[1] if len(sys.argv) > 1 else R + "output/named/MFL_app_named.c", errors="replace").read()
out = sys.argv[2] if len(sys.argv) > 2 else R + "names/MFL_param_types.csv"
MIN = 3
size = {}
for f in glob.glob(R + "names/MFL_structs*.txt"):
    for l in open(f):
        if l.startswith("struct,"):
            _, n, s = l.strip().split(",")[:3]; size[n] = int(s, 0)
gl = {}  # global name -> struct
for m in re.finditer(r"extern (?:struct )?(\w+) (\w+)(?:\[\d+\])?;", open(R + "output/decls.h", errors="replace").read()):
    if m.group(1) in size: gl[m.group(2)] = m.group(1)

def split_args(s):
    a, d, cur = [], 0, ""
    for ch in s:
        if ch in "([": d += 1
        elif ch in ")]": d -= 1
        if ch == "," and d == 0: a.append(cur.strip()); cur = ""
        else: cur += ch
    if cur.strip(): a.append(cur.strip())
    return a

fns = {}  # name -> dict
for m in re.finditer(r"^// ==== (\w+) @ ([0-9a-f]+)\n(.*?)(?=^// ==== |\Z)", src, re.S | re.M):
    name, addr, chunk = m.groups()
    h = re.search(r"^[A-Za-z_][^\n;{]*?\b%s\s*\((.*?)\)\s*\n\s*\{" % re.escape(name), chunk, re.S | re.M)
    if not h: continue
    ps = []
    for p in split_args(re.sub(r"\s+", " ", h.group(1))):
        if p in ("void", ""): continue
        pm = re.match(r"(.*?)(\w+)(?:\[\d*\])?$", p)
        ps.append((pm.group(1).strip(), pm.group(2)) if pm else ("", p))
    fns[name] = dict(params=ps, body=chunk[h.end():], head=chunk[:h.end()])
CALL = re.compile(r"\b(\w+)\(")

def offs(body, p):
    r = []
    for m in re.finditer(r"\(\(?(?:int\))?%s ?\+ ?(0x[0-9a-f]+|\d+)\)" % re.escape(p), body):
        r.append(int(m.group(1), 0))
    return r

CASTRE = re.compile(r"^\(\s*[\w ]+\*?\s*\)")
def clean(e):
    e = e.strip()
    while True:
        n = CASTRE.sub("", e).strip()
        if n.startswith("(") and n.endswith(")") and n.count("(") == n.count(")"): n = n[1:-1].strip()
        if n == e: return e
        e = n
def expr_struct(e, vt):
    m = re.match(r"^\((?:struct )?(\w+) ?\*\)", e.strip())
    if m and m.group(1) in size: return m.group(1)
    e = clean(e)
    m = re.match(r"^&?(\w+)(?: ?\+ ?[^\]]+|\[[^\]]*\])?$", e)
    if m:
        n = m.group(1)
        if n in gl and (e.startswith("&") or "+" in e or "[" in e or e == n): return gl[n]
        if n in vt and re.fullmatch(r"\w+(?: ?\+ ?[^\]\[]+)?", e): return vt[n]
    return None

result = {}  # (fn, idx) -> struct
for rnd in range(6):
    sites = collections.defaultdict(collections.Counter)
    for cn, c in fns.items():
        vt = {}
        for i, (t, p) in enumerate(c["params"]):
            m = re.match(r"(\w+) ?\*$", t)
            if m and m.group(1) in size: vt[p] = m.group(1)
            if (cn, i) in result: vt[p] = result[(cn, i)]
        for m in re.finditer(r"^\s*(?:struct )?(\w+) ?\*(\w+);", c["body"], re.M):
            if m.group(1) in size: vt[m.group(2)] = m.group(1)
        asg = collections.defaultdict(set)
        for m in re.finditer(r"^\s*(\w+) = (.*);$", c["body"], re.M):
            s = expr_struct(m.group(2), {})
            asg[m.group(1)].add(s)
        for v, ss in asg.items():
            ss.discard(None)
            if len(ss) == 1 and v not in vt: vt[v] = next(iter(ss))
        c["vt"] = vt
        for m in CALL.finditer(c["body"]):
            callee = m.group(1)
            if callee not in fns or callee == cn: continue
            i, d = m.end(), 1
            while d and i < len(c["body"]):
                d += {"(": 1, ")": -1}.get(c["body"][i], 0); i += 1
            for k, a in enumerate(split_args(c["body"][m.end():i - 1])):
                s = expr_struct(a, vt)
                sites[(callee, k)][s or "?"] += 1
    new = {}
    for (fn, k), cnt in sites.items():
        c = fns[fn]
        if k >= len(c["params"]) or (fn, k) in result: continue
        t, p = c["params"][k]
        if re.search(r"\w+ ?\*$", t) and not re.match(r"(undefined\d?|int|uint|void|float|char|byte|short|ushort|longlong) ?\*+$", t): continue
        structs = [s for s in cnt if s != "?"]
        if len(structs) != 1: continue
        S = structs[0]
        o = offs(c["body"], p)
        if cnt[S] < MIN and (cnt[S] < 1 or len(o) < 3): continue  # relaxed: one consistent caller + >=3 field accesses
        if o and max(o) + 1 > size[S]: continue
        new[(fn, k)] = S
    if not new: break
    result.update(new); print("round", rnd, len(new))
rows = []
for (fn, k), S in sorted(result.items()):
    t, p = fns[fn]["params"][k]
    nn = ""
    if re.fullmatch(r"reg_e\w+", p):
        cand = re.sub(r"(?<!^)(?=[A-Z])", "_", S).lower()
        if not re.search(r"\b%s\b" % cand, fns[fn]["head"] + fns[fn]["body"]): nn = cand
    rows.append("%s,%d,%s*%s" % (fn, k, S, "," + nn if nn else ""))
open(out, "w").write("# fn,param_index,type[,new_name]; generated by tools/infer_param_types.py\n" + "\n".join(rows) + "\n")
print(len(rows), "entries;", collections.Counter(S for S in result.values()))
