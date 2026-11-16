#!/usr/bin/env python3
"""scen_judge.py [--shots all|final|none] [--thr N] <run dir A> <run dir B>: judged verdict of one scenario of the scenario set (lane TS, W11).
A and B are two scenario.sh SCEN_EXE runs of the same scenario with fresh controls (NOCACHE=1): out_ctrl/ out_ours/ result.txt run_ours/.
PASS needs all of:
  - no crash, ours rc 0, in both runs (result.txt);  no mfl_trap.log in either run_ours/App_Executables (rule 23: a reached trap row is a failure)
  - every dump *.bin of the controls except counters.bin: control A == control B (wine-run/cmpdump.py, kind cars / gs / raw), else UNJUDGED;
    ours A and ours B == control A (cmpdump with CMP_DELTA = the link's DELTA from the run's script, data pointers moved by it are equal)
  - screenshots (--shots all: every png both controls have; final: final.png only; none): control A vs B below --thr (default 30, the
    scenario.sh limit), else UNJUDGED for that scenario; ours A and B vs control A below --thr
Exit 0 PASS, 1 FAIL, 3 UNJUDGED (the gate counts 3 as red).  --selftest: the rule on built-in dirs."""
import glob, os, subprocess, sys
WR = os.path.dirname(os.path.abspath(__file__))
while not os.path.exists(os.path.join(WR, "imgdiff.py")) and WR != "/":      # a staged copy (agents/<lane>/stage/...) uses the tree's wine-run tools
    WR = os.path.dirname(WR) if not os.path.exists(os.path.join(WR, "tools", "wine-run", "imgdiff.py")) else os.path.join(WR, "tools", "wine-run")
sys.path.insert(0, WR)
from imgdiff import diff

def run_state(run, k):
    t = open(os.path.join(run, "result.txt"), errors="replace").read() if os.path.exists(os.path.join(run, "result.txt")) else ""
    line = next((l for l in t.splitlines() if l.startswith(k + ":")), "")
    return "clean" in line and line.rstrip().endswith("rc 0"), line.strip() or "%s: no result line" % k

def cmpd(kind, a, b, delta=None, noise=()):
    env = dict(os.environ, CMP_DELTA=delta or "")
    r = subprocess.run([sys.executable, os.path.join(WR, "cmpdump.py"), kind, a, b, *noise], capture_output=True, text=True, env=env).stdout.strip()
    try: return int(r)
    except ValueError: return -1

def delta_of(run):
    p = os.path.join(run, "script_ours.txt")      # scenario.sh does not store SCEN_DELTA; addrmove moved the title-screen waitmem address by it
    for l in open(p, errors="replace") if os.path.exists(p) else []:
        if l.startswith("waitmem 0x") and l.split()[2:3] == ["0"]:
            return "%x" % (int(l.split()[1], 16) - 0x69d1e4)
    return None

def judge(A, B, shots="final", thr=30.0):
    bad, unj = [], []
    for run in (A, B):
        for k in ("ctrl", "ours"):
            ok, line = run_state(run, k)
            if not ok: (unj if k == "ctrl" else bad).append("%s %s" % (os.path.basename(run), line))
        if glob.glob(os.path.join(run, "run_ours", "App_Executables", "mfl_trap.log")): bad.append("%s mfl_trap.log" % os.path.basename(run))
    for run in (A, B):      # W12 hx3: a REQUIRED end event (scenset.sh `pressuntil ... end`) that timed out: ours -> FAIL, control -> UNJUDGED
        for k in ("ctrl", "ours"):
            m = os.path.join(run, "out_" + k, "missed.txt")
            for l in (open(m, errors="replace").read().splitlines() if os.path.exists(m) else []):
                if l.startswith("MISS end"): (unj if k == "ctrl" else bad).append("%s %s missed its end event: %s" % (os.path.basename(run), k, l[9:]))
    ca, cb = os.path.join(A, "out_ctrl"), os.path.join(B, "out_ctrl")
    for run in (A, B):      # scenset.sh dumps gs_end at its end event: a side without it never got there (a pass on no evidence otherwise)
        for k in ("ctrl", "ours"):
            if not os.path.exists(os.path.join(run, "out_" + k, "gs_end.bin")):
                (unj if k == "ctrl" else bad).append("%s %s has no gs_end.bin" % (os.path.basename(run), k))
    d = delta_of(A)
    for f in sorted(os.path.basename(x) for x in glob.glob(os.path.join(ca, "*.bin"))):
        if f == "counters.bin": continue
        kind = "cars" if f.startswith("cars") else "gs" if f.startswith("gs_") else "raw"
        n = cmpd(kind, os.path.join(ca, f), os.path.join(cb, f))
        if n != 0: unj.append("controls differ in %s (%d bytes)" % (f, n)); continue
        for run in (A, B):
            m = cmpd(kind, os.path.join(ca, f), os.path.join(run, "out_ours", f), d)
            if m != 0: bad.append("%s ours %s differs from the control (%d bytes)" % (os.path.basename(run), f, m))
    names = [] if shots == "none" else ["final.png"] if shots == "final" else sorted(os.path.basename(x) for x in glob.glob(os.path.join(ca, "*.png")))
    for f in names:
        if not (os.path.exists(os.path.join(ca, f)) and os.path.exists(os.path.join(cb, f))): continue
        c = diff(os.path.join(ca, f), os.path.join(cb, f))
        if c >= thr: unj.append("controls differ in %s (%.1f)" % (f, c)); continue
        for run in (A, B):
            o = os.path.join(run, "out_ours", f)
            if not os.path.exists(o): bad.append("%s ours has no %s" % (os.path.basename(run), f)); continue
            x = diff(os.path.join(ca, f), o)
            if x >= thr: bad.append("%s ours %s differs (%.1f, controls %.1f)" % (os.path.basename(run), f, x, c))
    v = "FAIL" if bad else "UNJUDGED" if unj else "PASS"
    return v, bad + unj

def main(a):
    shots, thr = "final", 30.0
    while a and a[0].startswith("--"):
        if a[0] == "--shots": shots = a[1]
        elif a[0] == "--thr": thr = float(a[1])
        a = a[2:]
    v, why = judge(a[0], a[1], shots, thr)
    for w in why: print("  " + w)
    print("SCENSET %s: %s" % (os.path.basename(a[0].rstrip("/")), v))
    return {"PASS": 0, "FAIL": 1, "UNJUDGED": 3}[v]

def selftest():
    import shutil, struct, tempfile, zlib
    def png(p, v):
        raw = b"".join(b"\0" + bytes([v]) * 24 for _ in range(8))
        ch = lambda t, d: struct.pack(">I", len(d)) + t + d + struct.pack(">I", zlib.crc32(t + d) & 0xffffffff)
        open(p, "wb").write(b"\x89PNG\r\n\x1a\n" + ch(b"IHDR", struct.pack(">IIBBBBB", 8, 8, 8, 2, 0, 0, 0)) + ch(b"IDAT", zlib.compress(raw)) + ch(b"IEND", b""))
    def mk(root, name, ours_gs=b"\0" * 16, ctrl_gs=b"\0" * 16, ours_png=10, ours_line="ours: clean  rc 0", trap=False):
        r = os.path.join(root, name)
        for k in ("ctrl", "ours"): os.makedirs(os.path.join(r, "out_" + k))
        open(os.path.join(r, "result.txt"), "w").write("ctrl: clean  rc 0\n%s\n" % ours_line)
        open(os.path.join(r, "out_ctrl", "gs_end.bin"), "wb").write(ctrl_gs); open(os.path.join(r, "out_ours", "gs_end.bin"), "wb").write(ours_gs)
        png(os.path.join(r, "out_ctrl", "final.png"), 10); png(os.path.join(r, "out_ours", "final.png"), ours_png)
        if trap: os.makedirs(os.path.join(r, "run_ours", "App_Executables")); open(os.path.join(r, "run_ours", "App_Executables", "mfl_trap.log"), "w").write("x")
        return r
    t = tempfile.mkdtemp()
    a, b = mk(t, "a"), mk(t, "b")
    assert judge(a, b)[0] == "PASS", judge(a, b)
    assert judge(a, mk(t, "c", ours_gs=b"\1" + b"\0" * 15))[0] == "FAIL"
    assert judge(a, mk(t, "d", ctrl_gs=b"\2" + b"\0" * 15))[0] == "UNJUDGED"
    assert judge(a, mk(t, "e", ours_png=200))[0] == "FAIL"
    assert judge(a, mk(t, "f", ours_line="ours: CRASH Unhandled exception  rc 3"))[0] == "FAIL"
    assert judge(a, mk(t, "g", trap=True))[0] == "FAIL"
    h = mk(t, "h"); os.remove(os.path.join(h, "out_ctrl", "gs_end.bin"))
    assert judge(a, h)[0] == "UNJUDGED"                                  # control without the end dump
    i = mk(t, "i"); os.remove(os.path.join(i, "out_ours", "gs_end.bin"))
    assert judge(a, i)[0] == "FAIL"                                      # ours without the end dump
    j, k = mk(t, "j"), mk(t, "k")
    for r in (j, k):
        for s in ("ctrl", "ours"): os.remove(os.path.join(r, "out_" + s, "gs_end.bin"))
    assert judge(j, k)[0] != "PASS"                                      # no dumps anywhere: never a PASS
    l = mk(t, "l"); open(os.path.join(l, "out_ours", "missed.txt"), "w").write("MISS end pressuntil Return 0x69c9c8==6 after 150s (load-scaled)\n")
    assert judge(a, l)[0] == "FAIL"                                      # W12 hx3: ours missed the required end event (gs_end still dumped)
    m = mk(t, "m"); open(os.path.join(m, "out_ctrl", "missed.txt"), "w").write("MISS end pressuntil Return 0x652f68==10 after 150s (load-scaled)\n")
    assert judge(a, m)[0] == "UNJUDGED"                                  # the control missed it: no verdict
    n = mk(t, "n"); open(os.path.join(n, "out_ours", "missed.txt"), "w").write("MISS other pressuntil x\n")
    assert judge(a, n)[0] == "PASS"                                      # only the `end` tag is required
    shutil.rmtree(t); print("selftest ok"); return 0

if __name__ == "__main__":
    a = sys.argv[1:]
    sys.exit(selftest() if a == ["--selftest"] else main(a) if len(a) >= 2 else (print(__doc__) or 2))
