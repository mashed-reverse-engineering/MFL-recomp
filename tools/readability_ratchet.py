#!/usr/bin/env python3
"""readability_ratchet.py [--update-baseline] [--quiet] [--selftest] [--files f ...]: readability can only improve.  (--files, last option: also print the counts of these files)
Counts Ghidra-default identifier tokens (uVar/iVar/param_/DAT_/FUN_ ...) plus generic rename-dodges (v1, a2, tmp3, local_8 ...)
in code (comments and string/char literals masked), per tier directory (portcheck scope: tier1, automatch_agent, automatch_near, hand src/*.c).
Baseline output/readability_baseline.txt (`tier class count` rows); ONLY --update-baseline writes it (M only).
Exit 1 when a tier/class count grows past the baseline (binding in check_all and preflight since W7 T7; W5/W6: report-only), 0 otherwise.
The generated bind tables src/automatch_near/an_hk_612_*.inc are not counted (EXEMPT).
Counting every token occurrence (not just definitions) is deliberate: a trivial rename to v1/a1 lands in class `generic` and still counts."""
import glob, os, re, sys
H = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(H, "..")); sys.path.insert(0, H)
import rawaddr
BASE = os.path.join(G, "output", "readability_baseline.txt")
CLASSES = {
 "ghidra_var": re.compile(r"\b(?:[uicbfdsl]{1,2}Var|p[A-Za-z]{0,3}Var|[A-Za-z]{0,3}Stack_|param_|local_|in_|unaff_|extraout_)\w*\b"),      # uVar1 iVar2 pcVar3 ppuVar4 uStack_14 auStack_20 param_1 local_14 in_EAX unaff_EBX extraout_EAX
 "ghidra_sym": re.compile(r"\b(?:DAT|FUN|PTR|LAB|SUB|UNK|s|u)_[0-9a-fA-F]{6,8}\b|\b(?:DAT|FUN|PTR)_\w+|\b(?:CONCAT|ZEXT|SEXT)\d+\b|\bSUB\d\d\b"),
 # v1 a2 tmp4 b1 n1 x_1 _v1 V1 uvar1 local1: renames that dodge the Ghidra names (D1 audit 2026-11-10 widened it); u8/s16/f32 style type names excluded
 "generic": re.compile(r"(?<![\w])(?!local_|[su]_[0-9a-fA-F]{6,8}\b)_?(?![usif](?:8|16|32|64)\b)(?:[A-Za-z]|tmp|var|arg|loc|temp|val|local|[a-z]{1,3}var)_?\d+_?(?!\w)"),
}
TIERS = {"tier1": "tier1/*.c", "automatch_agent": "automatch_agent/*.c", "automatch_near": "automatch_near/*.c", "hand": "*.c"}

def count(text):
    m = rawaddr.mask(text); return {k: len(rx.findall(m)) for k, rx in CLASSES.items()}

def other_files():
    """every .c/.h/.inc under src/ not in a named tier; *_shadow dirs (lane WIP) skipped"""
    R = os.path.join(G, "src"); named = set()
    for g in TIERS.values(): named.update(glob.glob(os.path.join(R, g)))
    res = []
    for d, ds, fs in os.walk(R):
        ds[:] = sorted(x for x in ds if not x.endswith("_shadow"))
        res += [os.path.join(d, f) for f in sorted(fs) if f.endswith((".c", ".h", ".inc"))]
    return [p for p in res if p not in named and os.path.isfile(p) and not EXEMPT(p)]

# W7 T7: the mflrw.dll bind tables an_hk_612_*.inc are generated lists of exe addresses (FUN_ / LAB_ names of callees and callback labels the
# original passes by address): not readable code to improve, so they are not counted
EXEMPT = lambda p: re.match(r"an_hk_612_\w+\.inc$", os.path.basename(p)) is not None

def scan():
    out = {}
    for t, g in TIERS.items():
        out[t] = [p for p in glob.glob(os.path.join(G, "src", g)) if os.path.isfile(p)]      # skips dirs/broken links named *.c
    out["other"] = other_files()
    res = {}
    for t, fl in out.items():
        c = {k: 0 for k in CLASSES}
        for f in sorted(fl):
            for k, v in count(open(f, errors="surrogateescape").read()).items(): c[k] += v
        res[t] = c
    return res

def selftest():
    s = 'int f(int param_1){ int iVar1; /* uVar9 FUN_00401000 */ char *s = "iVar5 v1"; iVar1 = DAT_004a2790 + local_14; return v2 + a1; }'
    c = count(s)
    assert c == {"ghidra_var": 4,"ghidra_sym": 1, "generic": 2}, c       # masked comment/string not counted; v2 a1 counted
    assert count("int speed; int wheel_count; int x0y;") == {"ghidra_var": 0, "ghidra_sym": 0, "generic": 0}
    assert count("0x1a1 + v10a + u8 + s16 + D3D9 + rwCOMBINEPOSTCONCAT") == {"ghidra_var": 0, "ghidra_sym": 0, "generic": 0}      # hex literal, type names and longer identifiers are not tokens
    assert count("b1 n2 x_3 _v4 V5 uvar6 local7 ivar_8 v9_ val10") == {"ghidra_var": 0, "ghidra_sym": 0, "generic": 10}   # rename dodges
    assert count("local_14 s_00401000 u_0040a0b0 param_1") == {"ghidra_var": 2, "ghidra_sym": 2, "generic": 0}       # no double count across classes
    assert count("ppuVar1 uStack_14 auStack_20 CONCAT44(a, b) SUB41(c, 0)") == {"ghidra_var": 3, "ghidra_sym": 2, "generic": 0}
    names = [os.path.relpath(p, os.path.join(G, "src")) for p in other_files()]
    assert names and not any(n.endswith("_shadow") or "_shadow" + os.sep in n for n in names), "shadow dirs skipped"
    assert not any(os.sep not in n and n.endswith(".c") for n in names), "top-level .c belongs to hand"
    assert not any(n.startswith(("tier1" + os.sep, "automatch_agent" + os.sep, "automatch_near" + os.sep)) and n.endswith(".c") for n in names)
    assert any(n.endswith((".h", ".inc")) for n in names)
    assert not any(os.path.basename(n).startswith("an_hk_612_") and n.endswith(".inc") for n in names), "bind tables exempt (T7)"
    assert EXEMPT("x/an_hk_612_matfx.inc") and not EXEMPT("x/an_hk_612.c") and not EXEMPT("x/an_hk_1367.c") and not EXEMPT("x/other.inc")
    print("readability_ratchet selftest ok"); return 0

def main(argv):
    if "--selftest" in argv: return selftest()
    if "--files" in argv:      # run.py preflight: token counts of the given files (a lane's WIP), then the tree verdict as usual
        i = argv.index("--files")
        for f in argv[i + 1:]: print("  %s  %s" % (" ".join("%s=%d" % kv for kv in count(open(f, errors="surrogateescape").read()).items()), f))
        argv = argv[:i]
    cur = scan(); tot = sum(sum(c.values()) for c in cur.values())
    line = "readability: %d tokens (%s)" % (tot, ", ".join("%s %d" % (t, sum(c.values())) for t, c in cur.items()))
    if "--update-baseline" in argv:
        with open(BASE, "w") as o:
            o.write("# tools/readability_ratchet.py baseline: a count above this is reported (WARNING in W5); only `--update-baseline` rewrites it\n# %s\n" % line)
            for t, c in cur.items():
                for k, v in c.items(): o.write("%s %s %d\n" % (t, k, v))
        print("baseline written"); return 0
    if "--quiet" not in argv:
        for t, c in cur.items(): print("  %-16s %s" % (t, " ".join("%s=%d" % kv for kv in c.items())))
    if not os.path.exists(BASE): print(line + "; no baseline (M runs --update-baseline at the freeze)"); return 0
    base = {}
    for l in open(BASE):
        p = l.split()
        if len(p) == 3 and not l.startswith("#"): base[(p[0], p[1])] = int(p[2])
    grew = ["%s/%s %d > %d" % (t, k, v, base.get((t, k), 0)) for t, c in cur.items() for k, v in c.items() if v > base.get((t, k), 0)]
    shrank = ["%s/%s %d < %d" % (t, k, v, base[(t, k)]) for t, c in cur.items() for k, v in c.items() if v < base.get((t, k), 0)]
    print(line + "; " + ("GREW: " + ", ".join(grew) if grew else "within baseline"))
    if shrank: print("SHRANK " + ", ".join(shrank) + " (re-baseline; informational)")      # freed room would hide later growth
    return 1 if grew else 0
if __name__ == "__main__": sys.exit(main(sys.argv[1:]))
