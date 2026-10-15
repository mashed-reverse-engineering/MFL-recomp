#!/usr/bin/env python3
"""shadow_merge.py <sweep dir with c*/ (or courses_par p*/c*/) out_ovl_1/shadowlog.bin> <shadow_names.txt> [out table] [--record]: merges the tier 2 shadow logs of all courses.
Per function: courses tested / courses with a difference / total tested calls / first difference.  Table lines: name STATUS tested_calls courses_ok courses_diff first_diff
--record: store hash + status of every line in output/shadow_cache.csv (tools/shadow_cache.py; only after a run of the CURRENT sources, then `shadowgen2.py ... --changed` skips the OK ones).
STATUS: OK (>= 1 tested call, never different), DIFF, CALLED (executed but never compared: called while another test ran or before the race), UNTESTED (never called)."""
import bisect, glob, os, re, struct, sys
rec = "--record" in sys.argv; av = [x for x in sys.argv[1:] if x != "--record"]
sweep, names_f = av[0], av[1]
out_f = av[2] if len(av) > 2 else None
names = [l.strip() for l in open(names_f) if l.strip()]
syms = []
for l in open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "src", "symbols.txt")):
    p = l.split()
    if len(p) == 2:
        try: syms.append((int(p[1], 16), p[0]))
        except ValueError: pass
syms.sort(); keys = [s[0] for s in syms]
def sym(a):
    i = bisect.bisect_right(keys, a) - 1
    return "%s+%#x" % (syms[i][1], a - syms[i][0]) if i >= 0 and a - syms[i][0] < 0x10000 else "heap/stack"
SZ = 124
tab = {n: dict(tested=0, calls=0, ok=[], bad=[], first="") for n in names}
for f in sorted(glob.glob(os.path.join(sweep, "c*/out_ovl_1/shadowlog.bin")) + glob.glob(os.path.join(sweep, "p*/c*/out_ovl_1/shadowlog.bin"))):
    c = int(re.search(r"/c(\d+)/out_ovl_1", f).group(1)); d = open(f, "rb").read()
    for i in range(len(d) // SZ):
        r = struct.unpack_from("<5i8I8I8I2I", d, i * SZ)
        fid, calls, tested, retd, nd = r[:5]
        if fid != i or i >= len(names) or calls == 0: continue
        tab[names[i]]["calls"] += calls
        if tested == 0: continue
        t = tab[names[i]]; t["tested"] += tested
        if nd == 0 and retd == 0: t["ok"].append(c)
        else:
            t["bad"].append(c)
            if not t["first"]:
                w = "; ".join("%s %08x/%08x" % (sym(r[5 + k]), r[13 + k], r[21 + k]) for k in range(min(nd, 3)))
                t["first"] = "c%d ret %d/%d (%#x vs %#x), %d words %s" % (c, retd, tested, r[29], r[30], nd, w)
lines = []
for n in names:
    t = tab[n]
    st = "DIFF" if t["bad"] else "OK" if t["ok"] else "CALLED" if t["calls"] else "UNTESTED"
    lines.append("%s %s %d %s %s %s" % (n, st, t["tested"], ",".join(map(str, t["ok"])) or "-", ",".join(map(str, t["bad"])) or "-", t["first"]))
cnt = {s: sum(1 for l in lines if l.split()[1] == s) for s in ("OK", "DIFF", "CALLED", "UNTESTED")}
print("%d functions: %d OK, %d DIFF, %d CALLED (executed, but nested / early: not compared), %d never called" % (len(names), cnt["OK"], cnt["DIFF"], cnt["CALLED"], cnt["UNTESTED"]))
if out_f: open(out_f, "w").write("\n".join(lines) + "\n")
else: print("\n".join(l for l in lines if " DIFF " in l))
if rec:
    if not out_f: sys.exit("--record needs an out table")
    sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")); import shadow_cache
    print("recorded:", shadow_cache.record(out_f), "entries in output/shadow_cache.csv")
