#!/usr/bin/env python3
"""scen_union.py: output/fn_scenarios.csv (name,scenarios; ';'-joined) = which scenario (output/scenario/<name>/executed.txt from scenario.sh: --count counters UNION gdb entry
breakpoints) executed which replaced function.  Course sweeps in output/scenario/<dir>/**/c<N> (courses.sh, counters only) are added as <dir>:c<N>.
Prints the totals and, for the names listed in NEVER (default output/never_exec.txt), how many are executed now.  Does not touch output/fn_courses.csv (quick battle courses only)."""
import glob, os, re, struct, sys
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
