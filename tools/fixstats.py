#!/usr/bin/env python3
"""fixstats.py <original chunk dir> <agent dir>...: classifies what matching agents changed (diff of each edited chunk against the original chunk) so that repeated patterns can become systematic fixers.
Categories per changed line pair: RET_TYPE (extern return type), CONV (__cdecl / __stdcall), PARAMS (extern argument list), GLOBAL_TYPE (extern variable type), BODY (function body), ADDED (new extern / typedef / pragma / asm).
Prints one line per function (agent directory) and a count per category.  Usage example: fixstats.py loop/r1/ac scratchpad/wave1 scratchpad/wave2"""
import collections, difflib, glob, os, re, sys
orig_dir = sys.argv[1]
def classify(a, b):
    ea = re.match(r"(/\*\w+\*/ )?extern (.*?)(__cdecl |__stdcall )?(\w+)\((.*)\);$", a or "")
    eb = re.match(r"(/\*\w+\*/ )?extern (.*?)(__cdecl |__stdcall )?(\w+)\((.*)\);$", b or "")
    if ea and eb and ea.group(4) == eb.group(4):
        if ea.group(2).strip() != eb.group(2).strip(): return "RET_TYPE"
        if (ea.group(3) or "") != (eb.group(3) or ""): return "CONV"
        return "PARAMS"
    ga = re.match(r"(/\*\w+\*/ )?extern ([^(;]*?)(\w+)(\[\d*\])?;$", a or "")
    gb = re.match(r"(/\*\w+\*/ )?extern ([^(;]*?)(\w+)(\[\d*\])?;$", b or "")
    if ga and gb and ga.group(3) == gb.group(3): return "GLOBAL_TYPE"
    if a is None or (b and re.match(r"(extern|typedef|#pragma|struct)", b.strip())): return "ADDED"
    return "BODY"
tot = collections.Counter(); per = {}
for d in sys.argv[2:]:
    for fdir in sorted(glob.glob(os.path.join(d, "*"))):
        if not os.path.isdir(fdir): continue
        for f in glob.glob(os.path.join(fdir, "ok_*.c")):
            o = os.path.join(orig_dir, os.path.basename(f))
            if not os.path.exists(o): continue
            A, B = open(o, errors="replace").read().split("\n"), open(f, errors="replace").read().split("\n")
            cats = collections.Counter()
            for tag, i1, i2, j1, j2 in difflib.SequenceMatcher(None, A, B, autojunk=False).get_opcodes():
                if tag == "equal": continue
                for k in range(max(i2 - i1, j2 - j1)):
                    a = A[i1 + k] if i1 + k < i2 else None; b = B[j1 + k] if j1 + k < j2 else None
                    cats[classify(a, b)] += 1
            if cats: per[os.path.basename(fdir)] = cats; tot.update(set(cats))
for fn, c in sorted(per.items()): print("%-40s %s" % (fn, " ".join("%s=%d" % kv for kv in sorted(c.items()))))
print("functions per category:", dict(tot), "of", len(per), "edited")
