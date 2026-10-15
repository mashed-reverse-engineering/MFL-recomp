#!/usr/bin/env python3
"""Conformance tests (original against our C versions, same random arguments, inside the game process).  Usage: conftest.py <work dir> <base dir with snap_ctrl (a warmed run, e.g. a regress dir)> [--max-rounds N]
Builds a control exe (--no-patch: original functions) and an overlay exe (patched), both with the conformance runtime (src/harness_conf, table from tools/conftest_gen.py), runs both
at the title screen (conf_poll calls every table function with NSETS argument sets, see conf.c) and compares the result arrays.  A function that makes a process hang or crash is
added to the skip list (in both exes) and the round is repeated.  Writes <work dir>/report.txt (and prints the summary).  One Wine job class at a time: uses slots 1 and 2."""
import json, os, re, struct, subprocess, sys, threading, time, glob
HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.join(HERE, "..")
W = os.path.abspath(sys.argv[1]); BASE = os.path.abspath(sys.argv[2])
ROUNDS = int(sys.argv[sys.argv.index("--max-rounds") + 1]) if "--max-rounds" in sys.argv else 12
os.makedirs(W, exist_ok=True)
NSETS, REC = 8, 32

def sh(cmd, **kw): return subprocess.run(cmd, capture_output=True, text=True, **kw)
def mapva(mp, name):
    for l in open(mp):
        p = l.split()
        if p and p[0] == name: return p[1]
PASS = sys.argv[sys.argv.index("--pass") + 1] if "--pass" in sys.argv else "all"
RACE = int(sys.argv[sys.argv.index("--race") + 1]) if "--race" in sys.argv else 0          # run the tests in the race at this race frame (real game state) instead of at the title screen
NAMES = sys.argv[sys.argv.index("--names") + 1] if "--names" in sys.argv else None        # only these functions (one name per line)
def build(skip):
    sh(["python3", os.path.join(TOOLS, "conftest_gen.py"), "--pass", PASS] + (["--skip", ",".join(sorted(skip))] if skip else []))
    for k, extra in (("ctrl", ["--no-patch"]), ("ovl", [])):
        r = sh(["python3", os.path.join(TOOLS, "build_overlay.py"), os.path.join(W, k + ".exe"), "--harness", "--conftest"] + os.environ.get("CONF_FLAGS", "").split() + extra)
        if r.returncode: sys.exit("build failed: " + r.stderr[-400:])
def run(k, slot, start):
    mp = os.path.join(W, k + ".exe.map")
    ce, ci, cd, cn, cs, ch, cr = (mapva(mp, n) for n in ("conf_enable", "conf_index", "conf_done", "conf_nres", "conf_start", "conf_hook", "conf_results"))
    poll = mapva(mp, "conf_poll")
    run_dir, out = os.path.join(W, "run_%s_%d" % (k, start)), os.path.join(W, "out_%s_%d" % (k, start))   # fresh directories per round: a stale dump from an earlier round must never be read
    snap = os.path.join(BASE, "snap_ctrl") if os.path.isdir(os.path.join(BASE, "snap_ctrl")) else os.path.join(BASE, "snap_ovl")      # warmed game copy (the exe is replaced below)
    sh([os.path.join(HERE, "clone.sh"), snap, run_dir])
    prep = os.path.join(W, "prep_%s_%d" % (k, start))
    sh(["python3", os.path.join(HERE, "prepare.py"), os.path.join(TOOLS, "..", "..", "MashedGame", "FullyLoaded", "installed"), prep, os.path.join(W, k + ".exe")])
    sh(["cp", os.path.join(prep, "App_Executables", "MFL.exe"), os.path.join(run_dir, "App_Executables", "MFL.exe")])
    script = os.path.join(W, "script_%s_%d.txt" % (k, start))
    if RACE:      # menu path of race_script.sh up to the race start, then the tests wait for the race frame
        rs = subprocess.run([os.path.join(HERE, "race_script.sh"), "0x" + mapva(mp, "freeze_target"), "0x" + mapva(mp, "race_frames"), "100000", mp, "fast"], capture_output=True, text=True, env=dict(os.environ, SHOTS="0", NOLIMIT="2")).stdout.split("\n")
        pre = [l for l in rs[:next(i for i, l in enumerate(rs) if l.startswith("poke 0x") and l.endswith(" 100000"))]]
        cw = mapva(mp, "conf_wait_frame")
        open(script, "w").write("\n".join(pre + ["poke 0x%s %d" % (cw, RACE), "poke 0x%s %d" % (cs, start), "poke 0x%s 0x%s" % (ch, poll),
            "waitmem 0x%s 1 300 12 0x%s" % (cd, ci), "dump st 0x%s 0x20" % ce, "dump res 0x%s 0x40000" % cr, ""]))
    else:
      open(script, "w").write("\n".join([
        "waitmem 0x69cbd0 1 30", "waitmem 0x69d1e4 0 5", "wait 1",
        "poke 0x%s %d" % (cs, start), "poke 0x%s 0x%s" % (ch, poll),
        "waitmem 0x%s 1 300 %d 0x%s" % (cd, 12, ci),
        "dump st 0x%s 0x20" % ce, "dump res 0x%s 0x40000" % cr, ""]))
    env = dict(os.environ, PFX="wine_pfx_%d" % slot, PERSIST="slot%d" % slot)
    subprocess.run(["timeout", "-k", "5", "420", os.path.join(HERE, "play.sh"), run_dir, script, out], env=env, capture_output=True)
    res = open(os.path.join(out, "res.bin"), "rb").read() if os.path.exists(os.path.join(out, "res.bin")) else b""
    st = open(os.path.join(out, "st.bin"), "rb").read() if os.path.exists(os.path.join(out, "st.bin")) else b""
    crashed = os.path.exists(os.path.join(out, "crashed"))
    vals = struct.unpack("<8I", st) if len(st) >= 32 else None
    return dict(res=res, idx=(vals[1] if vals else None), done=(vals[2] if vals else 0), nres=(vals[3] if vals else 0), crashed=crashed)

def names():
    return [l.split() for l in open(os.path.join(TOOLS, "..", "src", "harness_conf", "conf_names.txt"))]

def main():
    """one build, then rounds: every round runs both exes from `start`; a function that hangs (or kills one process) ends the round, is logged and skipped by restarting at idx+1 (no rebuild:
    both exes see the same sequence, so the comparison stays valid). Results are kept per table index."""
    build(set())
    nm = names()
    N = len(nm)
    cres, ores, log, skipped = {}, {}, [], []
    start = 0
    for rnd in range(ROUNDS * 40):
        if start >= N: break
        out = {}
        ths = [threading.Thread(target=lambda k=k, s=s: out.__setitem__(k, run(k, s, start))) for k, s in (("ctrl", 1), ("ovl", 2))]
        [t.start() for t in ths]; [t.join() for t in ths]
        c, o = out["ctrl"], out["ovl"]
        end = lambda r: N if r["done"] else (r["idx"] if r["idx"] is not None else start)
        ec, eo = end(c), end(o)
        common = min(ec, eo)
        for i in range(start, common):
            for d, r in ((cres, c), (ores, o)):
                d[i] = r["res"][(i - start) * NSETS * REC:(i - start + 1) * NSETS * REC]
        print("round %d: start %d, ctrl %s (end %d), ovl %s (end %d)" % (rnd, start, "done" if c["done"] else ("crashed" if c["crashed"] else "hung"), ec, "done" if o["done"] else ("crashed" if o["crashed"] else "hung"), eo), flush=True)
        if common >= N: break
        why = "ctrl %s, ovl %s" % ("done" if c["done"] else "stopped", "done" if o["done"] else "stopped")
        log.append("%s %s: %s" % (nm[common][2], nm[common][3], why)); skipped.append(nm[common][2])
        start = common + 1
    lines = ["# conformance test report: function, status (OK = same return value, same changed memory words for all %d argument sets; 'fault' = the call faulted and was resumed)" % NSETS]
    ok = bad_n = 0
    for i, n in enumerate(nm):
        if i not in cres: continue
        diff, faults, notes = [], 0, []
        for s in range(NSETS):
            a = cres[i][s * REC:(s + 1) * REC]; b = ores[i][s * REC:(s + 1) * REC]
            if len(a) < REC or len(b) < REC: diff.append("set %d missing" % s); continue
            ra, rb = struct.unpack("<iIIiIIII", a), struct.unpack("<iIIiIIII", b)
            faults += ra[5]
            if a == b: continue
            f = [nme for nme, x, y in zip(("ret", "st0lo", "st0hi", "nchg", "hash", "exc", "off0", "val0"), ra, rb) if x != y]
            ptr = lambda v: 0x1000000 <= (v & 0xffffffff) < 0x7000000          # runtime heap pointer: differs between two processes by layout, not a finding
            if f == ["ret"] and ptr(ra[0]) and ptr(rb[0]): notes.append("set %d: returns a heap pointer (%x / %x), ignored" % (s, ra[0] & 0xffffffff, rb[0] & 0xffffffff)); continue
            if f == ["hash", "val0"] and ra[3] == rb[3] and ra[6] == rb[6] and ptr(ra[7]) and ptr(rb[7]): notes.append("set %d: stores a heap pointer at +%x, ignored" % (s, ra[6])); continue
            diff.append("set %d: %s differ (orig ret %x nchg %d first change +%x=%x; ours ret %x nchg %d first change +%x=%x)" % (s, "/".join(f), ra[0] & 0xffffffff, ra[3], ra[6], ra[7], rb[0] & 0xffffffff, rb[3], rb[6], rb[7]))
        if diff:
            bad_n += 1; lines.append("DIFF  %-45s %s [%s %s]" % (n[2], diff[0], n[3], n[4]))
        else:
            ok += 1; lines.append("OK    %-45s%s%s [%s %s]" % (n[2], " (%d faulted)" % faults if faults else "", (" (" + notes[0] + (", +%d more" % (len(notes) - 1) if len(notes) > 1 else "") + ")") if notes else "", n[3], n[4]))
    lines += ["", "# not tested (hang or process death): %d" % len(skipped)] + ["#   " + l for l in log]
    open(os.path.join(W, "report.txt"), "w").write("\n".join(lines) + "\n")
    print("%d OK, %d DIFF, %d skipped -> %s" % (ok, bad_n, len(skipped), os.path.join(W, "report.txt")))

main()
