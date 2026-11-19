#!/usr/bin/env python3
"""private_layout.py <public checkout>: W16 CIN.  The link tools read the private tree's paths (output/..., src/automatch_near/...; src/ is
already src/ in the exported tools).  This adds a copy of every exported file at its private path, IN PLACE (the export paths stay, so
data/link/* and src/tier2/* are still there), and joins the two delink_relocs parts.  Run once in the CI link job before tools/delink/link_ci.sh.
The map is the inverse of tools/export_public.py STAGES for the renamed entries (W14 BLD agents/bld_w14/mkwork.py MAP); a stage that adds a
renamed entry adds its line here."""
import os, shutil, sys
R = os.path.abspath(sys.argv[1])
# export path prefix -> private path prefix (first match wins); None = no copy
MAP = [("src/tier2/names.txt", "src/automatch_near_names.txt"), ("src/tier2/", "src/automatch_near/"),
       ("data/link/delink_relocs.part", None), ("data/link/decls_float.txt", "output/decls.h"),
       ("data/link/", "output/"), ("data/clusters_enabled.txt", None), ("data/", "output/")]
def dst(rel):
    for a, b in MAP:
        if rel.startswith(a): return None if b is None else b + rel[len(a):] if a.endswith("/") else b
    return rel
n = 0
for root, dirs, fs in os.walk(R):
    dirs[:] = [d for d in dirs if not (root == R and d in (".git", "output"))]      # output/ = what this script writes
    for f in fs:
        rel = os.path.relpath(os.path.join(root, f), R); d = dst(rel)
        if d is None or d == rel: continue
        os.makedirs(os.path.dirname(os.path.join(R, d)), exist_ok=True); shutil.copy2(os.path.join(R, rel), os.path.join(R, d)); n += 1
parts = sorted(f for f in os.listdir(os.path.join(R, "data/link")) if f.startswith("delink_relocs.part"))
assert parts, "no data/link/delink_relocs.part*.csv"
with open(os.path.join(R, "output/delink_relocs.csv"), "w") as o:      # the two parts, header once
    for i, p in enumerate(parts):
        ls = open(os.path.join(R, "data/link", p)).readlines(); o.writelines(ls if i == 0 else ls[1:])
print("private_layout: %d copies, output/delink_relocs.csv from %d parts" % (n, len(parts)))
