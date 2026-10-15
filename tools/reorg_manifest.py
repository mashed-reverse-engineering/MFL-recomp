#!/usr/bin/env python3
"""reorg_manifest.py <moved.csv> <manifest in> <manifest out>: replace the source file of every manifest line by the module the function was moved to."""
import csv, sys
mv = {r["function"]: r["new"] for r in csv.DictReader(open(sys.argv[1]))}
n = 0
with open(sys.argv[3], "w") as out:
    for line in open(sys.argv[2]):
        p = line.split()
        if len(p) > 3 and p[0] not in ("#", "(none)") and p[1] in mv:
            line = line.replace(p[0], mv[p[1]], 1); n += 1
        out.write(line)
print("rewrote", n, "manifest lines")
