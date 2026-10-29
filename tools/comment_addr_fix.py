#!/usr/bin/env python3
"""comment_addr_fix.py <old-line> <new-line> [dir ...]: exact whole-line replacement of a stale extern address comment line (comment-only, the declaration is unchanged) in src/**/*.c,h.
Each file is re-read right before its write (other agents move sources); a file is only written when the C text without comments is identical after the edit.  Used T-L3: `/*005ddc28*/ extern float k_f_0;` -> `/*005e64a4*/ extern float k_f_0;`."""
import os, re, sys
old, new = sys.argv[1], sys.argv[2]; roots = sys.argv[3:] or [os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "src")]
strip = lambda t: re.sub(r"/\*.*?\*/", "", t, flags=re.S)
n = 0
for r in roots:
    for d, _, fs in os.walk(r):
        for f in fs:
            if not f.endswith((".c", ".h")): continue
            p = os.path.join(d, f)
            try: t = open(p, errors="surrogateescape").read()
            except OSError: continue
            if old not in t: continue
            u = t.replace(old, new)
            if strip(u).split("\n") != strip(t).split("\n") and strip(u) != strip(t): sys.exit("not comment-only: " + p)
            open(p, "w", errors="surrogateescape").write(u); n += 1
print("rewrote %d files" % n)
