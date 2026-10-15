#!/usr/bin/env python3
"""Find a minimal set of src/*.c files (and src/auto chunks) that makes the deterministic race diverge from the control.
Usage: ddmin_files.py <regress work dir (with snap_ovl, script_ovl.txt, out_ctrl_1)>
Calls bisect_files.sh with SKIP_FILES = every file that is not in the tested subset (delta debugging); prints the minimal failing set."""
import glob, os, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
RECOMP = os.path.join(HERE, "..", "..", "src")
work = sys.argv[1]
files = sorted(os.path.basename(f) for f in glob.glob(os.path.join(RECOMP, "*.c")) + glob.glob(os.path.join(RECOMP, "auto", "*.c")))
cache = {}

def bad(enabled):
    key = tuple(sorted(enabled))
    if key not in cache:
        skip = ",".join(f for f in files if f not in enabled) or "x_none.c"
        env = dict(os.environ, AUTO_FLAG="")
        out = subprocess.run([os.path.join(HERE, "bisect_files.sh"), work, skip], capture_output=True, text=True, env=env).stdout.strip()
        cache[key] = out.startswith("BAD")
        print("test %2d files -> %s" % (len(enabled), out), flush=True)
    return cache[key]

def ddmin(items):
    n = 2
    while len(items) >= 2:
        chunk = max(1, len(items) // n)
        subsets = [items[i:i + chunk] for i in range(0, len(items), chunk)]
        reduced = False
        for s in subsets:
            if bad(s):
                items, n, reduced = s, 2, True
                break
        if not reduced:
            for s in subsets:
                comp = [x for x in items if x not in s]
                if comp and bad(comp):
                    items, n, reduced = comp, max(n - 1, 2), True
                    break
        if not reduced:
            if n >= len(items):
                break
            n = min(len(items), n * 2)
    return items

if bad(files):
    print("full set is BAD; minimising", len(files), "files")
    print("minimal failing set:", ddmin(files))
else:
    print("full set is OK")
