#!/usr/bin/env python3
"""reorg_apply.py <out dir>: run tools/reorg.py until no function of src/manifest.txt gets a worse match status than before (stdlib only).
Sources with a regression stay as they are (`--legacy`).  Result: <out dir>/{modules..., manifest.txt, moved.csv} and a summary; nothing in src/ is touched."""
import csv, os, shutil, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__))
RECOMP = os.path.join(HERE, "..", "src")
out = os.path.abspath(sys.argv[1])
RANK = {"MATCH": 3, "EFFECTIVE": 2, "NEAR": 1, "DIFFER": 0}


def statuses(manifest):
    r = subprocess.run([sys.executable, os.path.join(HERE, "cmpfn.py"), "--manifest", manifest], capture_output=True, text=True)
    d = {}
    for l in r.stdout.splitlines():
        t = l.split()
        if len(t) >= 2 and t[0] in RANK: d[t[1]] = t[0]
    return d


base = statuses(os.path.join(RECOMP, "manifest.txt"))
legacy = set()
for it in range(12):
    os.makedirs(out, exist_ok=True)
    cmd = [sys.executable, os.path.join(HERE, "reorg.py"), out] + (["--legacy", ",".join(sorted(legacy))] if legacy else [])
    print(subprocess.run(cmd, capture_output=True, text=True).stdout.strip())
    for h in os.listdir(RECOMP):
        if h.endswith(".h"): shutil.copy(os.path.join(RECOMP, h), os.path.join(out, h))
    mv = {r["function"]: r for r in csv.DictReader(open(os.path.join(out, "moved.csv")))}
    subprocess.run([sys.executable, os.path.join(HERE, "reorg_manifest.py"), os.path.join(out, "moved.csv"), os.path.join(RECOMP, "manifest.txt"), os.path.join(out, "manifest.txt")], capture_output=True)
    new = statuses(os.path.join(out, "manifest.txt"))
    worse = [n for n in base if n in new and RANK[new[n]] < RANK[base[n]]]
    missing = [n for n in base if n not in new]
    print("iteration %d: %d functions worse, %d missing, legacy sources %d" % (it + 1, len(worse), len(missing), len(legacy)))
    if not worse and not missing:
        break
    add = {mv[n]["old"] for n in worse + missing if n in mv and mv[n]["old"] not in legacy}
    if not add:
        print("no source to keep for:", worse + missing); break
    legacy |= add
    print("keeping as they are:", ", ".join(sorted(add)))
print("done; legacy sources:", ", ".join(sorted(legacy)))
