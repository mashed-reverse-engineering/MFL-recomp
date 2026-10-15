#!/usr/bin/env python3
"""extract_fn.py <chunk.c> <function> <out.c>: a standalone source with only one function of an autocompile chunk plus the declarations it uses (the chunk's preamble lines for the names in its body), like
autocompile.build does.  Headers next to the chunk are copied.  Used to promote functions that an agent matched by editing its own copy of a chunk."""
import os, re, shutil, sys
src, fn, out = sys.argv[1:4]
text = open(src, errors="replace").read()
lines = text.split("\n")
start = next(i for i, l in enumerate(lines) if re.match(r"^(?!extern)[A-Za-z_][\w \*]*\b%s\(.*\)$" % re.escape(fn), l) and "{" in "".join(lines[i + 1:i + 3]).strip()[:1])
j = start + 1; depth = 0; seen = False
while True:
    depth += lines[j].count("{") - lines[j].count("}"); seen = seen or "{" in lines[j]; j += 1
    if seen and depth == 0: break
k = start
while k > 0 and (lines[k - 1].startswith("/*") or lines[k - 1].startswith("   ") or lines[k - 1].strip() == ""): k -= 1
body = "\n".join(lines[start:j])
used = set(re.findall(r"[A-Za-z_]\w*", body))
pre = []
first = next((i for i, l in enumerate(lines) if re.match(r"^/\* \w+ \*/$", l)), start)
prev_end = max([i for i in range(first, start) if lines[i] == "}"] or [first])
for l in lines[:first] + lines[prev_end + 1:start]:
    if l.startswith("#include"): pre.append(l); continue
    m = re.search(r"extern [^;]*?([A-Za-z_]\w*)(\(|\s*\[|;)", re.sub(r"__declspec\([^)]*\)", "", l))      # dllimport imports: the name is not `__declspec`
    if m and m.group(1) in used and m.group(1) != fn: pre.append(l)
    elif l.startswith(("typedef", "struct", "union", "}")): pre.append(l)      # type definitions an agent added above the function
open(out, "w").write("\n".join(pre) + "\n" + "\n".join(lines[k:j]) + "\n")
for h in os.listdir(os.path.dirname(os.path.abspath(src))):
    if h.endswith(".h"): shutil.copy(os.path.join(os.path.dirname(os.path.abspath(src)), h), os.path.join(os.path.dirname(os.path.abspath(out)), h))
