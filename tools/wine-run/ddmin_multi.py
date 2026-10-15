#!/usr/bin/env python3
"""Find ALL functions of a set that make the regress oracle fail (stdlib only).  Usage: ddmin_multi.py <regress work dir> <names.txt> [out.json]
bisect_files.sh is the oracle (same env: SAMELAYOUT, OVL_ARGS, COURSE, FRAMES, THRESH, DRIVE ...).  A test enables only the given functions (`--replace`).
Algorithm: if the whole remaining set fails, halve it until a failing group of one is found (or two halves that only fail together: that group is kept as one
unit), set it aside as culprit, repeat with the rest until the rest passes.  Progress (culprits, test cache) is saved after every test; rerunning resumes."""
import json, os, subprocess, sys, threading, time
from concurrent.futures import ThreadPoolExecutor
HERE = os.path.dirname(os.path.abspath(__file__))
work, names = sys.argv[1], [l.strip() for l in open(sys.argv[2]) if l.strip()]
state_path = sys.argv[3] if len(sys.argv) > 3 else os.path.join(work, "ddmin_multi.json")
state = json.load(open(state_path)) if os.path.exists(state_path) else {"culprits": [], "cache": {}}
def save(): json.dump(state, open(state_path, "w"), indent=1)
lock = threading.Lock()
T0 = time.time(); MAXWALL = int(os.environ.get("MAXWALL", "2400"))      # hard wall clock limit (a runaway search must stop by itself)
def bad(items, slot=None):
    key = ",".join(sorted(items))
    with lock:
        if key in state["cache"]: return state["cache"][key]
    if time.time() - T0 > MAXWALL: print("ABORT: MAXWALL %d s" % MAXWALL, flush=True); save(); os._exit(4)
    env = dict(os.environ, OVL_EXTRA="--replace " + ",".join(items), BRIDGE_ENABLED=",".join(items))      # stubs of these, and only these run as C (calls to the others go to the original)
    if slot: env["SLOT"] = str(slot)
    t0 = time.time()
    for attempt in range(4):
        try: out = subprocess.run([os.path.join(HERE, "bisect_files.sh"), work, ""], capture_output=True, text=True, env=env, timeout=int(os.environ.get("TEST_TIMEOUT", "900"))).stdout.strip().split("\n")[-1]
        except subprocess.TimeoutExpired: out = "ERROR timeout"
        if not out.startswith(("ERROR", "BUILDFAIL")): break
        print("[%s] %3d functions (slot %s) -> %s, retry %d" % (time.strftime("%H:%M:%S"), len(items), slot or "-", out, attempt + 1), flush=True)
        if slot: subprocess.run(["docker", "rm", "-f", "mfl_bis%s" % slot], capture_output=True)
        else: subprocess.run(["docker", "rm", "-f", "mfl_bis"], capture_output=True)
    else:
        raise SystemExit("the oracle fails repeatedly: " + out)
    with lock:
        state["cache"][key] = out.startswith("BAD")
        print("[%s] %3d functions (slot %s) -> %s (%.0f s)" % (time.strftime("%H:%M:%S"), len(items), slot or "-", out, time.time() - t0), flush=True)
        save()
        return state["cache"][key]
PAR = int(os.environ.get("PAR", "4"))
def bad_parts(parts):
    with ThreadPoolExecutor(len(parts)) as ex:
        return list(ex.map(lambda ip: bad(ip[1], ip[0] + 1), enumerate(parts)))
culprit_names = {n for c in state["culprits"] for n in c}
rest = [n for n in names if n not in culprit_names]
while rest and bad(rest):
    items = list(rest)
    while len(items) > 1:
        k = min(PAR, len(items))
        parts = [items[i * len(items) // k:(i + 1) * len(items) // k] for i in range(k)]      # k parts, tested at the same time in k slots
        res = bad_parts(parts)
        if any(res): items = parts[res.index(True)]
        else: break                       # only the parts together fail: keep the group
    state["culprits"].append(items); save()
    print("culprit:", " ".join(items), flush=True)
    rest = [n for n in rest if n not in items]
print("done: %d culprit group(s), %d functions pass together" % (len(state["culprits"]), len(rest)))
open(os.path.join(work, "ddmin_multi_pass.txt"), "w").write("\n".join(rest) + "\n")
open(os.path.join(work, "ddmin_multi_culprits.txt"), "w").write("\n".join(" ".join(c) for c in state["culprits"]) + "\n")
