#!/usr/bin/env python3
"""agent_verify.py: re-checks every file of src/automatch_agent/ standalone with check_one.py (byte identical to the original at its address, relocations hidden) and writes
output/agent_verify.txt: name RESULT sha1-of-file.  tools/tier_ledger.py takes the 'agent' evidence from there (recorded, not assumed).  Usage: agent_verify.py [jobs=4]"""
import glob, hashlib, os, re, subprocess, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__))); import cheatgrep
from concurrent.futures import ThreadPoolExecutor
HERE = os.path.dirname(os.path.abspath(__file__)); R = os.path.join(HERE, "..", "src")
addrs = {}
for l in open(os.path.join(R, "symbols.txt")):
    p = l.split()
    if len(p) == 2: addrs.setdefault(p[0], p[1])
def one(f):
    n = os.path.basename(f)[3:-2]
    va = addrs.get(n)
    m = re.search(r"_([0-9a-f]{8})$", n)
    if m: va = m.group(1)      # a name that exists at two addresses carries its address
    d = open(os.path.join(R, "..", "output", "agent_vas.txt")).read() if False else ""
    al = cheatgrep.allow(); ch = [1 for k, fn_, ln in cheatgrep.scan(f) if os.path.basename(f) not in al and not any(x in al for x in fn_.split(","))]
    if ch: return "%s FAIL:cheat %s" % (n, hashlib.sha1(open(f, "rb").read()).hexdigest()[:12])
    r = subprocess.run([sys.executable, os.path.join(HERE, "check_one.py"), f, n] + (["--va=0x" + va] if va else []), capture_output=True, text=True, timeout=400)
    return "%s %s %s" % (n, "MATCH" if r.returncode == 0 else "FAIL", hashlib.sha1(open(f, "rb").read()).hexdigest()[:12])
files = sorted(glob.glob(os.path.join(R, "automatch_agent", "ag_*.c")))
with ThreadPoolExecutor(int(sys.argv[1]) if len(sys.argv) > 1 else 4) as ex: out = list(ex.map(one, files))
open(os.path.join(HERE, "..", "output", "agent_verify.txt"), "w").write("\n".join(out) + "\n")
print(sum(1 for o in out if " MATCH " in o), "of", len(out), "match standalone")
