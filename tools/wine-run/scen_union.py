#!/usr/bin/env python3
"""scen_union.py: output/fn_scenarios.csv (name,scenarios; ';'-joined) = which scenario (output/scenario/<name>/executed.txt from scenario.sh: --count counters UNION gdb entry
breakpoints) executed which replaced function.  Course sweeps in output/scenario/<dir>/**/c<N> (courses.sh, counters only) are added as <dir>:c<N>.
Prints the totals and, for the names listed in NEVER (default output/never_exec.txt), how many are executed now.  Does not touch output/fn_courses.csv (quick battle courses only)."""
import glob, os, re, struct, sys
# standalone mode: scen_union.py standalone [fns csv = output/standalone_fns.csv from standalone_fns.py]
# -> output/fn_scenarios_standalone.csv (name,va,size,layer,scenarios): scenarios = output/scenario/<name>_cov_<tag>/out/gdbcov.txt (scen_cov.sh, "HIT name" per function of the standalone ours.exe), label <name>_<tag>.
# Prints per scenario and for the union the table layer, functions, bytes, executed (functions / bytes).
if sys.argv[1:2] == ["standalone"]:
    import collections, csv
    O = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "output")
    rows = list(csv.DictReader(open(sys.argv[2] if len(sys.argv) > 2 else O + "/standalone_fns.csv"))); byn = {r["name"]: r for r in rows}; ran = {}
    for g in sorted(glob.glob(O + "/scenario/*_cov_*/out/gdbcov.txt")):
        lab = os.path.basename(os.path.dirname(os.path.dirname(g))).replace("_cov_", "_")
        for l in open(g, errors="replace"):
            if l.startswith("HIT ") and l.split()[1] in byn: ran.setdefault(l.split()[1], set()).add(lab)
    with open(O + "/fn_scenarios_standalone.csv", "w") as f:
        w = csv.writer(f); w.writerow(["name", "va", "size", "layer", "scenarios"])
        for r in rows: w.writerow([r["name"], r["va"], r["size"], r["layer"], ";".join(sorted(ran.get(r["name"], ())))])
    labs = sorted({x for v in ran.values() for x in v})
    for lab in labs + ["UNION"]:
        T = collections.defaultdict(lambda: [0, 0, 0, 0])
        for r in rows:
            t = T[r["layer"]]; t[0] += 1; t[1] += int(r["size"])
            if r["name"] in ran and (lab == "UNION" or lab in ran[r["name"]]): t[2] += 1; t[3] += int(r["size"])
        print("== %s\n%-14s %9s %9s %9s %9s %6s" % (lab, "layer", "functions", "bytes", "executed", "exec bytes", "%fn"))
        tot = [sum(t[i] for t in T.values()) for i in range(4)]
        for k, t in sorted(T.items(), key=lambda x: -x[1][1]) + [("TOTAL", tot)]: print("%-14s %9d %9d %9d %9d %5.1f%%" % (k, t[0], t[1], t[2], t[3], 100.0 * t[2] / t[0]))
    sys.exit(0)
R = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "output"); S = R + "/scenario"
allf, ran = set(), {}
for ef in sorted(glob.glob(S + "/*/executed.txt")):
    sc = os.path.basename(os.path.dirname(ef))
    for l in open(ef):
        if l.strip(): ran.setdefault(l.strip(), set()).add(sc)
    for l in open(os.path.dirname(ef) + "/ovl.exe.map"):
        if l.startswith("count:"): allf.add(l.split()[0][6:])
for mp in glob.glob(S + "/*/**/ovl.exe.map", recursive=True):
    w = os.path.dirname(mp); m = re.search(r"/([^/]+)/(?:.*/)?c(\d+)$", w); cb = w + "/out_ovl_1/counters.bin"
    if not (m and os.path.exists(cb)): continue
    base, names = None, []
    for l in open(mp):
        p = l.split()
        if p[0].startswith("count:"): names.append((p[0][6:], int(p[1], 16)))
        elif p[0] == "count_table": base = int(p[1], 16)
    d = open(cb, "rb").read()
    for n, va in names:
        allf.add(n)
        if struct.unpack_from("<I", d, va - base)[0] > 0: ran.setdefault(n, set()).add("%s:c%s" % (m.group(1), m.group(2)))
open(R + "/fn_scenarios.csv", "w").write("name,scenarios\n" + "".join("%s,%s\n" % (n, ";".join(sorted(ran.get(n, ())))) for n in sorted(allf)))
nv = os.environ.get("NEVER", "output/never_exec.txt"); never = [l.strip() for l in open(nv) if l.strip()] if os.path.exists(nv) else []
print("%d replaced functions, %d executed by some scenario -> output/fn_scenarios.csv; of the %d in %s: %d executed" % (len(allf), len(ran), len(never), nv, sum(1 for n in never if n in ran)))
