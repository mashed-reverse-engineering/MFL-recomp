#!/usr/bin/env python3
"""T0 inventory, T1 C-declaration vs generated-extent check, F5 link-level name check.   Usage: types.py <globgen outdir>   (needs the VC7 container for sizeof probes)
Writes <outdir>/types.csv (T0: one row per generated item: chosen type, source, C declarations, Ghidra type) and <outdir>/types_report.txt (T1 + F5).
T1: sizeof(C declared type [* dims]) must fit the item extent from the declared name on (else the C overruns into the neighbour), and a C float scalar must not sit in an integer item.
F5 (static equivalent of the real link with stubs.c removed): duplicate definitions (LNK2005 / COMMON size), C extern data names without a generated definition (LNK2001),
function pointer targets that need a decorated name."""
import collections, csv, os, re, subprocess, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import cdecls
G = cdecls.G
W = os.path.abspath(sys.argv[1])
rows = list(csv.DictReader(open(os.path.join(W, "globals_layout.csv"))))
item = {r["name"]: (int(r["va"], 16), int(r["size"]), r) for r in rows}
addr = {}      # every generated name -> (address, item end)
for n, (va, sz, r) in item.items(): addr[n] = (va, va + sz)
for l in open(os.path.join(W, "aliases_gen.txt")):
    if l[0] == "#": continue
    n, o, p = l.split()
    if p in item and not n.endswith("__fwd"): addr[n] = (item[p][0] + int(o), item[p][0] + item[p][1])
decl = cdecls.scan(set(addr) | {n for n in re.findall(r"[A-Za-z_]\w*", open(os.path.join(G, "src/mfl_externs.h"), errors="replace").read())})
gdecl = {}
for l in open(os.path.join(G, "output/decls.h"), errors="replace"):
    m = re.match(r"/\*([0-9a-f]{8})\*/ extern (.*?) \**(\w+)(\[\d*\])?;", l)
    if m: gdecl[int(m.group(1), 16)] = m.group(2) + (m.group(4) or "")
chosen = {}
for sec in ("rdata", "data", "bss"):
    for m in re.finditer(r"/\*([0-9a-f]{8})\*/ (.+?) (\w+)((?:\[\d+\])?) = ", open(os.path.join(W, sec + ".c")).read()):
        chosen[m.group(3)] = m.group(2).replace("const ", "") + m.group(4)
src = {r["name"]: (r["source"], r["reason"]) for r in csv.DictReader(open(os.path.join(W, "types_chosen.csv")))}
# sizeof of the non-builtin C types
nonb = {d["base"] for ds in decl.values() for d in ds if d["base"] not in cdecls.SIZE and not d["ptr"]}
vc7 = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "vc7.sh")
pw = os.path.join(W, "probe"); os.makedirs(pw, exist_ok=True)
psz = cdecls.probe_sizes(nonb, pw, vc7)


def csize(d):
    """-> (element size or None, count or None)"""
    el = 4 if d["ptr"] else cdecls.SIZE.get(d["base"], psz.get(d["base"]))
    cnt = 1
    for x in d["dims"]:
        if x is None: return el, None
        cnt *= x
    return el, cnt


# T0
with open(os.path.join(W, "types.csv"), "w") as f:
    w = csv.writer(f)
    w.writerow(["name", "va", "section", "extent", "kind", "chosen", "source", "reason", "c_decls", "ghidra_type"])
    for r in rows:
        if r["kind"] == "pe": continue
        va = int(r["va"], 16)
        cs = sorted({"%s%s%s" % (d["base"], "*" * d["ptr"], "".join("[%s]" % (x if x is not None else "") for x in d["dims"])) for d in decl.get(r["name"], [])})
        s = src.get(r["name"], ("default", ""))
        w.writerow([r["name"], r["va"], r["section"], r["size"], r["kind"], chosen.get(r["name"], ""), s[0], s[1], " | ".join(cs), gdecl.get(va, "")])
# T1
over, unknown, floatint, conflicts = [], 0, [], []
for n, ds in sorted(decl.items()):
    if n not in addr: continue
    a, end = addr[n]
    avail = end - a
    for d in ds:
        if d["file"].endswith("mfl_externs.h") or True:
            el, cnt = csize(d)
            if el is None: unknown += 1; continue
            tot = el * (cnt if cnt is not None else 1)
            if tot > avail: over.append((n, "%s%s%s" % (d["base"], "*" * d["ptr"], d["dims"]), tot, avail, d["file"])); break
    k = {(d["base"], d["ptr"], tuple(d["dims"])) for d in ds}
    if len(k) > 1: conflicts.append((n, sorted(str(x) for x in k)))
    if any(d["base"] in ("float", "double") and not d["ptr"] and not d["dims"] for d in ds) and not chosen.get(n, "").startswith(("float", "double")):
        floatint.append((n, chosen.get(n, "?")))
# F5
gen_names = set(addr)
dups = [(n, d) for n, ds in decl.items() if n in gen_names for d in ds if not d["extern"]]
lnk2005 = [(n, d["file"]) for n, d in dups if d["init"]]
common = []
for n, d in dups:
    if d["init"]: continue
    el, cnt = csize(d)
    tot = el * (cnt or 1) if el else None
    common.append((n, d["file"], tot, addr[n][1] - addr[n][0]))
syms = {}
for fn in ("src/symbols.txt", "src/aliases.txt"):
    for l in open(os.path.join(G, fn)):
        p = l.split()
        if len(p) >= 2 and l[0] != "#" and re.fullmatch(r"[0-9a-fA-F]+", p[1]) and 0x5dd000 <= int(p[1], 16) < 0x949d64: syms[p[0]] = int(p[1], 16)
unres = sorted(n for n, ds in decl.items() if n in syms and n not in gen_names and any(d["extern"] for d in ds))
with open(os.path.join(W, "types_report.txt"), "w") as f:
    def P(s): f.write(s + "\n"); print(s)
    P("T0 types.csv rows: %d; chosen by source: %s" % (len(rows), dict(collections.Counter(src.get(r["name"], ("default",))[0] for r in rows if r["kind"] != "pe"))))
    P("sizeof probe: %d non-builtin C types, %d unknown to the headers" % (len(nonb), sum(1 for v in psz.values() if v is None)))
    P("T1 names with C declarations in the generated range: %d; declarations with unknown type size: %d" % (sum(1 for n in decl if n in addr), unknown))
    P("T1 C type larger than the generated extent (overrun in waiting): %d" % len(over))
    for o in over[:60]: P("   %s: %s = %d bytes > %d available (%s)" % o)
    P("T1 float declared in C, integer in the generated data: %d %s" % (len(floatint), floatint[:20]))
    P("T1 names declared with different types in different files: %d %s" % (len(conflicts), conflicts[:8]))
    P("F5 LNK2005 (C defines a name that is also generated, with initialiser): %d %s" % (len(lnk2005), lnk2005[:20]))
    P("F5 tentative C definitions of generated names (COMMON): %d, bigger than the generated extent: %d %s" % (len(common), sum(1 for c in common if c[2] and c[2] > c[3]), [c for c in common if c[2] and c[2] > c[3]][:20]))
    P("F5 LNK2001: C extern data names (symbols.txt, data range) without a generated definition: %d %s" % (len(unres), unres[:30]))
    dec = [l.split(",")[1] for l in open(os.path.join(W, "ptrs.csv")).read().splitlines()[1:] if l.split(",")[2] != "_" + l.split(",")[1]]
    P("F5 decorated function targets (stdcall/fastcall defined by our C): %d distinct" % len(set(dec)))
sys.exit(1 if lnk2005 or any(c[2] and c[2] > c[3] for c in common) else 0)
