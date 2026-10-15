#!/usr/bin/env python3
"""Disjoint group search (Opus speed review): split the function list into groups of G, test all groups in parallel, then split only the BAD ones further (binary, halves tested in parallel).
No restart from the whole set after a culprit: about n/G + culprits * log2(G) tests instead of 15-20 tests per culprit.  Every test enables ONLY the given functions (`--replace`).
Usage: groupsearch.py <regress work dir> <names.txt> <out survivors.txt> [group size=12]     env: PAR (default 4 slots), same oracle env as ddmin_multi.py (params.env supplies FRAMES, OVL_ARGS ...)
Writes <out> (functions never in a BAD single test), <out>.culprits (single functions that fail alone) and <out>.state.json (test cache, a rerun resumes)."""
import json, os, subprocess, sys, threading, time
from concurrent.futures import ThreadPoolExecutor
HERE = os.path.dirname(os.path.abspath(__file__))
work, names_f, out_f = sys.argv[1:4]
G = int(sys.argv[4]) if len(sys.argv) > 4 else 12
PAR = int(os.environ.get("PAR", "4"))
names = [l.strip() for l in open(names_f) if l.strip()]
state_p = out_f + ".state.json"
cache = json.load(open(state_p)) if os.path.exists(state_p) else {}
lock = threading.Lock(); slots = list(range(1, PAR + 1)); slot_lock = threading.Lock()
culprits = []
T0 = time.time(); MAXWALL = int(os.environ.get("MAXWALL", "2400")); NOK = [0]; NBAD = [0]      # hard limits: wall seconds, and "no test ever passed" (the oracle is noise, e.g. win / lose scenarios)

def abort(why):
    print("ABORT:", why, flush=True); os._exit(4)

def test(items):
    key = ",".join(sorted(items))
    with lock:
        if key in cache: return cache[key]
    with slot_lock: slot = slots.pop()
    try:
        env = dict(os.environ, OVL_EXTRA="--replace " + ",".join(items), BRIDGE_ENABLED=",".join(items), SLOT=str(slot))
        t0 = time.time()
        for attempt in range(4):
            out = subprocess.run([os.path.join(HERE, "bisect_files.sh"), work, ""], capture_output=True, text=True, env=env).stdout.strip().split("\n")[-1]
            if not out.startswith(("ERROR", "BUILDFAIL")): break
            subprocess.run(["docker", "rm", "-f", "mfl_bis%d" % slot], capture_output=True)
        else:
            raise SystemExit("the oracle fails repeatedly: " + out)
    finally:
        with slot_lock: slots.append(slot)
    bad = out.startswith("BAD")
    with lock:
        if bad: NBAD[0] += 1
        else: NOK[0] += 1
        if time.time() - T0 > MAXWALL: abort("MAXWALL %d s exceeded" % MAXWALL)
        if NBAD[0] >= 40 and NOK[0] == 0: abort("40 tests and none passed: the oracle is too noisy for this scenario (use THRESH=100000, crash only)")
        cache[key] = bad
        print("[%s] %3d functions (slot %d) -> %s (%.0f s)" % (time.strftime("%H:%M:%S"), len(items), slot, out[:110], time.time() - t0), flush=True)
        json.dump(cache, open(state_p, "w"))
    return bad

def minimize(items):
    n = 2
    while len(items) >= 2:
        sz = max(1, len(items) // n); chunks = [items[i:i + sz] for i in range(0, len(items), sz)]
        comps = [[x for x in items if x not in c] for c in chunks]
        res = [RUN.submit(test, c) for c in comps if c]
        hit = next((c for c, f in zip([c for c in comps if c], res) if f.result()), None)
        if hit is not None: items, n = hit, max(n - 1, 2)
        elif n >= len(items): break
        else: n = min(len(items), n * 2)
    return items

def search(items, pool):
    run = RUN
    """items are known to fail as a group: find the single functions that fail alone"""
    if len(items) == 1:
        with lock: culprits.append(items[0]); print("culprit:", items[0], flush=True)
        return
    h = len(items) // 2
    a, b = items[:h], items[h:]
    fa, fb = run.submit(test, a), run.submit(test, b)
    ra, rb = fa.result(), fb.result()
    if ra: search(a, pool)          # recursion in this thread (one thread per failing top level group): nested pool tasks waiting for each other exhausted the pool and deadlocked
    if rb: search(b, pool)
    if not ra and not rb:             # fails only together: shrink to a 1-minimal failing subset (ddmin with complements), only that subset is lost
        m = minimize(items)
        with lock: culprits.extend(m); print("culprit group (fails only together, minimal %d of %d):" % (len(m), len(items)), " ".join(m), flush=True)

RUN = ThreadPoolExecutor(PAR)       # only tests run here (one game per slot); the search tasks live in `pool` and just wait
with ThreadPoolExecutor(64) as pool, RUN as run:
    groups = [names[i:i + G] for i in range(0, len(names), G)]
    # tests are limited by the slot list; the search tasks only wait for them
    results = [(g, run.submit(test, g)) for g in groups]
    bad_groups = [g for g, f in results if f.result()]
    print("%d of %d groups fail as a group" % (len(bad_groups), len(groups)), flush=True)
    futs = [pool.submit(search, g, pool) for g in bad_groups]
    for f in futs: f.result()
surv = [n for n in names if n not in set(culprits)]
open(out_f, "w").write("\n".join(surv) + "\n"); open(out_f + ".culprits", "w").write("\n".join(culprits) + "\n")
print("done: %d culprits, %d survivors -> %s" % (len(culprits), len(surv), out_f))
