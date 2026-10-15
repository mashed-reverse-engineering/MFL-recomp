#!/usr/bin/env python3
"""Make the header written by ghidra/scripts/ExportTypes.java compile with cl 13.00.9466 (C mode).
Usage: fix_ghidra_types.py <in.h> <out.h>"""
import re, sys

s = open(sys.argv[1], encoding="utf-8", errors="replace").read()
s = s.replace("long long", "__int64")
s = re.sub(r"#define _MSC_VER[^\n]*\n", "", s)
s = s.replace("pointer32", "unsigned int")
s = re.sub(r"(struct \w+ \{[^\n]*\n)\};", lambda m: m.group(1) + "    char _empty;\n};", s)  # empty struct
s = re.sub(r"\[0\];", "[1];", s)                               # zero sized arrays
# runs of single byte "undefined fieldN_0xH;" members (gaps Ghidra spelled out byte by byte) -> one char array
out, run = [], []
def flush():
    global run
    if run:
        out.append("    char _gap_%s[%d];" % (run[0], len(run)))
        run = []
for l in s.split("\n"):
    m = re.match(r"\s+undefined field\d+_0x([0-9a-f]+);\s*$", l)
    if m:
        run.append(m.group(1))
    else:
        flush()
        out.append(l)
flush()
s = "\n".join(out)
open(sys.argv[2], "w").write(s)
