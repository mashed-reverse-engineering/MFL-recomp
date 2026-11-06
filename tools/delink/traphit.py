#!/usr/bin/env python3
"""traphit.py: name the int3-filled row behind a Wine "Unhandled exception ... at address X" (trap-hit attribution, plan 2026-11-05 v2 L1 1-0).
Usage: traphit.py [--dead <link>/mfl.obj.dead.csv] [--list F]... <wine.log | gate/sweep/scenario dir>...
  Dirs are searched recursively for *.log. Rows come from the filled lists in tools/delink (rw_trap, rw_dead, dshow_dead, dead_tables, dinput_dead;
  `--list` replaces them) plus, with --dead, the exact ranges a link filled. A hit at X counts for a row covering X or X-1 (int3 reports either).
  Each hit prints `TRAPHIT <log>: <code> at <va> -> <list>:<line> <va> <size> <name> [<section tag>] +off`; unattributed crashes print `CRASH ... -> not a filled row`.
  Last line `traphit: N crash(es), M on filled rows`; exit 1 if M > 0.
  traphit.py --selftest: tools/negtests/traphit (real Wine int3 line from output/delink/ls6, planted row) must name the planted row and nothing else."""
import csv, os, re, sys
D = os.path.dirname(os.path.abspath(__file__))
LISTS = ["rw_trap.txt", "rw_dead.txt", "dshow_dead.txt", "dead_tables.txt", "dinput_dead.txt"]
RX = re.compile(r"Unhandled (exception (0x[0-9A-Fa-f]+)|page fault)[^\n]*? at address ([0-9A-Fa-f]+)")


def rows_from_list(path):
    tag, out = "", []
    for n, l in enumerate(open(path), 1):
        if l.startswith("#"):
            tag = l[1:].split(":")[0].strip()[:40]; continue
        q = l.split(None, 2)
        if len(q) >= 2: out.append((int(q[0], 16), int(q[1]), "%s:%d" % (os.path.basename(path), n), q[2].strip() if len(q) > 2 else "", tag))
    return out


def rows_from_dead(path):
    return [(int(r["va"], 16), int(r["size"]), "%s:%d" % (os.path.basename(path), n + 2), r["why"], "link") for n, r in enumerate(csv.DictReader(open(path)))]


def logs(paths):
    for p in paths:
        if os.path.isfile(p): yield p
        for root, _, fs in os.walk(p):
            for f in sorted(fs):
                if f.endswith(".log"): yield os.path.join(root, f)


def find(rows, va):
    for a in (va, va - 1):      # list rows first, so a link dead.csv row only names what no list does
        for r in rows:
            if r[0] <= a < r[0] + r[1]: return r
    return None


def run(paths, lists, dead=None, out=sys.stdout):
    rows = [r for f in lists for r in rows_from_list(f)] + (rows_from_dead(dead) if dead else [])
    n = hits = 0
    for p in logs(paths):
        for m in RX.finditer(open(p, errors="replace").read()):
            n += 1; va = int(m.group(3), 16); code = m.group(2) or "page-fault"
            r = find(rows, va)
            if r:
                hits += 1
                print("TRAPHIT %s: %s at %08x -> %s %08x %d %s [%s] +%#x" % (p, code, va, r[2], r[0], r[1], r[3], r[4], va - r[0]), file=out)
            else:
                print("CRASH %s: %s at %08x -> not a filled row" % (p, code, va), file=out)
    print("traphit: %d crash(es), %d on filled rows" % (n, hits), file=out)
    return hits


def selftest():
    import io
    N = os.path.join(D, "..", "negtests", "traphit")
    b = io.StringIO(); h = run([os.path.join(N, "wine.log")], [os.path.join(N, "planted.txt")], out=b); t = b.getvalue()
    ok = h == 1 and "planted.txt:2 0065ade0 16 PLANTED_qh_qhull" in t
    b2 = io.StringIO(); h2 = run([os.path.join(N, "wine.log")], [os.path.join(N, "unplanted.txt")], out=b2)
    ok = ok and h2 == 0 and "not a filled row" in b2.getvalue()
    print(t + b2.getvalue() + ("selftest PASS" if ok else "selftest FAIL"))
    return 0 if ok else 1


if __name__ == "__main__":
    a = sys.argv[1:]
    if a == ["--selftest"]: sys.exit(selftest())
    lists, dead, paths = [], None, []
    while a:
        x = a.pop(0)
        if x == "--list": lists.append(a.pop(0))
        elif x == "--dead": dead = a.pop(0)
        else: paths.append(x)
    if not paths: sys.exit(__doc__)
    sys.exit(1 if run(paths, lists or [os.path.join(D, f) for f in LISTS], dead) else 0)
