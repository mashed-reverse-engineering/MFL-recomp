#!/usr/bin/env python3
"""fn_courses.py: which course executes which replaced function (from the --count coverage of courses.sh sweeps).
  fn_courses.py build <sweep dir>...   reads every <dir>/**/c<N>/ovl.exe.map + c<N>/out_ovl_1/counters.bin (courses.sh, courses_par.sh p<i>/c<N>, or a single
                                       regress dir named c<N>), unions several sweeps, writes output/fn_courses.csv (name,courses; courses joined by ';';
                                       functions nobody executed have an empty list). Refresh after every --count sweep (milestone).
  fn_courses.py <names...>             prints the course list covering the names (always includes course 0), and which names no course executes
                                       (those need a scenario script / are deferred to M2). Names may also be '@file' (one per line).
  fn_courses.py --selftest"""
import glob, os, re, struct, sys
CSV = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "output", "fn_courses.csv")

def build(dirs):
    allf, ran = set(), {}
    for d in dirs:
        for mp in glob.glob(os.path.join(d, "**", "ovl.exe.map"), recursive=True):
            w = os.path.dirname(mp); m = re.search(r"(?:^|/)c(\d+)$", w)
            cb = os.path.join(w, "out_ovl_1", "counters.bin")
            if not (m and os.path.exists(cb)): continue
            c, base, names = int(m.group(1)), None, []
            for l in open(mp):
                p = l.split()
                if p[0].startswith("count:"): names.append((p[0][6:], int(p[1], 16)))
                elif p[0] == "count_table": base = int(p[1], 16)
            data = open(cb, "rb").read()
            for n, va in names:
                allf.add(n)
                if base is not None and struct.unpack_from("<I", data, va - base)[0] > 0: ran.setdefault(n, set()).add(c)
    return allf, ran

def load():
    r = {}
    for l in open(CSV).read().split("\n")[1:]:
        if l: n, c = l.split(","); r[n] = {int(x) for x in c.split(";") if x}
    return r

if __name__ == "__main__":
    a = sys.argv[1:]
    if a[:1] == ["--selftest"]:
        import struct as s, tempfile
        t = tempfile.mkdtemp(); w = t + "/p0/c3"; os.makedirs(w + "/out_ovl_1")
        open(w + "/ovl.exe.map", "w").write("count_table 00a00000\ncount:f_a 00a00000\ncount:f_b 00a00004\n")
        open(w + "/out_ovl_1/counters.bin", "wb").write(s.pack("<II", 5, 0))
        allf, ran = build([t]); assert allf == {"f_a", "f_b"} and ran == {"f_a": {3}}, (allf, ran); print("ok"); sys.exit()
    if a[:1] == ["build"] and len(a) > 1:
        allf, ran = build(a[1:])
        if not allf: sys.exit("no c<N>/ovl.exe.map + counters.bin found (sweep without --count?)")
        os.makedirs(os.path.dirname(CSV), exist_ok=True)
        open(CSV, "w").write("name,courses\n" + "".join("%s,%s\n" % (n, ";".join(map(str, sorted(ran.get(n, ()))))) for n in sorted(allf)))
        print("%d functions, %d executed by some course -> output/fn_courses.csv" % (len(allf), len(ran))); sys.exit()
    names = []
    for x in a: names += [l.strip() for l in open(x[1:]) if l.strip()] if x.startswith("@") else [x]
    if not names or not os.path.exists(CSV): sys.exit(__doc__ if not names else "output/fn_courses.csv missing: run 'fn_courses.py build <sweep dir>' after a --count sweep")
    fc = load(); cs, none = {0}, []
    for n in names:
        if n not in fc: print("# %s: not in the table (not a replaced function, or newer than the last --count sweep): course 0 only" % n, file=sys.stderr)
        elif not fc[n]: none.append(n)
        elif not fc[n] & cs: cs.add(min(fc[n]))      # greedy: one course per function is enough for a crash smoke, courses already chosen (0 always) cover it
    for n in none: print("# no course executes %s (scenario script / defer to M2)" % n, file=sys.stderr)
    print(" ".join(map(str, sorted(cs))))
