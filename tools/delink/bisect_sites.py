#!/usr/bin/env python3
"""Find a falsely relocated site by bisection: build the relinked exe with a subset of candidate sites left unrelocated (mkobj --skip) and run one
course crash-only (race_relinked.sh).  Outcome per test: ok / same crash (crash text contains MATCH) / other (trap or another crash = the subset
holds real pointers).  Assumes one culprit; narrows the candidate list while the outcome is "ok" for the skipped half.
Usage: bisect_sites.py <work dir> <candidates file (hex sites)> <course> <delta hex> <crash match> [mkobj args...]   (env PAD, RUN per test ~70 s)"""
import os, subprocess, sys
H = os.path.dirname(os.path.abspath(__file__)); W, CF, C, D, MATCH = sys.argv[1:6]; EXTRA = " ".join(sys.argv[6:])
cand = [l.split()[0] for l in open(CF) if l.strip()]
n = 0
def test(skip):
    global n
    n += 1; d = os.path.join(W, "t%02d" % n); os.makedirs(d, exist_ok=True)
    open(os.path.join(d, "skip.txt"), "w").write("\n".join(skip) + "\n")
    env = dict(os.environ, MKOBJ_ARGS=(EXTRA + " --skip " + os.path.join(d, "skip.txt")).strip())
    subprocess.run(["timeout", "300", os.path.join(H, "roundtrip.sh"), d], env=env, capture_output=True)
    subprocess.run(["bash", "-c", "until [ $(cut -d' ' -f1 /proc/loadavg | cut -d. -f1) -lt 6 ]; do sleep 5; done"])
    r = subprocess.run([os.path.join(H, "race_relinked.sh"), os.path.join(d, "game"), os.path.join(d, "r"), D, C], capture_output=True, text=True).stdout.strip()
    res = "ok" if r.endswith(": ok") else "same" if MATCH in r else "other"
    print("test %d: skip %d sites -> %s (%s)" % (n, len(skip), res, r[:120]), flush=True)
    return res
while len(cand) > 1:
    a, b = cand[:len(cand) // 2], cand[len(cand) // 2:]
    ra = test(a)
    if ra == "ok": cand = a; continue
    rb = test(b)
    if rb == "ok": cand = b; continue
    print("no half fixes it (%s / %s): stopping with %d candidates" % (ra, rb, len(cand))); break
print("candidates left:", cand[:20])
