#!/usr/bin/env python3
"""spec_header.py IN.h OUT.h: bring a ghidra_types.h up to date with the struct specs names/MFL_structs*.txt WITHOUT Ghidra (the header is an ExportTypes.java export of a project state that predates later spec fields).
 - spec struct missing in the header          -> appended (fields sorted by offset, `char _gap_X[N]` between them, padded to the spec size; dependencies first)
 - spec field (by name) missing in the header -> put into the `char _gap_X[N]` / `undefined4 field_0xX;` lines that cover its bytes (the struct size does not change; a field whose bytes are
   not entirely free lines, e.g. a different field sits there, is skipped and listed)
Idempotent.  Prints what was added / skipped.  Usage: spec_header.py src/auto/ghidra_types.h /tmp/new_ghidra_types.h   (then `cmp`, and sync the copies: find recomp -name ghidra_types.h)"""
import glob, os, re, sys
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
BASE = {"u8": ("uchar", 1), "u16": ("ushort", 2), "i16": ("short", 2), "u32": ("uint", 4), "i32": ("int", 4), "f32": ("float", 4), "char": ("char", 1)}
spec = {}      # name -> [size, [(off, type, name, comment)]]
for f in sorted(glob.glob(os.path.join(G, "names", "MFL_structs*.txt"))):
    cur = None
    for l in open(f):
        l = l.strip()
        if not l or l.startswith("#"): continue
        p = l.split(",", 4)
        if p[0] == "struct": cur = spec.setdefault(p[1], [0, []]); cur[0] = int(p[2], 0)
        elif p[0] == "field" and cur is not None and len(p) >= 4: cur[1].append((int(p[1], 0), p[2], p[3], (p[4] if len(p) > 4 else "").strip().strip('"')))
def size_of(t):
    m = re.fullmatch(r"(\w+?)(?:\[(\d+)\])?", t); b, n = m.group(1), int(m.group(2) or 1)
    if b in BASE: return BASE[b][1] * n
    return spec[b][0] * n if b in spec else None
def decl(t, name):
    m = re.fullmatch(r"(\w+?)(?:\[(\d+)\])?", t); b, n = m.group(1), m.group(2)
    return "%s %s%s;" % (BASE[b][0] if b in BASE else "struct " + b, name, "[%s]" % n if n else "")
def cmt(c): return " /* %s */" % c.replace("*/", "* /") if c else ""
def free(line):
    """-> (offset, size) for a gap / placeholder line"""
    m = re.match(r"\s*char _gap_([0-9a-f]+)\[(\d+)\];", line)
    if m: return int(m.group(1), 16), int(m.group(2))
    m = re.match(r"\s*undefined(\d?) field_0x([0-9a-f]+);", line)
    if m: return int(m.group(2), 16), int(m.group(1) or 1)
def gap(ind, off, n): return ["%schar _gap_%x[%d];" % (ind, off, n)] if n > 0 else []
text = open(sys.argv[1], errors="replace").read(); log = []
def patch(st):
    global text
    m = re.search(r"(struct %s \{\n)(.*?)(\n\};)" % re.escape(st), text, re.S)
    if not m: return
    lines = m.group(2).split("\n")
    for off, t, name, c in sorted(spec[st][1]):
        if re.search(r"\b%s\b(?:\[\d+\])?;" % re.escape(name), "\n".join(lines)): continue
        sz = size_of(t)
        if not sz: log.append("skip %s.%s: unknown size of %s" % (st, name, t)); continue
        i = next((k for k, l in enumerate(lines) if free(l) and free(l)[0] <= off < free(l)[0] + free(l)[1]), None)
        if i is None: log.append("skip %s.%s +0x%x: not free" % (st, name, off)); continue
        j, end = i, free(lines[i])[0]
        while j < len(lines) and free(lines[j]) and free(lines[j])[0] == end and end < off + sz: end += free(lines[j])[1]; j += 1
        if end < off + sz: log.append("skip %s.%s +0x%x..0x%x: bytes not free" % (st, name, off, off + sz)); continue
        start = free(lines[i])[0]; ind = re.match(r"\s*", lines[i]).group(0)
        fm = re.match(r"\s*undefined(\d?) field_0x([0-9a-f]+);", lines[i])
        if fm and j == i + 1 and start == off and (int(fm.group(1) or 1)) == sz:      # keep the old placeholder name usable by existing sources: anonymous union {named; field_0xNN;}
            lines[i:j] = [ind + "union { " + decl(t, name) + " undefined%s field_0x%s; };" % (fm.group(1), fm.group(2)) + cmt(c)]
        else:
            lines[i:j] = gap(ind, start, off - start) + [ind + decl(t, name)[:-1] + ";" + cmt(c)] + gap(ind, off + sz, end - off - sz)
        log.append("add %s.%s +0x%x" % (st, name, off))
    text = text[:m.start(2)] + "\n".join(lines) + text[m.end(2):]
def emit(st):
    size, fl = spec[st]; out = ["struct %s {" % st]; pos = 0
    for off, t, name, c in sorted(fl):
        sz = size_of(t)
        if not sz or off < pos: log.append("skip %s.%s: overlap / unknown size" % (st, name)); continue
        out += gap("    ", pos, off - pos) + ["    " + decl(t, name) + cmt(c)]; pos = off + sz
    return "\n".join(out + gap("    ", pos, size - pos) + ["};"])
added, new = set(), []
def need(st):
    if st in added: return
    added.add(st)
    for _, t, _, _ in spec[st][1]:
        b = t.split("[")[0]
        if b in spec and not re.search(r"struct %s \{" % b, text): need(b)
    new.append(st)
for st in spec:
    if re.search(r"struct %s \{" % re.escape(st), text): patch(st)
    else: need(st)
if new:
    text = text.rstrip("\n") + "\n\n/* structs of names/MFL_structs*.txt that the exported header lacks (tools/spec_header.py) */\n" + "".join("typedef struct %s %s;\n" % (s, s) for s in new) + "\n" + "\n\n".join(emit(s) for s in new) + "\n"
    log += ["new struct " + s for s in new]
open(sys.argv[2], "w").write(text)
print("\n".join(log) or "nothing to do")
