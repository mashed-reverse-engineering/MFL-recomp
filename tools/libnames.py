#!/usr/bin/env python3
"""Turn a libmatch.py result into RenameFns.java csv files: anchors (unique byte matches) and gap names (functions between two matched
neighbours of the same source file, assigned in link order when the counts agree).
Usage: libnames.py <workdir of libmatch (with _o/*.obj)> <libmatch csv> <fnindex.txt> <out prefix> <label>   writes <prefix>.csv and <prefix>_gaps.csv
fnindex.txt = tab separated 'addr name size ...' list of the functions in the exe (FnIndex.java output)."""
import glob, os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
src = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "libmatch.py")).read().replace("\nmain()\n", "\n")
ns = {"__file__": os.path.join(os.path.dirname(os.path.abspath(__file__)), "libmatch.py")}
exec(compile(src, "libmatch", "exec"), ns)

def main():
    work, csv, idx, prefix, label = sys.argv[1:6]
    fns = sorted((int(f[0], 16), f[1]) for f in (l.rstrip("\n").split("\t") for l in open(idx)) if len(f) >= 3)
    rows = {}
    for l in open(csv):
        r = l.strip().split(",")
        if len(r) >= 3 and not (len(r) > 3 and r[3]):
            rows[(r[2], r[1])] = int(r[0], 16)
    anchors_out, gaps_out = [], []
    for c in sorted(glob.glob(os.path.join(work, "**/*.c"), recursive=True)):
        rel = os.path.relpath(c, work)
        obj = os.path.join(work, "_o", os.path.basename(c)[:-2] + ".obj")
        if not os.path.exists(obj):
            continue
        names = [n for n, code, m in ns["obj_functions"](obj)]
        anchors = [(i, rows[(rel, n)]) for i, n in enumerate(names) if (rel, n) in rows]
        for i, a in anchors:
            anchors_out.append("%08x,%s,%s %s (byte identical when compiled with the game's compiler)" % (a, names[i], label, rel))
        for (i, a), (j, b) in zip(anchors, anchors[1:]):
            if j - i >= 2:
                between = [f for f in fns if a < f[0] < b]
                if len(between) == j - i - 1:
                    for k, f in zip(range(i + 1, j), between):
                        gaps_out.append("%08x,%s,%s %s (by link order between matched neighbours)" % (f[0], names[k], label, rel))
    open(prefix + ".csv", "w").write("\n".join(anchors_out) + "\n")
    open(prefix + "_gaps.csv", "w").write("\n".join(gaps_out) + "\n")
    print(len(anchors_out), "anchors,", len(gaps_out), "gap names")

main()
