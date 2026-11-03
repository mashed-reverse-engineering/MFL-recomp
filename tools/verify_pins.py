#!/usr/bin/env python3
"""verify_pins.py [--pin] [--selftest]   tamper check for the verification scripts and the checked function set.

output/verify_pins.json holds (1) the sha256 of every file in PINNED (the scripts whose verdicts decide installs) and
(2) the checked set: every function with real evidence, from output/tier_ledger.csv (authority bytes-reloc / diffemu /
shadow) and output/behaviour_evidence.csv (verified=1, covers the clusters).
Default (check): exit 1 when a pinned file changed / vanished, or a function LEFT the checked set (gone, or authority
dropped to static / none) since the pin; downgrades (bytes-reloc -> diffemu) and new functions are printed, not fatal.
--pin writes the json.  ONLY the main session pins, after reviewing the diff of the changed scripts (agents never pin:
a worker that edits a checker and re-pins defeats the point; the json is tracked in git, so a re-pin shows in review).
Run tools/tier_ledger.py + tools/evidence_ledger.py first (check_all.sh does) so the ledgers are current."""
import csv, datetime, hashlib, json, os, sys
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
PIN = os.path.join(G, "output", "verify_pins.json")
PINNED = ["tools/cmpfn.py", "tools/asmdiff.py",                     # manifest byte compare (asmdiff = its diff classes)
          "tools/overlay.py", "tools/build_overlay.py",             # RELCHECK (overlay.py) + the strict builds / flags that feed it
          "tools/check_all.sh", "tools/gate.sh", "tools/tier_ledger.py",
          "tools/defectlint.py", "tools/diffemu/core.py", "tools/delink/sa_oracles.sh",
          "tools/d3dtrace/drawcmp.py", "tools/wine-run/scenario.sh"]
STRONG = ("bytes-reloc", "diffemu", "shadow")
RANK = {"bytes-reloc": 3, "diffemu": 2, "shadow": 1, "verified": 1}

def sha(p):
    try: return hashlib.sha256(open(os.path.join(G, p), "rb").read()).hexdigest()
    except OSError: return "MISSING"

def checked_set():
    out = {}
    for r in csv.DictReader(open(os.path.join(G, "output", "tier_ledger.csv"))):
        if r["authority"] in STRONG: out[r["name"]] = "%s:%s" % (r["tier"], r["authority"])
    p = os.path.join(G, "output", "behaviour_evidence.csv")
    for r in (csv.DictReader(open(p)) if os.path.exists(p) else []):
        if r["verified"] == "1": out.setdefault(r["name"], "%s:verified" % r["tier"])
    return out

def diff(old, new):
    """-> (left, downgraded, added): names that left the checked set, names whose authority got weaker, new names."""
    left = sorted(n for n in old if n not in new)
    down = sorted(n for n in old if n in new and RANK[new[n].split(":")[1]] < RANK[old[n].split(":")[1]])
    return left, down, sorted(n for n in new if n not in old)

def selftest():
    old = {"a": "tier1:bytes-reloc", "b": "tier2:diffemu", "c": "cluster:verified"}
    new = {"a": "tier2:diffemu", "c": "cluster:verified", "d": "tier1:bytes-reloc"}
    assert diff(old, new) == (["b"], ["a"], ["d"]), diff(old, new)
    print("selftest ok")

def main():
    if "--selftest" in sys.argv: return selftest()
    files = {p: sha(p) for p in PINNED}; cs = checked_set()
    if "--pin" in sys.argv:
        json.dump({"pinned_at": datetime.datetime.now().isoformat(timespec="seconds"), "files": files, "checked": cs},
                  open(PIN, "w"), indent=0, sort_keys=True)
        print("pinned %d files, %d checked functions -> %s" % (len(files), len(cs), PIN)); return 0
    if not os.path.exists(PIN): print("no pin yet (%s): the main session runs --pin" % PIN); return 1
    pin = json.load(open(PIN)); bad = 0
    for p in sorted(set(pin["files"]) | set(files)):
        if pin["files"].get(p) != files.get(p):
            bad += 1; print("CHANGED %s (pinned %s, now %s)" % (p, (pin["files"].get(p) or "-")[:12], (files.get(p) or "not pinned")[:12]))
    left, down, added = diff(pin["checked"], cs)
    for n in left: print("LEFT %s (was %s, now %s)" % (n, pin["checked"][n], "gone or unchecked"))
    for n in down: print("WEAKER %s (%s -> %s)" % (n, pin["checked"][n], cs[n]))
    print("verify_pins: %d pinned file(s) changed, %d function(s) left the checked set, %d weaker, %d new (pin of %s)"
          % (bad, len(left), len(down), len(added), pin["pinned_at"]))
    return 1 if bad or left else 0

if __name__ == "__main__":
    sys.exit(main())
