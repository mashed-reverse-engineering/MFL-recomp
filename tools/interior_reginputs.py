#!/usr/bin/env python3
"""interior_reginputs.py -> output/reginputs_interior.csv `name,addr,regs,ret_n`: register inputs of call targets that are NOT the start of an ownership item.
reginputs2.py analyses one function per ownership item, but an unnamed `gap_` item can hold several functions (gap_455a40: 455aa0, 455b80, 455dd0 ...): their register
inputs were never computed (455dd0 reads ESI; course_city_cb7 / cb6 dropped `mov esi,1|0` before the call and the game misbehaved with diffemu EQUAL).
Each interior target T of a direct call / tail jmp is analysed as a function [T, item end) with the same CFG dataflow as reginputs2 (unreachable tail ignored); the callee table is
the reginputs2.csv rows plus the interior results (fixpoint).  diffemu core.Tables merges the rows into its register-callee table (name FUN_<addr>); tools/t2_auto.py rejects a
candidate that calls such a function without an adapter.   Usage: interior_reginputs.py [out]   (needs reginputs2.csv, objdump)"""
import csv, os, re, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HERE)
import reginputs2 as R
G = R.G
out = sys.argv[1] if len(sys.argv) > 1 else os.path.join(G, "output", "reginputs_interior.csv")
exe, secs = R.load_pe()
items = sorted((int(r["lo"], 16), int(r["hi"], 16), r["name"]) for r in csv.DictReader(open(R.OWN)) if r["kind"] == "code")
starts = {lo for lo, hi, n in items}
dump = subprocess.run(["objdump", "-w", "-M", "intel", "-D", "--no-show-raw-insn", "--start-address=0x401000", "--stop-address=0x5d7000", R.EXE], capture_output=True, text=True).stdout
ins, order = R.parse_objdump(dump)
import bisect
lo_list = [lo for lo, hi, n in items]
targets = {}
for a in order:
    op, ar = ins[a]
    if op == "call" and R.ADDR.match(ar.strip()):      # calls only: a `jmp` into the middle of an item is a label of that function (a tail jmp from another item is rare; ponytail: add it when a case shows up)
        t = int(ar.strip(), 16)
        k = bisect.bisect_right(lo_list, t) - 1
        if k >= 0 and t not in starts and items[k][0] < t < items[k][1] and t in ins: targets[t] = items[k]
# every function of a multi-function item ends at the next interior target, and the item START is a function of its own too: gap_455e60 [455e60,456080) holds 455ee0 / 455fd0, their
# `pop esi` made the whole item look like it saved ESI, so the `push esi` argument of 455e60 itself was no register input (course_city_cb1 lost `lea esi,[esp+0x20]`)
by_item = {}
for t0, it_ in list(targets.items()): by_item.setdefault(it_, []).append(t0)
for (ilo, ihi, iname), ts in by_item.items():
    pts = sorted(set([ilo] + ts))
    for k, t0 in enumerate(pts): targets[t0] = (ilo, pts[k + 1] if k + 1 < len(pts) else ihi, iname)
callee, calleeN = {}, {}
for r in csv.DictReader(open(os.path.join(G, "output", "reginputs2.csv"))):
    callee[int(r["addr"], 16)] = {x for x in r["reg_inputs"].split("|") if x}; calleeN[int(r["addr"], 16)] = int(r["ret_n"] or 0)
tb = R.tables(0x401000, 0x5d7000, ins, order, exe, secs)
res = {}
for it in range(6):
    changed = False
    for t, (lo, hi, n) in sorted(targets.items()):
        r = R.analyse(t, hi, ins, order, tb, callee, calleeN, {})
        if r is None: continue
        res[t] = r; calleeN[t] = r["retn"]
        if callee.get(t) != set(r["inputs"]): callee[t] = set(r["inputs"]); changed = True
    if not changed: break
with open(out, "w") as f:
    f.write("name,addr,regs,ret_n\n")
    for t, r in sorted(res.items()):
        if r["inputs"]: f.write("FUN_%08x,%08x,%s,%d\n" % (t, t, "|".join(r["inputs"]), r["retn"]))
print("%d interior call targets, %d with register inputs -> %s" % (len(targets), sum(1 for r in res.values() if r["inputs"]), out), file=sys.stderr)
