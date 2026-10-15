#!/usr/bin/env python3
"""mk_importlibs.py <out dir>: import libraries for every DLL the original exe imports (the Platform SDK libs user32.lib / gdi32.lib ... are not available here, only kernel32.lib of the VC7 CRT set).
Reads the import table of MFL.exe (objdump -p), takes the stdcall argument bytes N of every import from names/MFL_import_protos.txt (tools/import_protos.py: pushes in front of `call [IAT]`, 0 when unknown), writes
<dll>.def with `Name = _Name@N` entries and builds <dll>.lib with the VC7 lib.exe (vc7.sh).  Symbols defined: _Name@N and __imp__Name@N (the import by the undecorated name Name)."""
import os, re, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__))
EXE = os.path.join(HERE, "..", "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")
out = os.path.abspath(sys.argv[1]); os.makedirs(out, exist_ok=True)
nargs = {}
for l in open(os.path.join(HERE, "..", "names", "MFL_import_protos.txt")):
    m = re.search(r"__stdcall (\w+)\((.*)\);", l)
    if m: nargs[m.group(1)] = 0 if m.group(2).strip() in ("", "void") else len(m.group(2).split(","))
import glob, re as _re
for lib in glob.glob(os.path.join(HERE, "..", "toolchain", "rwsdk", "lib", "d3d9", "release", "*.lib")):      # the SDK libs (cl 12) carry the exact decorated import names `__imp__Name@N` they use
    for m in _re.finditer(rb"__imp__([A-Za-z0-9_]+)@(\d+)", open(lib, "rb").read()): nargs[m.group(1).decode()] = int(m.group(2)) // 4      # authoritative (the real decorated name), overrides the push count heuristic
txt = subprocess.run(["objdump", "-p", EXE], capture_output=True, text=True).stdout
dll, imports = None, {}
for l in txt.splitlines():
    m = re.match(r"\s*DLL Name: (\S+)", l)
    if m: dll = m.group(1); imports[dll] = []; continue
    m = re.match(r"\s+[0-9a-f]+\s+(\d+)\s+(\w+)\s*$", l)       # hint  name
    if m and dll: imports[dll].append(m.group(2))
extra = os.environ.get("EXTRA_USER32", "")      # imports used by the SDK libs but not by the game exe (comma list): added to user32
if extra: imports.setdefault("USER32.dll", []).extend(extra.split(","))
for d, names in imports.items():
    base = os.path.splitext(d)[0].lower()
    with open(os.path.join(out, base + ".def"), "w") as f:
        f.write("LIBRARY %s\nEXPORTS\n" % d)
        for n in sorted(set(names)): f.write("%s@%d\n" % (n, 4 * nargs.get(n, 0)))      # decorated entry: the import is by the undecorated name (NAME_UNDECORATE)
    r = subprocess.run([os.path.join(HERE, "vc7.sh"), out, "lib.exe", "/nologo", "/machine:ix86", "/def:%s.def" % base, "/out:%s.lib" % base], capture_output=True, text=True, env=dict(os.environ, VC7_CONTAINER=os.environ.get("VC7_CONTAINER", "ac")))
    print("%-14s %4d imports  %s" % (d, len(set(names)), (r.stdout + r.stderr).strip().splitlines()[-1] if (r.stdout + r.stderr).strip() else ""))
