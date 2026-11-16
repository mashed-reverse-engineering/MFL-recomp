#!/usr/bin/env python3
"""fixlint.py [--selftest]: the MFL_FIX_* switch lint (W11 B5, output/qol_design_2026-11-16.md section 1).  Over every src/**/*.c / *.inc / *.h:
  - a file that mentions MFL_FIX_ includes "mfl_fix.h" and uses only the names src/mfl_fix.h defines (a misspelt name would compile to "off");
  - outside mfl_fix.h a switch is tested with `#if MFL_FIX_X`, never #ifdef / #ifndef / defined() (the header defines every switch, so those are always true);
  - every copy of mfl_fix.h under src/ equals src/mfl_fix.h (the diffemu compiler sees one directory at a time, as for mfl_tier2.h).
Exit 1 on any finding."""
import glob, os, re, sys, tempfile
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))

def lint(root):
    out = []; hdr = os.path.join(root, "mfl_fix.h")
    known = set(re.findall(r"^\s*#\s*define\s+(MFL_FIX_\w+)", open(hdr).read(), re.M)) if os.path.exists(hdr) else set()
    for p in sorted(glob.glob(os.path.join(root, "**", "*"), recursive=True)):
        if not p.endswith((".c", ".inc", ".h")) or not os.path.isfile(p): continue
        t = open(p, errors="replace").read(); rel = os.path.relpath(p, root)
        if os.path.basename(p) == "mfl_fix.h":
            if p != hdr and t != open(hdr).read(): out.append("%s: differs from src/mfl_fix.h (copy the edited one to every copy)" % rel)
            continue
        if "MFL_FIX_" not in t: continue
        if not re.search(r'^\s*#\s*include\s+"mfl_fix\.h"', t, re.M): out.append("%s: mentions MFL_FIX_ but does not include \"mfl_fix.h\"" % rel)
        for n in sorted(set(re.findall(r"\bMFL_FIX_\w+", t)) - known): out.append("%s: %s is not defined in mfl_fix.h" % (rel, n))
        for m in re.finditer(r"^\s*#\s*(ifdef|ifndef)\s+(MFL_FIX_\w+)|\bdefined\s*\(?\s*(MFL_FIX_\w+)", t, re.M):
            out.append("%s:%d: test %s with `#if`, not %s" % (rel, t[:m.start()].count("\n") + 1, m.group(2) or m.group(3), m.group(1) or "defined()"))
    return out

def selftest():
    d = tempfile.mkdtemp(); os.makedirs(d + "/sub")
    open(d + "/mfl_fix.h", "w").write("#ifndef MFL_FIX_A\n#define MFL_FIX_A 0\n#endif\n")
    open(d + "/sub/mfl_fix.h", "w").write(open(d + "/mfl_fix.h").read())
    open(d + "/sub/good.c", "w").write('#include "mfl_fix.h"\n#if MFL_FIX_A\nint x;\n#endif\n')
    assert lint(d) == [], lint(d)
    open(d + "/sub/bad.c", "w").write('#include "mfl_fix.h"\n#if MFL_FIX_AA\nint y;\n#endif\n#ifdef MFL_FIX_A\n#endif\n')      # misspelt name + #ifdef
    open(d + "/sub/noinc.c", "w").write("#if MFL_FIX_A\n#endif\n")
    open(d + "/sub2.h", "w").write(""); open(d + "/sub/mfl_fix.h", "a").write("/* edited copy */\n")
    r = lint(d)
    assert any("MFL_FIX_AA is not defined" in x for x in r) and any("not #ifdef" in x or "not ifdef" in x for x in r) and any("does not include" in x for x in r) \
        and any("differs from" in x for x in r) and len(r) == 4, r
    print("fixlint selftest: ok (clean tree passes; misspelt name, #ifdef, missing include, differing copy FAIL)")

if __name__ == "__main__":
    if sys.argv[1:] == ["--selftest"]: selftest(); sys.exit(0)
    r = lint(os.path.join(G, "src"))
    for x in r: print("FAIL:fixlint " + x)
    print("fixlint: %d finding(s)" % len(r)); sys.exit(1 if r else 0)
