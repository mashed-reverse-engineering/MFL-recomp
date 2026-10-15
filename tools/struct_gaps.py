#!/usr/bin/env python3
"""struct_gaps.py <failed.txt> [header=src/auto/ghidra_types.h]: structs of the generated header end in (or contain) `char _gap_XX[N];` for the bytes Ghidra has no fields for.  The decompiled code names such
a field `field_0xOFF`; cl then says "'field_0xOFF' : is not a member of 'S'" and the whole chunk (30 functions) is lost.  Splits the gap around OFF into
`_gap_XX[OFF-XX]; undefined4 field_0xOFF; _gap_(OFF+4)[...]` (the size of the struct does not change) for every such error in failed.txt, and writes the header in place.  Usage: struct_gaps.py failed.txt"""
import os, re, sys
HERE = os.path.dirname(os.path.abspath(__file__))
failed = sys.argv[1]
hdr = sys.argv[2] if len(sys.argv) > 2 else os.path.join(HERE, "..", "src", "auto", "ghidra_types.h")
need = {}
for l in open(failed, errors="replace"):
    for m in re.finditer(r"'field_0x([0-9a-f]+)' : is not a member of '(\w+)'", l):
        need.setdefault(m.group(2), set()).add(int(m.group(1), 16))
text = open(hdr, errors="replace").read()
done = 0
for st, offs in sorted(need.items()):
    m = re.search(r"(struct %s \{\n)(.*?)(\n\};)" % re.escape(st), text, re.S)
    if not m: print("struct", st, "not found"); continue
    body = m.group(2)
    for off in sorted(offs):
        g = None
        for gm in re.finditer(r"^(\s*)char _gap_([0-9a-f]+)\[(\d+)\];", body, re.M):
            x, n = int(gm.group(2), 16), int(gm.group(3))
            if x <= off < x + n and off + 4 <= x + n: g = (gm, x, n); break
        if not g: print("no gap for %s +0x%x" % (st, off)); continue
        gm, x, n = g
        new = []
        if off > x: new.append("%schar _gap_%x[%d];" % (gm.group(1), x, off - x))
        new.append("%sundefined4 field_0x%x;" % (gm.group(1), off))
        if x + n > off + 4: new.append("%schar _gap_%x[%d];" % (gm.group(1), off + 4, x + n - off - 4))
        body = body[:gm.start()] + "\n".join(new) + body[gm.end():]; done += 1
    text = text[:m.start(2)] + body + text[m.end(2):]
open(hdr, "w").write(text)
print(done, "gap splits in", len(need), "structs")
