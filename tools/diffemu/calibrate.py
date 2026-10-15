#!/usr/bin/env python3
"""calibrate.py: sanity checks of the harness.
 1. byte identical tier 1 / agent functions must all be EQUAL (else the harness is wrong)
 2. the shadow DIFF functions of output/tier2_known_bad.txt / shadow_t2.txt: how many does the emulator reproduce
 3. sensitivity: flip one conditional jump of our code (jcc <-> inverse), the mutant must be caught (DIFF) when the jump is covered
usage: calibrate.py [tier1|shadow|mutate|all]"""
import os, sys
try: import unicorn
except ImportError:
    py = os.environ.get("DIFFEMU_PY") or os.path.expanduser("~/.cache/mfl_diffemu_venv/bin/python"); os.execv(py, [py] + sys.argv)
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import core, diffemu
from core import ROOT, T

IDX = diffemu.index()

def fn_of(name):
    src = IDX[name][0]; obj, _ = diffemu.compile_obj(src); return diffemu.make_fn(name, src, obj)

def tier1(n=20):
    names = sorted(k for k, (p, kind) in IDX.items() if kind in ("tier1", "agent") and "/tier1/" in p or "automatch_agent" in p)[::7][:n + 6]
    bad = ok = 0
    for nm in names:
        fn, why = fn_of(nm)
        if fn is None: continue
        r = diffemu.evaluate(fn, 60, 2)
        if r["verdict"] == "INCONCLUSIVE": continue
        print("%-9s %s cases %d" % (r["verdict"], nm, r["cases"])); ok += r["verdict"] == "EQUAL"; bad += r["verdict"] != "EQUAL"
    print("tier1 / agent: %d EQUAL, %d NOT EQUAL (must be 0)" % (ok, bad)); return bad

def shadow():
    bad = [l.split()[0] for l in open(os.path.join(ROOT, "output", "tier2_known_bad.txt")) if "shadow test DIFF" in l]
    hit = 0
    for nm in bad:
        p = os.path.join(os.environ.get("TMPDIR", "/tmp/claude-%d" % os.getuid()), "dxcal", nm, "ok.c")      # the tier 2 source of these was removed from the tree: regenerate with `tools/autocompile.py <dir> output/decls.h <va> <va> 1` (dir with prelude.h, ghidra_types.h)
        if os.path.exists(p): IDX[nm] = (p, "file")
        if nm not in IDX: print("no source in the tree: " + nm); continue
        fn, why = fn_of(nm)
        if fn is None: print("%-9s %s %s" % ("INCONCL.", nm, why)); continue
        r = diffemu.evaluate(fn, 100, 2); hit += r["verdict"] == "DIFF"
        print("%-9s %s cases %d %s" % (r["verdict"], nm, r["cases"], (r["diff"][1][0][:150] if r["diff"] else dict(r["skip"]))))
    print("shadow DIFFs reproduced: %d of %d" % (hit, len(bad)))

def mutate(names=None, per=6):
    """flip jcc opcodes of our code one at a time"""
    names = names or [n for n, (p, k) in IDX.items() if k == "tier2"][:200]
    tot = caught = 0
    for nm in names:
        fn, why = fn_of(nm)
        if fn is None: continue
        base = diffemu.evaluate(fn, 60, 2)
        if base["verdict"] != "EQUAL": continue
        cands = []; code = fn.code
        for a in sorted(fn.leaders_u):      # linear decode from block starts
            pass
        for i, ins in enumerate(core.CS.disasm(code, 0)):
            if ins.mnemonic[0] == "j" and ins.mnemonic != "jmp" and ins.size in (2, 6): cands.append((i, ins.address + (0 if ins.size == 2 else 1), code[ins.address + (0 if ins.size == 2 else 1)]))
        for _, off, b in cands[:per]:
            f2, _ = fn_of(nm); f2.mutate = (off, b ^ 1)
            r = diffemu.evaluate(f2, 60, 2); tot += 1; caught += r["verdict"] == "DIFF"
            print("%-9s %s jcc at +0x%x flipped" % (r["verdict"], nm, off))
        if tot >= 40: break
    print("mutants caught: %d of %d" % (caught, tot))

if __name__ == "__main__":
    w = sys.argv[1] if len(sys.argv) > 1 else "all"
    if w in ("tier1", "all"): tier1()
    if w in ("shadow", "all"): shadow()
    if w in ("mutate", "all"): mutate()
