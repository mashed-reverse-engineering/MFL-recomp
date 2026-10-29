#!/usr/bin/env python3
"""bytematch_install.py [--work DIR] [--dry] <name>...|@file: move byte-identical tier-2 functions to tier 1.
For each name: take the chunk text (DIR/out/<chunk> from tools/bytematch_rewrite.py --apply when it exists, else src/automatch_near/<chunk>), cut the function out with tools/extract_fn.py into
src/tier1/<name>.c, verify THAT file standalone with cmpfn.check (/O2, masked relocations + RELCHECK targets) and only then (a) write the tier-1 file, (b) delete the definition from the chunk
(the chunk keeps its externs), (c) drop the line from src/automatch_near_names.txt.  Anything that does not verify is left untouched and printed as SKIP.
After the run: tools/check_all.sh, `flock <lock> tools/gate.sh tier1`."""
import os, re, shutil, subprocess, sys, tempfile
HERE = os.path.dirname(os.path.abspath(__file__)); R = os.path.join(HERE, "..", "src")
sys.path.insert(0, HERE)
import cmpfn, bytediff_causes as bc, bytematch_rewrite as rw

args = sys.argv[1:]
work = os.path.join(os.environ.get("TMPDIR", "/tmp"), "bytematch_rewrite")
if "--work" in args: i = args.index("--work"); work = args[i + 1]; del args[i:i + 2]
dry = "--dry" in args
if dry: args.remove("--dry")
gs = "--gs" in args      # /GS stack-cookie functions: the tier-1 file gets the `/* compile: /GS */` marker and is verified with cl /GS (tools/gsfix.py)
if gs: args.remove("--gs")
if "--find-gs" in args:      # list the tier-2 names whose original code loads the security cookie (0x62c5d8) and whose chunk has no marker
    for n, (va, ch) in sorted(bc.names().items()):
        sz = cmpfn.FNSZ.get(va)
        if sz and b"\xd8\xc5\x62\x00" in cmpfn.exe_bytes(va, sz) and b"compile: /GS" not in open(os.path.join(R, "automatch_near", ch), "rb").read(): print(n)
    sys.exit(0)
want = []
for a in args:
    want += [l.split()[0] for l in open(a[1:]) if l.strip()] if a.startswith("@") else [a]
nm = bc.names()
nd = os.path.join(R, "automatch_near"); t1 = os.path.join(R, "tier1")
texts = {}; removed = []
tmp = tempfile.mkdtemp(prefix="bmi_", dir=os.environ.get("TMPDIR", "/tmp"))
for h in os.listdir(R):
    if h.endswith(".h"): shutil.copy(os.path.join(R, h), tmp)
try:
    for n in want:
        if n not in nm: print("SKIP %s: not a tier-2 name" % n); continue
        va, ch = nm[n]
        if ch not in texts:
            p = os.path.join(work, "out", ch)
            texts[ch] = open(p if os.path.exists(p) else os.path.join(nd, ch), errors="surrogateescape").read()
        t = texts[ch]
        m = next((m for m in rw.DEF.finditer(t) if m.group(1) == n), None)
        if not m: print("SKIP %s: definition not found" % n); continue
        lines = t[:m.start()].split("\n")      # leading comments: back to the previous function end / declaration line
        k = len(lines)
        while k > 0 and not re.match(r"^(\}|#include|/\*[0-9a-f]{8}\*/ extern|extern |typedef |\};|.*;\s*$)", lines[k - 1]): k -= 1
        s = len("\n".join(lines[:k])) + (1 if k else 0)
        first = next((x for x in rw.DEF.finditer(t)), None)
        pre = t[:first.start()] if first else ""      # preamble before the first function
        body = t[s:m.end()]
        used = set(re.findall(r"[A-Za-z_]\w*", body))
        if used & {"RW_GLOBAL", "rwsprintf"}: used.add("RwEngineInstance")      # macros of mfl_tier2.h / the preamble expand to it
        keep = []
        for l in pre.split("\n"):
            mm = re.search(r"extern [^;]*?(\w+)\s*(\(|\[|;)", l)
            if l.startswith("/*") and "extern" in l or l.startswith("extern"):
                if mm and mm.group(1) in used: keep.append(l)
            elif l.startswith(("/* ", "/*\n")) or l.strip() == "" or l.strip().startswith(("*", "/*")): continue
            else: keep.append(l)
        out = os.path.join(tmp, n + ".c")
        open(out, "w", errors="surrogateescape").write(("/* compile: /GS */\n" if gs and "compile: /GS" not in body else "") + "\n".join(keep) + "\n" + body)
        ok, det = cmpfn.check(out, [(n, va)], ["/O2"] + (["/GS"] if gs else []))[n]
        if not ok: print("SKIP %s: %s" % (n, det[:90])); continue
        print("OK   %s -> src/tier1/%s.c (%s)" % (n, n, det))
        if not dry:
            shutil.copy(out, os.path.join(t1, n + ".c"))
            texts[ch] = t[:s] + t[m.end():]
        removed.append(n)
    if not dry:
        for ch, t in texts.items(): open(os.path.join(nd, ch), "w", errors="surrogateescape").write(t)
        nf = os.path.join(R, "automatch_near_names.txt")
        L = [l for l in open(nf) if not (len(l.split()) >= 2 and l.split()[1] in removed)]
        open(nf, "w").write("".join(L))
    print("installed %d of %d" % (len(removed), len(want)))
finally:
    shutil.rmtree(tmp, ignore_errors=True)
