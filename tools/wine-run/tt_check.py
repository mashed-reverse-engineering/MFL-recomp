#!/usr/bin/env python3
"""tt_check.py [--ours <run dir>] <run dir> [<run dir>...]: (--ours: judge that run's ours against the controls of the other dirs only;
the negative fixture: ours of lesson N against the controls of lesson M must FAIL)
behaviour verdict of training_tt runs (lane TT, W10).
A run dir is a scenario.sh SCEN_EXE dir (out_ctrl/ out_ours/, probe.txt from training_tt.sh).  Every out_* of every run dir is summarised:
  lesson   g_training_lesson (0x8c8078) seen during the lesson; row = g_career_row (row.bin)
  steps    g_car_8c86c8 values in order (consecutive repeats collapsed)
  texts    g_ui_69d2dc text ids in order while a dialog is up (g_car_69d2d8 != 0), repeats collapsed
  done     GameState progress[row*18+17] == 2 in gs_end.bin (the lesson's completion mark)
  trap     mfl_trap.log in the run's App_Executables (release links only; a harness link has none)
PASS rule (ours vs every control): no crash (scenario.sh RESULT line), same done, same steps, same texts, gs_end differs from the
control only in bytes where the controls differ among themselves (needs two controls; with one control: 0 bytes), no trap log.
A control that did not complete the lesson makes the lesson UNJUDGED (exit 0, printed), not a PASS.
Exit 0 PASS / UNJUDGED, 1 FAIL.  --selftest: built-in check of the summary and the rule."""
import glob, os, struct, sys

def progress(ev):
    """ev: (step, None) on a step change, (step, text id) on a new dialog text.  A step seen before = a failed attempt and its retry
    (lesson 0: 1 -> 3 (failed) -> 1; lesson 3: 13 -> 100 -> 13): the detour and the first attempt are dropped, the retry kept.
    -> (list of (step, text) of the successful path, number of retries)"""
    path, retries = [], 0
    for st, tx in ev:
        if tx is None and any(p[0] == st for p in path):
            retries += 1; path = path[:next(i for i, p in enumerate(path) if p[0] == st)]
        path.append((st, tx))
    return path, retries

def summary(out, run=None):
    ev = []
    p = os.path.join(out, "probe.txt"); s = {"dir": out, "lesson": None, "row": None, "steps": [], "texts": [], "done": None, "trap": False}
    if os.path.exists(p):
        for l in open(p, errors="replace"):
            v = l.split()
            if len(v) != 6: continue
            try: t, lesson, step, dlg, txt, phase = float(v[0]), *map(int, v[1:])
            except ValueError: continue
            if phase == 6 and s["lesson"] is None: s["lesson"] = lesson
            if not s["steps"] or s["steps"][-1] != step: s["steps"].append(step); ev.append((step, None))
            if dlg and txt and (not s["texts"] or s["texts"][-1] != txt): s["texts"].append(txt); ev.append((step, txt))
    s["path"], s["retries"] = progress(ev)
    r = os.path.join(out, "row.bin"); g = os.path.join(out, "gs_end.bin")
    if os.path.exists(r) and os.path.getsize(r) >= 4: s["row"] = struct.unpack("<I", open(r, "rb").read(4))[0]
    if s["row"] is not None and s["row"] < 15 and os.path.exists(g):
        d = open(g, "rb").read(); o = 4 * (s["row"] * 18 + 17)
        if len(d) >= o + 4: s["done"] = struct.unpack_from("<I", d, o)[0] == 2
    m = os.path.join(out, "missed.txt")      # W12 hx5a: training_tt's trainloop hit its safety cap (or the game went away) before the done word
    s["miss"] = [l.strip() for l in open(m, errors="replace") if l.startswith("MISS trainloop")] if os.path.exists(m) else []
    if run: s["trap"] = bool(glob.glob(os.path.join(run, "run_%s" % os.path.basename(out)[4:], "App_Executables", "mfl_trap.log")))
    return s

def gsdiff(a, b):
    try: x, y = open(a, "rb").read(), open(b, "rb").read()
    except OSError: return None
    return {i for i in range(min(len(x), len(y))) if x[i] != y[i]} | set(range(min(len(x), len(y)), max(len(x), len(y))))

def crashed(run):
    p = os.path.join(run, "result.txt"); t = open(p, errors="replace").read() if os.path.exists(p) else ""
    return {k: ("%s: CRASH" % k) in t or ("%s : CRASH" % k) in t for k in ("ctrl", "ours")}

def judge(ctrls, ours, noise):
    """ctrls / ours: summaries; noise: set of GameState byte offsets that differ between controls (None = unknown). -> (verdict, reasons)"""
    c0 = ctrls[0]
    if not c0["done"]: return "UNJUDGED", ["control did not complete the lesson"] + ["control: " + x for x in c0.get("miss", [])]
    bad = []
    for c in ctrls[1:]:
        if (c["done"], c["path"]) != (c0["done"], c0["path"]): bad.append("controls disagree (done / progression): no verdict on ours")
    if bad: return "UNJUDGED", bad
    for o in ours:
        tag = os.path.basename(os.path.dirname(o["dir"])) + "/" + os.path.basename(o["dir"])
        if o.get("crash"): bad.append("%s crashed" % tag)
        if o["done"] != c0["done"]: bad.append("%s lesson done %s, control %s" % (tag, o["done"], c0["done"]))
        for x in o.get("miss", []): bad.append("%s %s (lesson not finished within the safety cap)" % (tag, x))
        if o["path"] != c0["path"]: bad.append("%s progression %s, control %s" % (tag, pfmt(o["path"]), pfmt(c0["path"])))
        if o["trap"]: bad.append("%s mfl_trap.log" % tag)
        d = o.get("gsdiff")
        if d is None: bad.append("%s no gs_end dump" % tag)
        else:
            extra = d - (noise or set())
            if extra: bad.append("%s GameState differs beyond control noise at %d bytes (first +0x%x)" % (tag, len(extra), min(extra)))
    return ("FAIL" if bad else "PASS"), bad

fmt = lambda t: " ".join("%x" % x for x in t)
pfmt = lambda p: " ".join("s%d" % st if tx is None else "%x" % tx for st, tx in p)

def main(runs):
    ctrls, ours, only = [], [], None
    if runs[:1] == ["--ours"]: only, runs = runs[1], runs[2:] + [runs[1]]      # negative fixture: ours of that run only, controls of the others only
    for run in runs:
        cr = crashed(run)
        for k, lst in (("ctrl", ctrls), ("ours", ours)):
            if only and (k == "ours") != (run == only): continue
            out = os.path.join(run, "out_" + k)
            if os.path.isdir(out): s = summary(out, run); s["crash"] = cr[k]; lst.append(s)
    if not ctrls or not ours: print("tt_check: need out_ctrl and out_ours"); return 1
    cg = [os.path.join(c["dir"], "gs_end.bin") for c in ctrls]
    noise = set()
    for g in cg[1:]:
        d = gsdiff(cg[0], g); noise |= d if d is not None else set()
    for o in ours: o["gsdiff"] = gsdiff(cg[0], os.path.join(o["dir"], "gs_end.bin"))
    for s in ctrls + ours:
        print("%-40s lesson %s row %s done %s retries %d trap %s\n    raw steps %s texts [%s]\n    progression %s" % (s["dir"][-40:], s["lesson"], s["row"], s["done"], s["retries"], s["trap"], s["steps"], fmt(s["texts"]), pfmt(s["path"])))
        for x in s["miss"]: print("    " + x)
    print("GameState control noise: %d bytes (%d controls)" % (len(noise), len(ctrls)))
    v, why = judge(ctrls, ours, noise)
    for w in why: print("  " + w)
    print("TT: %s" % v); return 1 if v == "FAIL" else 0

def selftest():
    # g0 control: 0 (texts) 1 2dd 3 2dc 1 2dd 2 2de -> retry removed = ours 0 1 2dd 2 2de
    pc, rc_ = progress([(0, None), (0, 0x2d6), (1, None), (1, 0x2dd), (3, None), (3, 0x2dc), (1, None), (1, 0x2dd), (2, None), (2, 0x2de)])
    po, ro = progress([(0, None), (0, 0x2d6), (1, None), (1, 0x2dd), (2, None), (2, 0x2de)])
    assert pc == po and rc_ == 1 and ro == 0
    assert progress([(0, None), (0, 0x2d6), (2, None)])[0] != po      # a skipped step stays visible
    c = {"dir": "a/out_ctrl", "lesson": 0, "row": 0, "path": po, "done": True, "trap": False}
    o = dict(c, dir="a/out_ours", gsdiff=set())
    assert judge([c], [o], set())[0] == "PASS"
    assert judge([c], [dict(o, path=progress([(0, None), (0, 0x2d7), (0, 0x2d6)])[0])], set())[0] == "FAIL"
    assert judge([c], [dict(o, path=po[:2])], set())[0] == "FAIL"      # stopped early / skipped the rest
    assert judge([c], [dict(o, done=False)], set())[0] == "FAIL"
    assert judge([c], [dict(o, gsdiff={5})], {5})[0] == "PASS"
    assert judge([c], [dict(o, gsdiff={6})], {5})[0] == "FAIL"
    assert judge([c], [dict(o, trap=True)], set())[0] == "FAIL"
    assert judge([dict(c, done=False)], [o], set())[0] == "UNJUDGED"
    assert judge([c, dict(c, path=po[:3])], [o], set())[0] == "UNJUDGED"
    v, why = judge([c], [dict(o, done=False, miss=["MISS trainloop 0x7eecdc==2: safety timeout 600s"])], set())      # W12 hx5a: the cap is named
    assert v == "FAIL" and any("safety cap" in w for w in why), why
    v, why = judge([dict(c, done=False, miss=["MISS trainloop x: safety timeout 600s"])], [o], set())
    assert v == "UNJUDGED" and any("safety timeout" in w for w in why), why
    print("selftest ok"); return 0

if __name__ == "__main__":
    a = sys.argv[1:]
    sys.exit(selftest() if a == ["--selftest"] else main(a) if a else (print(__doc__) or 2))
