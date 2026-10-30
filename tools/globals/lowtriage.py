#!/usr/bin/env python3
"""F2: triage of low-confidence ('value only') relocation words found by globgen.  Usage: lowtriage.py <globgen outdir> [--write]
A low word becomes a pointer only if it is on src/globals_reloc_allow.txt.  Rule set (evidence of a real pointer; reviewed by sampling the groups):
  base   reloc kind data/fntable/string, resolved to a symbol or generated item (never code_interior: odd code offsets, 118+ false positives seen)
  align  value % 4 == 0 (string pointers excepted): rejects colours / packed constants such as 0x005dffff, 0x00804000
  proof  one of: target is exactly a symbol/item start (E1); another non-low reloc (code or data) targets the same address (E2);
         a neighbouring word (+-4) is a medium/high pointer (E3); the same target value occurs in >= 3 low words (E4, jump tables / node pools); E5 the word lies in a library span (variant L), E6 it points into its own item
Without --write only the counts are printed.  Remaining low words stay raw numbers (listed in raw.csv, reason 'low')."""
import collections, csv, os, sys
G = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
W = sys.argv[1]
rs = list(csv.DictReader(open(os.path.join(W, "low_pointers.csv"))))
nl = {int(r["target"], 16) for r in csv.DictReader(open(os.path.join(G, "output/delink_relocs.csv"))) if r["confidence"] != "low" and r["kind"] not in ("const", "header")}
lowc = collections.Counter(int(r["value"], 16) for r in rs)
ok, rest = [], collections.Counter()
for r in rs:
    v = int(r["value"], 16)
    if r["reloc_kind"] == "code_interior": rest["code_interior"] += 1; continue
    if r["kind"] == "raw": rest["unresolved"] += 1; continue
    if r["item"].startswith("g_libspan_"): ok.append((r, ["E5"])); continue          # library spans: the delinker (mkobj) moved every such word and 15/15 courses ran; a stride-16 table of qhull / D3DX node pointers stayed raw and crashed (T-E3)
    if r["item"] == r["symbol"] and v % 4 == 0: ok.append((r, ["E6"])); continue      # self-referential (list heads, node pools inside one object)
    if v % 4 and r["reloc_kind"] != "string": rest["unaligned data word"] += 1; continue
    nb = r["prev_conf"] in ("high", "medium") or r["next_conf"] in ("high", "medium")
    ev = [n for n, c in (("E1", r["off"] == "0"), ("E2", v in nl), ("E3", nb), ("E4", lowc[v] >= 3)) if c]
    if not ev: rest["aligned, no proof"] += 1; continue
    ok.append((r, ev))
print("low words: %d, allowed: %d, stay raw: %d %s" % (len(rs), len(ok), len(rs) - len(ok), dict(rest)))
print("allowed by proof:", dict(collections.Counter(",".join(e) for _, e in ok)))
if "--write" in sys.argv:
    with open(os.path.join(G, "src", "globals_reloc_allow.txt"), "w") as f:
        f.write("# Reviewed low-confidence relocation words for tools/globals/globgen.py (F2). Written by tools/globals/lowtriage.py --write; edit by hand only through that tool's rules.\n# site  proof  item -> symbol+off\n")
        for r, ev in sorted(ok, key=lambda x: x[0]["site"]):
            f.write("%s %s %s -> %s+%s\n" % (r["site"], ",".join(ev), r["item"], r["symbol"], r["off"]))
