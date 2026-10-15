#!/usr/bin/env python3
"""metrics.py [file]: readability numbers of the Ghidra export (default output/named/MFL_app_named.c); gate for every Ghidra apply."""
import re, sys
t = open(sys.argv[1] if len(sys.argv) > 1 else "output/named/MFL_app_named.c", errors="replace").read()
M = [("param_N", r"\bparam_\d+"), ("xVarN", r"\b[a-z]{1,3}Var\d+"), ("undefined", r"\bundefined\d?\b"), ("field_0xNN", r"\bfield_0x[0-9a-f]+"),
     ("raw ptr casts", r"\*\(\w+ ?\*\)\((?:\w+|\(int\)\w+) ?\+ ?(?:0x)?[0-9a-f]+\)"), ("reg_eXX", r"\breg_e\w+"), ("FUN_ calls", r"\bFUN_[0-9a-f]+\("), ("g_ auto", r"\bg_[0-9a-f]{6}\b|\bDAT_[0-9a-f]+")]
for n, r in M: print("%-14s %6d" % (n, len(re.findall(r, t))))
