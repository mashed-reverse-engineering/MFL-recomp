#!/bin/bash
# status.sh: deterministic summary of output/tier_ledger.csv (run tools/tier_ledger.py first), ownership bytes, last gate, census blockers, provenance.
G=$(dirname "$(readlink -f "$0")")/..; export LC_ALL=C
LAST=$(ls -td $G/output/gate/*/ 2>/dev/null | while read d; do [ -f $d/gate.log -o -f $d/provenance.txt ] && { echo $d; break; }; done)
python3 - "$G" "$LAST" <<'PY'
import csv, collections, os, sys
G, last = sys.argv[1], sys.argv[2]; O = G + "/output/"
def rows(f): return list(csv.DictReader(open(O + f))) if os.path.exists(O + f) else []
by = collections.defaultdict(int)
for r in rows("delink_sdk/ownership_items.csv"):
    if r["kind"] == "code": by[r["name"]] += int(r["hi"], 16) - int(r["lo"], 16)
L = rows("tier_ledger.csv"); cnt = collections.Counter(); size = collections.Counter(); stale = 0; val = 0
for r in L:
    k = (r["tier"], r["authority"]); cnt[k] += 1; size[k] += by.get(r["name"], 0); stale += int(r["stale"]); val += int(r["validated"])
print("tier x authority: functions / bytes")
for k in sorted(cnt): print("  %-7s %-12s %5d %8d" % (k + (cnt[k], size[k])))
inc = sum(1 for r in rows("diffemu_results.csv") if r["verdict"] == "INCONCLUSIVE")
print("stale %d  INCONCLUSIVE %d  tier2 validated %d" % (stale, inc, val))
res = "no gate run"
if last and os.path.exists(last + "/gate.log"):
    ls = [l for l in open(last + "/gate.log", errors="replace") if l.startswith("GATE ")]
    res = ls[-1].strip() if ls else "no result line"
print("last gate: %s: %s" % (os.path.basename(last.rstrip("/")) or "-", res))
bl = collections.Counter()
for r in rows("tier2_census.csv"): bl[r["blocker"] or "none"] += int(r["bytes"] or 0)
print("census top blockers (bytes): " + ", ".join("%s %d" % kv for kv in sorted(bl.items(), key=lambda kv: (-kv[1], kv[0]))[:5]))
pv = last + "/provenance.txt" if last else ""
print("provenance: " + (" | ".join(l.strip() for l in open(pv)) if os.path.exists(pv) else "none"))
print("next: " + ("re-validate %d stale row(s) (diffemu.py --all-tier2; shadow)" % stale if stale else "work the top census blocker" if bl else "run tier_ledger.py"))
PY
