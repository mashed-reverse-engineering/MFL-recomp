#!/usr/bin/env python3
"""Second stage of ddmin_files.py: find the minimal set of FUNCTIONS of one recomp C file that makes the race diverge.
Usage: ddmin_funcs.py <regress work dir> <file.c>   (all other files are skipped, the functions are enabled with --replace)"""
import glob, os, re, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__))
RECOMP = os.path.join(HERE, "..", "..", "src")
work, target = sys.argv[1], sys.argv[2]
path = os.path.join(RECOMP, target)
if not os.path.exists(path):          # an autocompiled chunk lives in src/auto
    path = os.path.join(RECOMP, "auto", target)
src = open(path).read()
funcs = re.findall(r"^(?:__declspec\(naked\)\s+)?[A-Za-z_][\w\s\*]*?\b(\w+)\([^;{]*\)\s*\n\{", src, re.M)
files = sorted(os.path.basename(f) for f in glob.glob(os.path.join(RECOMP, "*.c")) + glob.glob(os.path.join(RECOMP, "auto", "*.c")))
skip = ",".join(f for f in files if f != target)
print("functions:", funcs)
cache = {}
def bad(en):
    k = tuple(sorted(en))
    if k not in cache:
        env = dict(os.environ, AUTO_FLAG="--auto" if os.sep + "auto" + os.sep in path else "", OVL_EXTRA="--replace " + ",".join(en))
        out = subprocess.run([os.path.join(HERE, "bisect_files.sh"), work, skip], capture_output=True, text=True, env=env).stdout.strip()
        cache[k] = out.startswith("BAD")
        print("test %s -> %s" % (",".join(en), out), flush=True)
    return cache[k]
items = list(funcs)
if bad(items):
    n = 2
    while len(items) >= 2:
        chunk = max(1, len(items) // n)
        subs = [items[i:i + chunk] for i in range(0, len(items), chunk)]
        red = False
        for s in subs:
            if bad(s):
                items, n, red = s, 2, True
                break
        if not red:
            for s in subs:
                comp = [x for x in items if x not in s]
                if comp and bad(comp):
                    items, n, red = comp, max(n - 1, 2), True
                    break
        if not red:
            if n >= len(items):
                break
            n = min(len(items), n * 2)
    print("minimal failing functions:", items)
else:
    print("whole file is OK with --replace")
