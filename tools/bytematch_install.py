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
RWMAC = {"RW_GLOBAL"} | set(re.findall(r"^#define (\w+)[^\n]*\bRW_GLOBAL\(", open(os.path.join(R, "mfl_tier2.h"), errors="replace").read(), re.M))
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
        while k > 0 and (re.match(r"^\s*\*", lines[k - 1]) or not re.match(r"^(\}|#include|/\*[0-9a-f]{8}\*/ extern|extern |typedef |\};|.*;\s*$)", lines[k - 1])): k -= 1      # a ';' inside a leading block comment does not end the walk
        s = len("\n".join(lines[:k])) + (1 if k else 0)
        first = next((x for x in rw.DEF.finditer(t)), None)
        pre = t[:first.start()] if first else ""      # preamble before the first function
        body = t[s:m.end()]
        used = set(re.findall(r"[A-Za-z_]\w*", body))
        helpers = []      # (start, end, name) of helper `#define`s / `typedef struct {...} name;` blocks above the function that its body uses (anywhere in the chunk, not only in the leading walk)
        grow = True
        while grow:
            grow = False
            for hm in re.finditer(r"^(?:/\*[^\n]*\*/\n)?(?:#define (\w+)[^\n]*\n|typedef struct[^{;]*\{.*?\n\}\s*(\w+);\n)", t[:s], re.M | re.S):
                hn = hm.group(1) or hm.group(2)
                if hn not in used or any(h[0] == hm.start() for h in helpers): continue
                helpers.append((hm.start(), hm.end(), hn)); used |= set(re.findall(r"[A-Za-z_]\w*", hm.group(0))); grow = True
        helpers.sort()
        hcut = [h for h in helpers if not re.search(r"\b%s\b" % h[2], t[:h[0]] + t[h[1]:s] + t[m.end():])]      # not used by another function of the chunk: cut it there, else only copy
        body = "".join(t[a_:b_] for a_, b_, _ in helpers) + body
        if used & RWMAC: used.add("RwEngineInstance")      # macros of mfl_tier2.h / the preamble expand to it (RwFree, RwMalloc, rwsprintf, ...)
        keep = []
        in_c = False
        for l in pre.split("\n"):
            if in_c:      # continuation line of a multi-line block comment: drop (a line that does not start with ' *' leaked through before)
                in_c = "*/" not in l; continue
            if l.startswith("/*") and "*/" not in l: in_c = True; continue
            mm = re.search(r"extern [^;]*?(\w+)\s*(\(|\[|;)", l)
            if l.startswith("/*") and "extern" in l or l.startswith("extern"):
                if mm and mm.group(1) in used: keep.append(l)
            elif l.startswith(("/* ", "/*\n")) or l.strip() == "" or l.strip().startswith(("*", "/*")): continue
            else: keep.append(l)
        bl = set(body.split("\n")); keep = [l for l in keep if not (l.startswith("#define") and l in bl)]      # a #define that the body block already carries
        out = os.path.join(tmp, n + ".c")
        open(out, "w", errors="surrogateescape").write(("/* compile: /GS */\n" if gs and "compile: /GS" not in body else "") + "\n".join(keep) + "\n" + body)
        ok, det = cmpfn.check(out, [(n, va)], ["/O2"] + (["/GS"] if gs else []))[n]
        if not ok: print("SKIP %s: %s" % (n, det[:90])); continue
        print("OK   %s -> src/tier1/%s.c (%s)" % (n, n, det))
        m_end = m.end()
        if not dry:
            shutil.copy(out, os.path.join(t1, n + ".c"))
            for a_, b_, _ in sorted(hcut, reverse=True): t = t[:a_] + t[b_:]; s -= b_ - a_; m_end -= b_ - a_
            texts[ch] = t[:s] + t[m_end:]
        removed.append(n)
    if not dry:
        for ch, t in texts.items(): open(os.path.join(nd, ch), "w", errors="surrogateescape").write(t)
        nf = os.path.join(R, "automatch_near_names.txt")
        L = [l for l in open(nf) if not (len(l.split()) >= 2 and l.split()[1] in removed)]
        open(nf, "w").write("".join(L))
    print("installed %d of %d" % (len(removed), len(want)))
finally:
    if not os.environ.get("BMI_KEEP"): shutil.rmtree(tmp, ignore_errors=True)      # BMI_KEEP=1: leave the generated standalone files for debugging
