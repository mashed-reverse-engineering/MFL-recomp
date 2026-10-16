#!/usr/bin/env python3
"""t2_classify.py RUN_DIR [out.txt]: pattern table of the DIFF / LINT / COMPILE-ERROR functions of a t2_auto run (count, bytes, 2 examples per pattern).
Pattern = the defectlint finding classes the generated text shows (the mechanical ones first: fbits, sgn, enum), else the first diverging observable of the diffemu diff / the compile error code."""
import csv, collections, json, os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import defectlint as d
R = os.path.abspath(sys.argv[1]); out = sys.argv[2] if len(sys.argv) > 2 else os.path.join(R, "patterns.txt")
d.load_gt(); gd = d.load_decls(); d.ENABLED.update(l.strip() for l in open(os.path.join(d.G, "src", "bridges", "enabled.txt")))
rows = [x for x in csv.DictReader(open(os.path.join(R, "results.csv"))) if x["verdict"] in ("DIFF", "LINT", "COMPILE-ERROR")]
NAME = {"fbits": "float arg to int/() parameter (bit copy needed)", "sgn": "undefined4 compared with signed value ((int) needed)", 2: "float/dword bit copy (class 2)", 3: "_ftol() without ST0", 6: "vtable call convention", 7: "register callee called plain",
        8: "() callee, original pushes more args (class 8)", 10: "callee reads more stack args than passed (class 10)", 9: "frame layout: one frame object split into locals (class 9)", "arity": "call arity != prototype (class 4)"}
def pattern(x):
    f = os.path.join(R, "files", x["name"] + ".c")
    if x["verdict"] == "COMPILE-ERROR":
        m = re.search(r"C\d+ '?([^':]*)'?", x["detail"]); code = re.search(r"C\d+", x["detail"])
        sym = re.search(r"C2065 '(\w+)'", x["detail"])
        return "compile %s%s" % (code.group(0) if code else "other", (" " + ("enum " if sym and re.match(r"(COURSE|WEAPON)_", sym.group(1)) else "") + sym.group(1)) if sym and re.match(r"(COURSE|WEAPON)_", sym.group(1)) else ""), x["detail"][:90]
    ks = set()
    if os.path.exists(f):
        L = d.Lint(f, open(f).read(), gd); fi = L.run()
        for h in L.hints: ks.add(h[0])
        for a in fi:
            c = a["cls"]
            if c == 4 and "called with" in (a.get("note") or "") or c == 4 and "pushes pending" in (a.get("note") or ""): ks.add("arity")
            elif c in (1, 4): pass
            else: ks.add(c)
    p = os.path.join(R, "diffemu", "diffemu_diffs", x["name"] + ".txt"); t = open(p).read() if os.path.exists(p) else ""
    first = ""
    m = re.search(r"orig \('(\w+)', \((.*?)\), .*?\) ours \('(\w+)', \((.*?)\)", t)
    if m: first = "call %s arg%s" % (m.group(1), "/" + m.group(3) if m.group(3) != m.group(1) else "")
    elif "frame object" in t: first = "frame object behind pointer arg"
    elif "does not return" in t: first = "ours does not return"
    elif "x87 stack" in t: first = "x87 stack depth"
    elif "memory:" in t: first = "memory writes"
    key = "+".join(str(k) for k in sorted(ks, key=str)) if ks else "no lint finding"
    return key + " | " + (first.split(" arg")[0] if first.startswith("call") and False else ("call trace" if first.startswith("call") else first)), first
c = collections.defaultdict(list)
for x in rows:
    k, ex = pattern(x); c[k].append((x["name"], int(x["bytes"]), ex))
L = ["patterns for %d functions (%d bytes) of %s" % (len(rows), sum(int(x["bytes"]) for x in rows), R)]
for k, v in sorted(c.items(), key=lambda kv: -sum(a[1] for a in kv[1])):
    kk = " + ".join(str(NAME.get(int(p) if p.isdigit() else p, p)) for p in k.split(" | ")[0].split("+")) + (" | " + k.split(" | ")[1] if " | " in k else "")
    L.append("%4d fns %6d B  %s" % (len(v), sum(a[1] for a in v), kk))
    for a in v[:2]: L.append("      %-38s %5d B  %s" % (a[0], a[1], a[2][:100]))
open(out, "w").write("\n".join(L) + "\n"); print("\n".join(L))
