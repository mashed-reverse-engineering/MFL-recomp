#!/usr/bin/env python3
"""readable_vs_head.py [file.c ...]   object-identity proof of readability edits: every modified (git diff --name-only) src/clusters/*.c and src/automatch_near/*.c
(or the given files) is compiled as `git show HEAD:<file>` and as the working copy (flags /O2 /Gy /Ob0, headers laid out like build_overlay.py) and compared
with tools/readable_check.py (code bytes, relocations, every COMDAT).  Prints SAME / NOT SAME per file; exit 1 when any file differs."""
import glob, os, shutil, subprocess, sys, tempfile
HERE = os.path.dirname(os.path.abspath(__file__)); ROOT = os.path.dirname(HERE); R = os.path.join(ROOT, "src")
files = sys.argv[1:] or [f for f in subprocess.run(["git", "-C", ROOT, "diff", "--name-only", "--", "src/clusters", "src/automatch_near"], capture_output=True, text=True).stdout.split() if f.endswith(".c")]
bk = os.path.join(os.environ.get("TMPDIR", "/tmp/claude-%d" % os.getuid()), "readable_rename"); os.makedirs(bk, exist_ok=True); bad = 0
for f in files:
    f = os.path.relpath(os.path.abspath(f if os.path.isabs(f) or os.path.exists(f) else os.path.join(ROOT, f)), ROOT); d = tempfile.mkdtemp(prefix="rv_", dir=bk)
    for h in glob.glob(os.path.join(R, "*.h")) + [x for x in glob.glob(os.path.join(ROOT, os.path.dirname(f), "*.h")) if os.path.basename(x) not in ("prelude.h", "ghidra_types.h")]: shutil.copy(h, d)
    for h in ("ghidra_types.h", "prelude.h"): shutil.copy(os.path.join(R, "auto", h), d)
    open(os.path.join(d, "before.c"), "wb").write(subprocess.run(["git", "-C", ROOT, "show", "HEAD:" + f], capture_output=True).stdout)
    shutil.copy(os.path.join(ROOT, f), os.path.join(d, "after.c"))
    r = subprocess.run([sys.executable, os.path.join(HERE, "readable_check.py"), os.path.join(d, "before.c"), os.path.join(d, "after.c")], capture_output=True, text=True)
    ok = r.returncode == 0; bad += not ok; print("%-8s %s" % ("SAME" if ok else "NOT SAME", f)); 
    if not ok: print((r.stdout + r.stderr)[-500:])
    shutil.rmtree(d, ignore_errors=True)
sys.exit(1 if bad else 0)
