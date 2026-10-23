#!/usr/bin/env python3
"""readable_rename.py <file.c> <map.txt>   word-boundary renames + object-identity proof (tools/readable_check.py).
map.txt lines: `@cmt <anchor> ## <text>` (insert a comment line before the unique line containing <anchor>); `old new` (identifier rename, whole file, not inside strings/comments is NOT checked: keep old names unique) or `@lit <regex> <replacement>` (regex substitution, one line each).
The file is rewritten only when readable_check says SAME against the pre-edit text (backup kept in $TMPDIR/readable_rename/); else untouched.  Comment-only edits: pass an empty map and edit by hand, then run readable_check.py <backup> <file>."""
import os, re, shutil, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__)); f, m = sys.argv[1], sys.argv[2]
bk = os.path.join(os.environ.get("TMPDIR", "/tmp/claude-%d" % os.getuid()), "readable_rename"); os.makedirs(bk, exist_ok=True)
t = open(f).read(); n = t
for ln in open(m):
    p = ln.rstrip("\n").split("\t") if ln.startswith("@lit\t") else ln.split()      # `@lit<TAB>regex<TAB>replacement` (regex may contain spaces)
    if not p or p[0].startswith("#"): continue
    if p[0] == "@cmt":      # `@cmt <anchor text> ## <comment>`: comment line (same indent) before the unique line containing the anchor
        anc, cm = ln.split(None, 1)[1].split(" ## ", 1); L = n.split("\n"); ix = [i for i, x in enumerate(L) if anc in x]
        if len(ix) != 1: sys.exit("@cmt anchor not unique (%d): %s" % (len(ix), anc))
        i = ix[0]; L.insert(i, re.match(r"\s*", L[i]).group(0) + "/* " + cm.strip() + " */"); n = "\n".join(L)
    elif p[0] == "@lit": n = re.sub(p[1], p[2] if "\t" in ln[5:] else " ".join(p[2:]), n)
    else: n = re.sub(r"(?<![\w.])%s\b" % re.escape(p[0]), p[1], n) if False else re.sub(r"\b%s\b" % re.escape(p[0]), p[1], n)
import glob, tempfile
R = os.path.join(HERE, "..", "src"); d = tempfile.mkdtemp(prefix="rr_", dir=bk)      # headers laid out like build_overlay.py (src/*.h, the file's dir, then src/auto prelude / ghidra_types)
for h in glob.glob(os.path.join(R, "*.h")) + [x for x in glob.glob(os.path.join(os.path.dirname(os.path.abspath(f)), "*.h")) if os.path.basename(x) not in ("prelude.h", "ghidra_types.h")]: shutil.copy(h, d)
for h in ("ghidra_types.h", "prelude.h"): shutil.copy(os.path.join(R, "auto", h), d)
b = os.path.join(d, "before.c"); a = os.path.join(d, "after.c")
open(b, "w").write(t); open(a, "w").write(n)
try:
    r = subprocess.run([sys.executable, os.path.join(HERE, "readable_check.py"), b, a], capture_output=True, text=True)
    last = (r.stdout.strip().split("\n") or [""])[-1]
    print(os.path.basename(f), last, "" if r.returncode == 0 else (r.stdout + r.stderr)[-600:])
    if r.returncode == 0 and n != t: shutil.copy(f, os.path.join(bk, os.path.basename(f))); open(f, "w").write(n)
finally:
    shutil.rmtree(d, ignore_errors=True)
