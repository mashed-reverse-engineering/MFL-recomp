#!/usr/bin/env python3
"""split_tier1.py <out dir> [jobs=4]: converts the tier 1 chunk files (src/automatch/am_*.c, many functions per file) into ONE FILE PER FUNCTION (extract_fn.py + the declarations the function uses), checks every file
standalone with check_one.py (byte identical at its address) and keeps only the ones that match.  Result: <out dir>/<function>.c and <out dir>/split_report.txt (name, va, MATCH / FAIL).  Per function files are what the
readability pass edits and what the final source tree is made of; the chunks are the machine output stage."""
import os, subprocess, sys, shutil
from concurrent.futures import ThreadPoolExecutor
HERE = os.path.dirname(os.path.abspath(__file__)); R = os.path.join(HERE, "..", "src")
out = os.path.abspath(sys.argv[1]); jobs = int(sys.argv[2]) if len(sys.argv) > 2 else 4
os.makedirs(out, exist_ok=True)
for h in ("prelude.h", "ghidra_types.h"): shutil.copy(os.path.join(R, "auto", h), out)
items = []
for l in open(os.path.join(R, "automatch_names.txt")):
    p = l.split()
    if len(p) >= 3: items.append((p[1], p[0], p[2]))      # name, va, chunk
def one(it):
    name, va, chunk = it
    dst = os.path.join(out, name + ".c")
    src = os.path.join(R, "automatch", "am_" + chunk if not chunk.startswith("am_") else chunk)
    if not os.path.exists(src): src = os.path.join(R, "automatch", chunk)
    r = subprocess.run([sys.executable, os.path.join(HERE, "extract_fn.py"), src, name, dst], capture_output=True, text=True)
    if r.returncode: return "%s %s EXTRACT-FAIL" % (name, va)
    r = subprocess.run([sys.executable, os.path.join(HERE, "check_one.py"), dst, name, "--va=0x" + va], capture_output=True, text=True, timeout=600)
    if r.returncode != 0: os.rename(dst, dst + ".fail"); return "%s %s FAIL" % (name, va)
    return "%s %s MATCH" % (name, va)
with ThreadPoolExecutor(jobs) as ex: res = list(ex.map(one, items))
open(os.path.join(out, "split_report.txt"), "w").write("\n".join(res) + "\n")
print(sum(1 for r in res if r.endswith("MATCH")), "of", len(res), "functions match standalone ->", out)
