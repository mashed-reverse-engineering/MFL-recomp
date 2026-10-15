#!/usr/bin/env python3
"""output/regsigs.csv: name,addr,regs,ret_n,stack parameter types (;-separated) for every function with register inputs.
The register inputs come from the disassembly (output/reginputs.csv), the stack parameters from the decompilation made BEFORE scripts/SetRegisterParams.java
(argument: that file; the decompiler had inferred the stack parameters dynamically, as param_N; the first one (thiscall) or two (fastcall) parameters of
such functions are the ECX / EDX registers and are not stack parameters).  Usage: regsigs.py <old named decompilation> <out csv>"""
import csv, re, sys
old = open(sys.argv[1], errors="replace").read()
blocks = {m.group(1): (m.group(2), m.group(3)) for m in re.finditer(r"// ==== (\w+) @ ([0-9a-f]{8})\n(.*?)(?=\n// ==== |\Z)", old, re.S)}
rows = []
for r in csv.DictReader(open("output/reginputs.csv")):
    if not r["reg_inputs"].strip("|"): continue
    b = blocks.get(r["name"])
    types = []
    if b:
        head = re.sub(r"/\*.*?\*/", " ", b[1].split("\n{", 1)[0], flags=re.S)
        head = " ".join(head.split())
        conv = re.search(r"__(thiscall|fastcall)", head)
        m = re.match(r".*?\b\w+\((.*)\)$", head)
        ps = [p.strip() for p in re.split(r",(?![^()]*\))", m.group(1))] if m and m.group(1).strip() not in ("", "void") else []
        skip = 2 if conv and conv.group(1) == "fastcall" else (1 if conv else 0)
        for p in ps[skip:]:
            mm = re.match(r"(.*?)\s*\b\w+((?:\[\d*\])*)$", p)
            types.append((mm.group(1).strip() if mm else "undefined4"))
    rows.append((r["name"], r["addr"], r["reg_inputs"], r["ret_n"], ";".join(types)))
with open(sys.argv[2], "w") as f:
    f.write("name,addr,regs,ret_n,stack\n")
    for x in rows: f.write(",".join(x) + "\n")
print(len(rows), "functions,", sum(1 for x in rows if x[4]), "with stack parameters")
