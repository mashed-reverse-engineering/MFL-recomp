#!/usr/bin/env python3
"""perf_probe.py [--budget PCT] <scenario dir>...: V4 frame-time probe, report-only.
Each dir is an output/scenario/perf_c0_sa_<tag> of `scenario.sh perf_c0` (SCEN_EXE mode, NOCACHE=1): out_ctrl/probe.txt and out_ours/probe.txt hold
"elapsed race_frames" every 0.25 s.  Per run and side: mean ms per race frame over the samples in which the race advanced (first and last interval
dropped).  Prints per-run values, the median of the run medians per side and ours/ctrl; 'over budget' when ours > ctrl * (1 + PCT/100)
(default 10).  Exit 0 always (report-only), 2 on missing data."""
import os, statistics, sys

def frame_ms(path):
    s = [tuple(float(x) for x in l.split()[:2]) for l in open(path) if len(l.split()) >= 2]
    mv = [b for a, b in zip(s, s[1:]) if b[1] > a[1] and a[1] > 0][1:-1]     # samples inside the running race (partial start / freeze intervals dropped)
    # mean over the race, not a median of 0.25 s intervals: ~16 frames per interval would quantise each one by ~6%
    return (mv[-1][0] - mv[0][0]) * 1000 / (mv[-1][1] - mv[0][1]) if len(mv) >= 4 else None

def main(a):
    bud = 10.0
    if a[:1] == ["--budget"]: bud = float(a[1]); a = a[2:]
    med = {"ctrl": [], "ours": []}
    for d in a:
        row = []
        for side in med:
            p = os.path.join(d, "out_" + side, "probe.txt"); m = frame_ms(p) if os.path.exists(p) else None
            row.append("%s %s" % (side, "%.3f ms" % m if m else "no data"))
            if m: med[side].append(m)
        print("%s: %s" % (os.path.basename(d.rstrip("/")), ", ".join(row)))
    if not med["ctrl"] or not med["ours"]: print("perf_probe: missing data"); return 2
    c, o = statistics.median(med["ctrl"]), statistics.median(med["ours"]); r = (o / c - 1) * 100
    print("perf_probe: median frame time ctrl %.3f ms, ours %.3f ms (%d / %d runs): %+.1f%% (budget +%.0f%%) -> %s (report-only)"
          % (c, o, len(med["ctrl"]), len(med["ours"]), r, bud, "over budget" if r > bud else "ok"))
    return 0

def selftest():
    import tempfile
    d = tempfile.mkdtemp()
    for side, step in (("ctrl", 10), ("ours", 12)):
        os.makedirs(os.path.join(d, "out_" + side))
        with open(os.path.join(d, "out_" + side, "probe.txt"), "w") as f:
            for i in range(20): f.write("%.2f %d\n" % (i * 0.25, 0 if i < 2 else (i - 2) * step))
    assert abs(frame_ms(os.path.join(d, "out_ctrl", "probe.txt")) - 25.0) < 1e-6
    assert abs(frame_ms(os.path.join(d, "out_ours", "probe.txt")) - 250 / 12) < 1e-6
    print("selftest ok")

if __name__ == "__main__":
    if sys.argv[1:] == ["--selftest"]: selftest(); sys.exit(0)
    if len(sys.argv) < 2: sys.exit(__doc__)
    sys.exit(main(sys.argv[1:]))
