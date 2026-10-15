#!/usr/bin/env python3
"""cluster_decls.py FILE.c...: fills the `/* DECLS-BEGIN */ .. /* DECLS-END */` block of a hand written cluster file (output/cluster_pilot) with the
`extern` lines of output/decls.h for every identifier the file uses and does not declare itself (own `extern` lines win, so a cluster file can give globals a real type)."""
import os, re, sys
G = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
def rd(p): return open(os.path.join(G, *p.split("/")), errors="replace")
std = {l.split()[0] for l in rd("output/stdcall.txt") if l.strip()}                        # va of every stdcall function (decls.h carries no convention)
fix = dict(l.split()[:2] for l in rd("names/MFL_conv_fix.txt") if l.strip() and l[0] != "#")  # name -> cdecl|stdcall, wins over stdcall.txt
regs = {l.split(",")[0]: l.split(",")[2] for l in rd("output/regsigs.csv")}                  # name -> register inputs (usercall)
dl = {}
for l in rd("output/decls.h"):
    m = re.match(r"/\*([0-9a-f]{8})\*/ extern (.*?)\b(\w+)\s*(\(|\[|;)", l)
    if not m: continue
    va, pre, n, br = m.groups(); l = l.rstrip()
    if br == "(":
        cv = fix.get(n) or ("stdcall" if va in std else "stdcall" if "__stdcall" in pre else "cdecl")      # decls.h only as the last resort
        l = l.replace("__stdcall ", "", 1); pre = pre.replace("__stdcall ", "", 1)
        if cv == "stdcall": l = l.replace(pre + n, pre + "__stdcall " + n, 1)
        if n in regs: l += " /* REGCONV %s: write as natural C or hook it with the @ stub option */" % regs[n]
        if pre.strip() == "undefined": l = l.replace("extern undefined", "extern /* RET? */ undefined", 1)
    dl.setdefault(n, l)
for f in sys.argv[1:]:
    t = open(f).read(); a, b = t.index("/* DECLS-BEGIN */"), t.index("/* DECLS-END */")
    body = t[:a] + t[b:]
    own = set(re.findall(r"\bextern\b[^;(]*?\b(\w+)\s*(?:\(|\[|;)", body)) | set(re.findall(r"^(?!extern|typedef)[\w \*]+?\b(\w+)\(.*\)$", body, re.M))
    code = re.sub(r"/\*.*?\*/", " ", body, flags=re.S); local = set(re.findall(r"[\w\*]\s+\**(\w+)\s*[,;)=\[]", code)) - set(re.findall(r"\b(\w+)\s*\[[^\]]*\]\s*\.", code))
    used = sorted(n for n in set(re.findall(r"\b\w+\b", code)) if n in dl and n not in own and n not in local)
    for n in used:
        if "/* RET? */" in dl[n]: print("WARNING %s: %s: return type undefined (RET?), check the callers' use of EAX / ST0" % (f, n), file=sys.stderr)
    open(f, "w").write(t[:a] + "/* DECLS-BEGIN */\n" + "".join(dl[n] + "\n" for n in used) + t[b:])
    print(f, "decls:", " ".join(used))
