#!/usr/bin/env python3
"""permute_all.py [rounds] [variants] [seed]: run the source permuter (tools/permute.py) over every NEAR / EFFECTIVE function of src/manifest.txt and adopt a variant
only when it is byte identical (score 1.0): the function text in its module is replaced, the manifest line becomes MATCH.  Log: permute_all.log next to the manifest.
Everything else is left as it is.  Uses its own compiler container and Wine prefix (VC7_CONTAINER=permute, VC7_PFX=wine_pfx_ccp)."""
import os, re, subprocess, sys, time
HERE = os.path.dirname(os.path.abspath(__file__))
RECOMP = os.path.join(HERE, "..", "src")
rounds = sys.argv[1] if len(sys.argv) > 1 else "8"
nvar = sys.argv[2] if len(sys.argv) > 2 else "50"
seed = sys.argv[3] if len(sys.argv) > 3 else "1"
env = dict(os.environ, VC7_CONTAINER="permute", VC7_PFX="wine_pfx_ccp")
st = subprocess.run([sys.executable, os.path.join(HERE, "cmpfn.py"), "--manifest", os.path.join(RECOMP, "manifest.txt")], capture_output=True, text=True).stdout
todo = [l.split()[1] for l in st.splitlines() if l.startswith(("NEAR", "EFFECTIVE"))]
rows = {}
for l in open(os.path.join(RECOMP, "manifest.txt")):
    p = l.split()
    if len(p) > 3 and p[0] not in ("#", "(none)"): rows[p[1]] = (p[0], p[2], l)
log = open(os.path.join(RECOMP, "permute_all.log"), "a")
def say(m):
    print(m, flush=True); log.write(m + "\n"); log.flush()
for name in todo:
    if name not in rows: continue
    f, va, line = rows[name]
    path = os.path.join(RECOMP, f)
    t0 = time.time()
    r = subprocess.run([sys.executable, os.path.join(HERE, "permute.py"), path, name, va, rounds, nvar, seed], capture_output=True, text=True, env=env)
    scores = re.findall(r"^(?:start|round \d+) ([0-9.]+)", r.stdout, re.M)
    best = float(scores[-1]) if scores else 0.0
    bestfile = path[:-2] + ".best.c"
    if best >= 0.9999 and os.path.exists(bestfile):
        # adopt: the function text of the best file replaces the one in the module
        import permute
        _, newfn, _ = permute.split_function(open(bestfile).read(), name)
        pre, oldfn, post = permute.split_function(open(path).read(), name)
        open(path, "w").write(pre + newfn + post)
        chk = subprocess.run([sys.executable, os.path.join(HERE, "cmpfn.py"), path, name, va], capture_output=True, text=True, env=env).stdout
        if " MATCH" in chk:
            m = open(os.path.join(RECOMP, "manifest.txt")).read().replace(line, "%s %s %s /O2 MATCH\n" % (f, name, va) if False else re.sub(r"\s+(DIFFERENT|EFFECTIVE|NEAR).*$", " MATCH", line.rstrip("\n")) + "\n", 1)
            open(os.path.join(RECOMP, "manifest.txt"), "w").write(m)
            say("%-45s ADOPTED (score %.4f, %.0f s)" % (name, best, time.time() - t0))
        else:
            open(path, "w").write(pre + oldfn + post)          # did not verify: restore
            say("%-45s variant did not verify, restored (%s)" % (name, chk.strip()[-80:]))
    else:
        say("%-45s best score %.4f (%.0f s)" % (name, best, time.time() - t0))
    if os.path.exists(bestfile): os.remove(bestfile)
