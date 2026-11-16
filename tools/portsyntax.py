#!/usr/bin/env python3
"""portsyntax.py [--out <csv>] [--jobs N]   REPORT ONLY (always exit 0): second-compiler syntax run over the standalone link's C source set.
i686-w64-mingw32-gcc -c -fms-extensions -DNOASM on every file (tier1, tier2 = automatch_near, an_codfh_*, enabled clusters, hand written src/*.c), no link,
sources compiled in place (nothing is written into the tree).  A file that fails is compiled once more with its MSVC `__asm` blocks removed: ok then = "asm only".
Prints per group: files, ok as is, asm-only failures, other failures; the baseline (output/portsyntax_baseline.txt: "ok <n> total <m>") and the difference.
CSV (default <scratch>/portsyntax.csv, never a tracked file): file, group, ok, asm_only, first error.
W11 PP (port-prep item 1): the W10 spike measured 3,773 of 4,468 before the header and per-file fixes."""
import csv, glob, os, re, subprocess, sys, tempfile, time
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
R = ROOT + "/src"
CC = "i686-w64-mingw32-gcc"
ASMB = re.compile(r"\b_{1,2}asm\s*\{[^}]*\}|\b_{1,2}asm\b[^\n{]*")
FLAGS = ["-c", "-w", "-std=gnu99", "-fms-extensions", "-D__int64=long long", "-DNOASM", "-msse2", "-fmax-errors=1"]      # -msse2: the 5 an_rwp SSE intrinsic files (the port maps them to sse2neon)


def lines(p):
    return [l.split()[0] for l in open(p) if l.strip() and not l.startswith("#")] if os.path.exists(p) else []


def groups():
    skip = set(lines(R + "/skip_build.txt")) | {"an_hk_975.c"}      # the link skips these (link_ours.sh SKIP_FILES, skip_build.txt)
    t2 = [f for f in sorted(glob.glob(R + "/automatch_near/*.c")) if os.path.basename(f) not in skip]
    return {"tier1": sorted(glob.glob(R + "/tier1/*.c")),
            "codfh": [f for f in t2 if os.path.basename(f).startswith("an_codfh_")],
            "tier2": [f for f in t2 if not os.path.basename(f).startswith("an_codfh_")],
            "clusters": [R + "/clusters/cl_%s.c" % n for n in lines(R + "/clusters/enabled.txt")],
            "hand": sorted(glob.glob(R + "/*.c"))}


def cc(src, d, extra=()):
    inc = ["-I", d, "-I", R + "/auto", "-I", R, "-I", R + "/tier1"]
    r = subprocess.run([CC] + FLAGS + inc + list(extra) + [src, "-o", "/dev/null"], capture_output=True, text=True, errors="replace")
    err = [l for l in r.stderr.split("\n") if " error: " in l or "fatal error" in l]
    return r.returncode == 0, (err[0].split("error: ", 1)[-1][:100] if err else "")


def one(a):
    g, f = a
    d = os.path.dirname(f)
    if not os.path.exists(f):
        return g, f, False, False, "missing file"
    ok, e = cc(f, d)
    if ok:
        return g, f, True, False, ""
    t = open(f, errors="surrogateescape").read()
    if not ASMB.search(t):
        return g, f, False, False, e
    with tempfile.TemporaryDirectory(prefix="psx_", dir=os.environ.get("TMPDIR", "/tmp")) as td:
        c = td + "/" + os.path.basename(f)
        open(c, "w", errors="surrogateescape").write(ASMB.sub(" ", t))
        ok2, e2 = cc(c, d)
    return g, f, False, ok2, "" if ok2 else (e2 or e)


def main(argv):
    out = jobs = None
    a = list(argv)
    while a:
        k = a.pop(0)
        if k == "--out":
            out = a.pop(0)
        elif k == "--jobs":
            jobs = int(a.pop(0))
        else:
            print(__doc__)
            return 0
    if subprocess.run(["which", CC], capture_output=True).returncode:
        print("portsyntax: %s not installed: skipped (report-only)" % CC)
        return 0
    t0 = time.time()
    G = groups()
    work = [(g, f) for g, fl in G.items() for f in fl]
    with ThreadPoolExecutor(jobs or min(8, os.cpu_count() or 4)) as ex:
        res = list(ex.map(one, work))
    out = out or "/tmp/claude-%d/portsyntax.csv" % os.getuid()
    with open(out, "w", newline="") as fo:
        w = csv.writer(fo)
        w.writerow(["file", "group", "ok", "asm_only", "first_error"])
        for g, f, ok, ao, e in res:
            w.writerow([os.path.relpath(f, R), g, int(ok), int(ao), e])
    tot = ok_n = 0
    for g in G:
        rs = [x for x in res if x[0] == g]
        ok = sum(x[2] for x in rs)
        ao = sum(x[3] for x in rs)
        tot += len(rs)
        ok_n += ok
        print("portsyntax %-8s files %4d  ok %4d (%.1f%%)  asm-only fail %3d  other fail %3d" % (g, len(rs), ok, 100.0 * ok / max(len(rs), 1), ao, len(rs) - ok - ao))
    bl = ROOT + "/output/portsyntax_baseline.txt"
    base = ""
    if os.path.exists(bl):
        m = re.search(r"^ok (\d+) total (\d+)", open(bl).read(), re.M)      # W12: the data line only (the comment's W10 figure was read before)
        if m:
            base = "  baseline ok %s of %s (%+d)" % (m.group(1), m.group(2), ok_n - int(m.group(1)))
    nonasm = sum(1 for x in res if not x[2] and not x[3])
    print("portsyntax TOTAL ok %d total %d, non-asm failures %d%s  [%.0f s, csv %s]  (report only)" % (ok_n, tot, nonasm, base, time.time() - t0, out))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
