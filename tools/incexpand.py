#!/usr/bin/env python3
"""incexpand.py: inline the local `#include "<name>.inc"` lines of a C source, as cl does, so the text linters (defectlint, portcheck, declcheck, rawaddr,
cheatgrep, stack_lint) see a KES group's decls + bind block (src/automatch_near/an_hk_612_<grp>.inc, engine switch window 3 plan section 1).
Only quoted includes ending in .inc and found next to the file are inlined (one level is enough; nested .inc are inlined too).  `python3 incexpand.py` = self-test."""
import os, re

INC = re.compile(r'^[ \t]*#[ \t]*include[ \t]+"([\w.-]+\.inc)"[^\n]*$', re.M)


def expand(text, d, read=None):
    """text of a file in directory d -> text with its .inc includes inlined; read(path) -> text (default: the file on disk; defectlint --ref passes git show)"""
    read = read or (lambda p: open(p, errors="replace").read() if os.path.exists(p) else None)
    def sub(m):
        t = read(os.path.join(d, m.group(1)))
        return m.group(0) if t is None else expand(t.rstrip("\n"), d, read)
    return INC.sub(sub, text)


def incs(text):
    """the .inc names a source includes"""
    return INC.findall(text)


def read_expanded(path):
    return expand(open(path, errors="replace").read(), os.path.dirname(path))


if __name__ == "__main__":
    import tempfile
    d = tempfile.mkdtemp()
    open(os.path.join(d, "a_g.inc"), "w").write('extern int g;\n#include "a_h.inc"\n')
    open(os.path.join(d, "a_h.inc"), "w").write("extern int h;\n")
    open(os.path.join(d, "a.c"), "w").write('#include "mfl_tier2.h"\n#include "a_g.inc"\n#include "missing.inc"\nint f(void) { return g + h; }\n')
    t = read_expanded(os.path.join(d, "a.c"))
    for n in ("a_g.inc", "a_h.inc", "a.c"): os.remove(os.path.join(d, n))
    os.rmdir(d)
    assert t == '#include "mfl_tier2.h"\nextern int g;\nextern int h;\n#include "missing.inc"\nint f(void) { return g + h; }\n', repr(t)
    assert incs('#include "x_y.inc"\n#include "z.h"\n') == ["x_y.inc"]
    print("incexpand self-test ok")
