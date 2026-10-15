#!/usr/bin/env python3
"""field_dossier.py [-s BASE_REGEX] [-o OUT] [export.c]: per-field access dossier for every `field_0xNN` in the named export.
Groups by base (struct array / variable name), offset: read/write counts, functions, constants stored, strings and callees in the function, sample lines."""
import re, sys, collections, argparse
ap = argparse.ArgumentParser(); ap.add_argument("file", nargs="?", default="output/named/MFL_app_named.c")
ap.add_argument("-s", default="", help="regex on base name"); ap.add_argument("-o"); ap.add_argument("-n", type=int, default=6)
a = ap.parse_args()
src = open(a.file, errors="replace").read().split("\n")
FN = re.compile(r"^[A-Za-z_][\w \*]*?\b(\w+)\(.*\)\s*$")
ACC = re.compile(r"(\w+)(?:\[[^\]]*\])?(?:->|\.)field_0x([0-9a-f]+)")
D = collections.defaultdict(lambda: dict(r=0, w=0, fns=collections.Counter(), consts=collections.Counter(), types=collections.Counter(), calls=collections.Counter(), strs=collections.Counter(), lines=[]))
fn, body = None, []
def flush():
    if fn is None: return
    txt = "\n".join(b for _, b in body)
    strs = set(re.findall(r'"([^"]{3,40})"', txt)); calls = set(re.findall(r"\b([a-z_]\w+)\(", txt)) - {"if", "while", "for", "switch", "return", "sizeof"}
    for ln, l in body:
        for m in ACC.finditer(l):
            base, off = m.group(1), int(m.group(2), 16)
            d = D[(base, off)]
            after = l[m.end():]; before = l[:m.start()]
            w = re.match(r"\s*(?:[+\-*/|&^]|<<|>>)?=(?!=)", after) or re.search(r"(\+\+|--)\s*$", before)
            if w:
                d["w"] += 1
                c = re.match(r"\s*=\s*(-?(?:0x[0-9a-f]+|\d+(?:\.\d+)?f?))\s*;", after)
                if c: d["consts"][c.group(1)] += 1
            else: d["r"] += 1
            t = re.search(r"\((\w[\w ]*\*?)\)\s*[&(]*$", before)
            if t: d["types"][t.group(1)] += 1
            d["fns"][fn] += 1
            for s in strs: d["strs"][s] += 1
            for c in calls: d["calls"][c] += 1
            if len(d["lines"]) < a.n: d["lines"].append("%s:%d %s" % (fn, ln, l.strip()[:110]))
for i, l in enumerate(src, 1):
    if l and not l[0].isspace() and l[0] not in "{}/#" and "(" in l and not l.endswith(";"):
        flush(); m = re.search(r"\b(\w+)\(", l); fn, body = m.group(1), []
    elif fn: body.append((i, l))
flush()
out = open(a.o, "w") if a.o else sys.stdout
for (base, off), d in sorted(D.items(), key=lambda kv: (kv[0][0], kv[0][1])):
    if not re.search(a.s, base): continue
    top = lambda c, k=5: ", ".join("%s(%d)" % kv if kv[1] > 1 else kv[0] for kv in c.most_common(k))
    print("== %s +0x%x  r=%d w=%d  fns=%d" % (base, off, d["r"], d["w"], len(d["fns"])), file=out)
    print("  fns:    ", top(d["fns"], 8), file=out)
    if d["consts"]: print("  consts: ", top(d["consts"]), file=out)
    if d["types"]: print("  casts:  ", top(d["types"]), file=out)
    if d["strs"]: print("  strings:", top(d["strs"], 4), file=out)
    print("  callees:", top(d["calls"], 10), file=out)
    for l in d["lines"]: print("    ", l, file=out)
