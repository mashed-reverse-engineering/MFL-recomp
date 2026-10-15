#!/usr/bin/env python3
"""struct_named.py <failed.txt>... [--dry]: `'member' : is not a member of 'S'` in autocompile failures, where the member is a NAMED field of names/MFL_structs.txt (`field,OFF,type,name`) that the
exported header (src/auto/ghidra_types.h, from before the field was named in Ghidra) lacks.  Puts the field into struct S at OFF without changing the layout:
  - OFF inside a `char _gap_X[N];` pad  -> the pad is split around the new field
  - OFF is an existing field of the same size (`undefined4 field_0xOFF;`) -> `union { <old field>; <type> name; };` (nameless union, old code keeps compiling)
Types: u32 uint, i32 int, f32 float, u8 byte, T[n] arrays, other names as is.  Misaligned or not fitting fields are reported and skipped.  Also fixes RwGlobalsRelease (`--release`, always): the SDK's
stringFuncs / memoryFuncs sit at +0xC4 / +0x108 (the release build has no debug fields in front), replacing `field_0xcc` / `field_0x108` / `field_0x10c` by unions over the real members.
All copies of ghidra_types.h under src/ are rewritten identically.  The caller must check sizeof / offsetof: the script writes <header dir>/../../output/struct_named_check.c (cl /Zs) with
one assertion per patched field.  Usage: struct_named.py failed.txt [...] [--dry]"""
import glob, os, re, sys
G = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
P = lambda *a: os.path.join(G, *a)
DRY = "--dry" in sys.argv
heads = sorted(glob.glob(P("src", "*", "ghidra_types.h")))
text = open(P("src", "auto", "ghidra_types.h"), errors="replace").read()      # src/auto is the master (tools/struct_gaps.py writes only that one); every copy is overwritten with the result
TY = {"u32": ("uint", 4), "i32": ("int", 4), "f32": ("float", 4), "u8": ("byte", 1), "u16": ("ushort", 2), "i16": ("short", 2)}
spec, cur = {}, None
for l in open(P("names", "MFL_structs.txt")):
    p = [x.strip() for x in l.split(",", 4)]
    if p[0] == "struct": cur = p[1]; spec[cur] = {}
    elif p[0] == "field" and cur and len(p) >= 4: spec[cur][p[3]] = (int(p[1], 16), p[2])
need = {}
for f in sys.argv[1:]:
    if f.startswith("--"): continue
    for l in open(f, errors="replace"):
        for m in re.finditer(r"'(\w+)' : is not a member of '(\w+)'", l):
            if not m.group(1).startswith("field_0x"): need.setdefault(m.group(2), set()).add(m.group(1))
checks, done, skipped = [], [], []
def csize(t):
    m = re.match(r"(\w+)\[(\d+)\]$", t)
    base, n = (m.group(1), int(m.group(2))) if m else (t, 1)
    if base in TY: return TY[base][1] * n
    m2 = re.search(r"struct %s \{" % re.escape(base), text)
    return None
def merge_run(body, off, tot):
    """consecutive pad / plain 4 byte members that start at OFF and end exactly at OFF+TOT: (start, end, lines) in BODY, or None (offsets are tracked from the member sizes, every member must be simple)"""
    pos, cur, lines, a_ = None, 0, [], None
    for lm in re.finditer(r"^(.*)$", body, re.M):
        l = lm.group(1)
        if not l.strip(): continue
        g_ = re.match(r"\s*char _gap_([0-9a-f]+)\[(\d+)\];", l)
        sc = re.match(r"\s*(?:undefined4|uint|int|float|undefined4 \*|void \*) \*?\w+;", l)
        if g_: start, size = int(g_.group(1), 16), int(g_.group(2))
        elif sc:
            fm = re.search(r"field_0x([0-9a-f]+)", l)
            if not fm: cur = None; lines, a_ = [], None; continue
            start, size = int(fm.group(1), 16), 4
        else:
            lines, a_ = [], None; continue
        if a_ is None and start == off: a_ = lm.start(); lines = []
        if a_ is not None:
            lines.append(l)
            if start + size == off + tot: return a_, lm.end(), lines
            if start + size > off + tot: return None
    return None

for st, names in sorted(need.items()):
    m = re.search(r"(struct %s \{\n)(.*?)(\n\};)" % re.escape(st), text, re.S)
    if st not in spec or not m: skipped.append((st, "no spec / struct")); continue
    body = m.group(2)
    for nm in sorted(names):
        if nm not in spec[st]: skipped.append((st + "." + nm, "not in MFL_structs.txt")); continue
        off, t = spec[st][nm]
        mm = re.match(r"(\w+)\[(\d+)\]$", t)
        base, cnt = (mm.group(1), int(mm.group(2))) if mm else (t, None)
        if base not in TY: skipped.append((st + "." + nm, "type " + t)); continue
        ct, sz = TY[base]; tot = sz * (cnt or 1)
        if off % min(sz, 4): skipped.append((st + "." + nm, "misaligned")); continue
        decl = "%s %s%s;" % (ct, nm, "[%d]" % cnt if cnt else "")
        g = None
        for gm in re.finditer(r"^(\s*)char _gap_([0-9a-f]+)\[(\d+)\];", body, re.M):
            x, n = int(gm.group(2), 16), int(gm.group(3))
            if x <= off and off + tot <= x + n: g = (gm, x, n); break
        if g:
            gm, x, n = g; ind = gm.group(1); new = []
            if off > x: new.append("%schar _gap_%x[%d];" % (ind, x, off - x))
            new.append(ind + decl)
            if x + n > off + tot: new.append("%schar _gap_%x[%d];" % (ind, off + tot, x + n - off - tot))
            body = body[:gm.start()] + "\n".join(new) + body[gm.end():]
        else:
            run = merge_run(body, off, tot)
            if run:
                a_, b_, lines_ = run
                ind = re.match(r"\s*", lines_[0]).group(0)
                body = body[:a_] + "%sunion { struct {\n%s\n%s}; %s };" % (ind, "\n".join("    " + x.strip().join([ind, ""]) for x in lines_), ind, decl) + body[b_:]
                done.append("%s.%s +0x%x %s (union over %d old members)" % (st, nm, off, t, len(lines_))); checks.append("typedef char chk_%s_%s[(int)&((struct %s *)0)->%s == 0x%x ? 1 : -1];" % (st, nm, st, nm, off)); continue
            fm = re.search(r"^(\s*)(undefined4|uint|int|float) (field_0x%x|field_%x);.*$" % (off, off), body, re.M)
            if fm and tot == 4:
                body = body[:fm.start()] + "%sunion { %s %s; %s };" % (fm.group(1), fm.group(2), fm.group(3), decl) + body[fm.end():]
            else: skipped.append((st + "." + nm, "no pad / same size field at +0x%x" % off)); continue
        done.append("%s.%s +0x%x %s" % (st, nm, off, t)); checks.append("typedef char chk_%s_%s[(int)&((struct %s *)0)->%s == 0x%x ? 1 : -1];" % (st, nm, st, nm, off))
    text = text[:m.start(2)] + body + text[m.end(2):]
# RwGlobalsRelease: the real SDK members behind the numbered fields
rel = re.search(r"(struct RwGlobalsRelease \{\n)(.*?)(\n\};)", text, re.S)
if rel and "memoryFuncs" not in rel.group(2):
    b = rel.group(2)
    b = b.replace("    char _gap_48[132];\n    undefined4 field_0xcc;\n    char _gap_d0[56];\n    undefined4 field_0x108;\n    undefined4 field_0x10c;\n    char _gap_110[12];\n",
        "    RwStandardFunc stdFunc[29]; /* +0x48 */\n    struct RwLinkList dirtyFrameList; /* +0xBC */\n"
        "    union { struct RwStringFunctions stringFuncs; /* +0xC4 */ struct { char _gap_c4[8]; undefined4 field_0xcc; }; };\n"
        "    union { struct RwMemoryFunctions memoryFuncs; /* +0x108: rwmalloc, rwfree, rwrealloc, rwcalloc */ struct { undefined4 field_0x108; undefined4 field_0x10c; }; };\n"
        "    char _gap_118[4];\n")
    if b != rel.group(2):
        text = text[:rel.start(2)] + b + text[rel.end(2):]
        checks += ["typedef char chk_rel_str[(int)&((struct RwGlobalsRelease *)0)->stringFuncs == 0xc4 ? 1 : -1];", "typedef char chk_rel_mem[(int)&((struct RwGlobalsRelease *)0)->memoryFuncs == 0x108 ? 1 : -1];",
                   "typedef char chk_rel_f11c[(int)&((struct RwGlobalsRelease *)0)->field_0x11c == 0x11c ? 1 : -1];", "typedef char chk_rel_size[sizeof(struct RwGlobalsRelease) == 0x200 ? 1 : -1];"]
        done.append("RwGlobalsRelease.stringFuncs/memoryFuncs")
for d in done: print("added", d)
for s_, why in skipped: print("skipped", s_, why)
if DRY or not done: sys.exit()
for h in heads: open(h, "w").write(text)
open(P("output", "struct_named_check.c"), "w").write('#include "prelude.h"\n#include "ghidra_types.h"\n' + "\n".join(checks) + "\n")
print("wrote %d headers; check: cl /Zs output/struct_named_check.c in src/auto (tools/vc7.sh)" % len(heads))
