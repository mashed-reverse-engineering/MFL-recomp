#!/usr/bin/env python3
"""residual.py WORKDIR [SRC]: classify what still differs for the non-hit functions of a bytematch_permute X87=1 run (best text of DIR/cache/*.json compiled /O2 /G5, diffed against the original):
regop_first (fld st(i)/fop st(i) vs memory operand first, fxch, pops), spill_frame (esp slots / sub esp), reg_role (only register names differ), control_flow (jumps / compare form), other.  -> CSV on stdout."""
import difflib, json, os, re, sys, glob
HERE = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, os.path.join(HERE, ".."))
import cmpfn, asmdiff, bytediff_causes as bc
work = sys.argv[1]
print("name,hit,best_insn,best_bytes,class")
for f in sorted(glob.glob(os.path.join(work, "cache", "*.json"))):
    st = json.load(open(f)); name = os.path.basename(f)[:-5]
    if st["hit"]: print("%s,1,0,0,hit" % name); continue
    src = os.path.join(work, "_r_%s.c" % name); open(src, "w").write(st["best"])
    va = None
    for l in open(os.path.join(os.path.dirname(os.path.normpath(sys.argv[2] if len(sys.argv) > 2 else work)), "names.txt")) if False else []: pass
    nm = {l.split()[1]: int(l.split()[0], 16) for l in open(os.path.join(sys.argv[2], "names.txt"))}
    va = nm[name]
    pre = "#include <math.h>\n"
    open(src, "w").write(pre + st["best"])
    ok, det = cmpfn.check(src, [(name, va)], ["/O2", "/G5"])[name]
    last = cmpfn.LAST.get(name)
    if not last: print("%s,0,%s,%s,compile" % (name, st["best_score"][0], st["best_score"][1])); continue
    code, masked = last; ob = cmpfn.exe_bytes(va, cmpfn.FNSZ.get(va) or len(code))
    ours = bc.norm(asmdiff.disasm(bc.fill(code, masked, ob))); org = bc.norm(asmdiff.disasm(ob))
    d = [l for o in difflib.SequenceMatcher(None, ours, org, autojunk=False).get_opcodes() if o[0] != "equal" for l in ours[o[1]:o[2]] + org[o[3]:o[4]]]
    t = " ".join(d)
    if re.search(r"fxch|st\(\d\)|fstp st|fld st", t): c = "regop_first"
    elif re.search(r"esp", t): c = "spill_frame"
    elif re.search(r"\bj[a-z]+\b", t): c = "control_flow"
    elif d and all(re.sub(r"\b(e[a-ds][xi]|e[sd]i|ebp)\b", "R", x) in [re.sub(r"\b(e[a-ds][xi]|e[sd]i|ebp)\b", "R", y) for y in d] for x in d): c = "reg_role"
    else: c = "other"
    print("%s,0,%s,%s,%s" % (name, st["best_score"][0], st["best_score"][1], c))
