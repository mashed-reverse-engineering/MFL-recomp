#!/usr/bin/env python3
"""new_fn.py: byte-match work for Rwp functions that have no tier-2 chunk (T-P4).  Work dir (env RWP_WORK, default $TMPDIR/rwp_new) holds
  <name>.c      the candidate (first line `/* compile: /G5 */` for library code), one function per file, the function named like its alias in src/aliases.txt
  desc.txt      `name|one-line description` (install adds it as a comment above the function)
Usage:
  new_fn.py check <name> <va-hex> [flags...]     compile + compare one function (default flags: /O2 /G5), prints the assembly diff
  new_fn.py perm  <name>:<va> ... [permute opts]  tools/bytematch_permute.py on the work-dir files (names mapped to <name>.c; add --frame for the frame mutators)
  new_fn.py install <name>...                     copy the verified files to src/tier1/<name>.c (needs a desc.txt line and a MATCH)
tools/check_all.sh and `flock <lock> tools/gate.sh tier1` afterwards."""
import os, re, shutil, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__)); G = os.path.join(HERE, "..", ".."); T = os.path.join(G, "tools")
W = os.environ.get("RWP_WORK") or os.path.join(os.environ.get("TMPDIR", "/tmp"), "rwp_new")
sys.path.insert(0, T)
os.makedirs(W, exist_ok=True)


def check(name, va, flags):
    env = dict(os.environ, VA="0x" + va)
    return subprocess.run([sys.executable, os.path.join(T, "check_one.py"), os.path.join(W, name + ".c"), name] + (flags or ["/O2", "/G5"]), capture_output=True, text=True, env=env)


def main(a):
    if not a: sys.exit(__doc__)
    if a[0] == "check":
        r = check(a[1], a[2], a[3:]); print(r.stdout[-3000:], r.stderr[-500:]); sys.exit(r.returncode)
    if a[0] == "perm":
        import bytematch_permute as bp, bytediff_causes as bc
        nm = {}; rest = []
        for x in a[1:]:
            if ":" in x and not x.startswith("-"): n, v = x.split(":"); nm[n] = (int(v, 16), n + ".c")
            else: rest.append(x)
        bc.names = lambda: nm
        for h in os.listdir(os.path.join(G, "src")):
            if h.endswith(".h"): shutil.copy(os.path.join(G, "src", h), W)
        sys.argv = ["x", "--work", os.path.join(W, "pw"), "--src", W, "--names", ",".join(nm)] + rest
        bp.main(); return
    if a[0] == "install":
        import bytematch_rewrite as rw
        desc = dict(l.rstrip("\n").split("|", 1) for l in open(os.path.join(W, "desc.txt")) if "|" in l)
        va_of = {}
        for f in ("aliases.txt", "symbols.txt"):
            for l in open(os.path.join(G, "src", f)):
                p = l.split()
                if len(p) == 2: va_of.setdefault(p[0], p[1][-6:])
        for n in a[1:]:
            txt = open(os.path.join(W, n + ".c")).read()
            m = next((m for m in rw.DEF.finditer(txt) if m.group(1) == n), None)
            if not m or n not in desc or n not in va_of: print("SKIP", n, "(definition, description or alias missing)"); continue
            r = check(n, va_of[n], None)
            if not r.stdout.startswith("MATCH"): print("SKIP", n, "no match"); continue
            dst = os.path.join(G, "src", "tier1", n + ".c")
            if os.path.exists(dst): print("EXISTS", n); continue
            open(dst, "w").write(txt[:m.start()] + "/* %s (Rwp library) */\n" % desc[n] + txt[m.start():]); print("installed", n)
        return
    sys.exit(__doc__)


main(sys.argv[1:])
