#!/usr/bin/env python3
"""ladder_overridden.py clsweep.log [...] > output/ladder_overridden_diffs.csv: roots whose first DIFF was overridden by a later retry rung (clsweep prints `| earlier_diff: <line>`)."""
import csv, re, sys
w = csv.writer(sys.stdout); w.writerow(["root", "final_via", "earlier_diff"])
for f in sys.argv[1:]:
    for l in open(f):
        m = re.match(r"(\S+) EQUAL(?: \[via (\w+)\])? \|.*\| earlier_diff: (.+)$", l.rstrip())
        if m and m.group(3) != "-": w.writerow([m.group(1), m.group(2) or "", m.group(3)])
