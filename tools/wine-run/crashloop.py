#!/usr/bin/env python3
"""Remove crashing functions one by one using the crash address: test the whole set (bisect_files.sh with --replace <all names>); when the run crashes in one of OUR functions
(`BAD (crash: page fault at ... in <function>+0x..`) that function is dropped and the test repeated; when it passes, or the failure is not a crash in one of the names (hang, crash in original code),
the loop stops and tells why.  Usage: crashloop.py <regress work dir> <names.txt> <out survivors.txt> [max rounds=80]    (same env as ddmin_multi.py: OVL_ARGS, FRAMES ... come from params.env)"""
import os, re, subprocess, sys, time
HERE = os.path.dirname(os.path.abspath(__file__))
work, names_f, out_f = sys.argv[1:4]
maxr = int(sys.argv[4]) if len(sys.argv) > 4 else 80
names = [l.strip() for l in open(names_f) if l.strip()]
dropped = []
T0 = time.time(); MAXWALL = int(os.environ.get("MAXWALL", "2400"))
for r in range(maxr):
    if time.time() - T0 > MAXWALL: print("ABORT: MAXWALL %d s" % MAXWALL); break
    env = dict(os.environ, OVL_EXTRA="--replace " + ",".join(names), SLOT="1")
    t0 = time.time()
    try: out = subprocess.run([os.path.join(HERE, "bisect_files.sh"), work, ""], capture_output=True, text=True, env=env, timeout=int(os.environ.get("TEST_TIMEOUT", "900"))).stdout.strip().split("\n")[-1]
    except subprocess.TimeoutExpired: out = "ERROR timeout"
    print("[%s] round %d: %d functions -> %s (%d s)" % (time.strftime("%H:%M:%S"), r, len(names), out[:150], time.time() - t0), flush=True)
    if out.startswith("ERROR"): continue
    if not out.startswith("BAD"): break
    m = re.search(r"in (\w+)\+0x", out)
    if m and m.group(1) in names:
        dropped.append(m.group(1)); names.remove(m.group(1)); continue
    print("stopped: the failure does not name one of the functions:", out)
    break
open(out_f, "w").write("\n".join(names) + "\n")
print("dropped (%d): %s" % (len(dropped), " ".join(dropped)))
print("%d survivors -> %s" % (len(names), out_f))
