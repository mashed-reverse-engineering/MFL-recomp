#!/usr/bin/env python3
"""mk_importlibs.py <out dir>: import libraries for every DLL the original exe imports (the Platform SDK libs user32.lib / gdi32.lib ... are not available here, only kernel32.lib of the VC7 CRT set).
Reads the import table of MFL.exe (objdump -p), takes the stdcall argument bytes N of every import from names/MFL_import_protos.txt (tools/import_protos.py: pushes in front of `call [IAT]`, 0 when unknown), writes
<dll>.def with `Name = _Name@N` entries and builds <dll>.lib with the VC7 lib.exe (vc7.sh).  Symbols defined: _Name@N and __imp__Name@N (the import by the undecorated name Name)."""
import os, re, struct, subprocess, sys
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
if os.environ.get("OBJLIST"):      # our objects (tools/delink/link_ours.sh objs_local.txt: "src obj" per line) name the decorated imports they use: authoritative over the push count
    for l in open(os.environ["OBJLIST"]):
        if l.strip():
            for m in re.finditer(r"__imp__([A-Za-z0-9_]+)@(\d+)", subprocess.run(["objdump", "-t", l.split()[-1]], capture_output=True, text=True).stdout): nargs[m.group(1)] = int(m.group(2)) // 4
txt = subprocess.run(["objdump", "-p", EXE], capture_output=True, text=True).stdout
dll, imports = None, {}
for l in txt.splitlines():
    m = re.match(r"\s*DLL Name: (\S+)", l)
    if m: dll = m.group(1); imports[dll] = []; continue
    m = re.match(r"\s+[0-9a-f]+\s+(\d+)\s+(\w+)\s*$", l)       # hint  name
    if m and dll: imports[dll].append(m.group(2)); continue
    m = re.match(r"\s+[0-9a-f]+\s+(\d+)\s+<none>\s*$", l)       # import by ordinal: symbol __imp__Ordinal_<n>, `Ordinal_<n> @<n> NONAME` in the def
    if m and dll: imports[dll].append("Ordinal_" + m.group(1))
extra = os.environ.get("EXTRA_USER32", "")      # imports used by the SDK libs but not by the game exe (comma list): added to user32
if extra: imports.setdefault("USER32.dll", []).extend(extra.split(","))
def write_implib(path, dll, names, nargs):
    """The VC7 lib.exe /def makes imports by the DECORATED name (`GetDlgItem@8` in the DLL's name table), which no DLL exports.  So the archive is written here:
    one short import object (IMPORT_OBJECT_HEADER) per name, symbol `_Name@N`, name type UNDECORATE (the loader imports `Name`); an ordinal import is a DATA
    object (only `__imp__Ordinal_<n>`) with name type ORDINAL.  First and second linker member as lib.exe writes them."""
    mem = []      # (symbols, bytes)
    old = open(path, "rb").read()      # the lib.exe made library of the same def: its three descriptor members (__IMPORT_DESCRIPTOR_x, __NULL_IMPORT_DESCRIPTOR, x_NULL_THUNK_DATA) are real COFF objects
    nsym = struct.unpack_from(">I", old, 8 + 60)[0]; offs0 = struct.unpack_from(">%dI" % nsym, old, 8 + 60 + 4)
    names0 = old[8 + 60 + 4 + 4 * nsym:old.index(b"\0\0", 8 + 60 + 4 + 4 * nsym) + 1].split(b"\0")
    bysym = {}
    for o_, n_ in zip(offs0, names0): bysym.setdefault(o_, []).append(n_.decode("latin1"))
    for o_, ss in sorted(bysym.items()):
        sz = int(old[o_ + 48:o_ + 58]); body = old[o_ + 60:o_ + 60 + sz]
        if body[:4] != b"\0\0\xff\xff": mem.append(([x for x in ss if not x.startswith("__imp_")] if False else ss, body))
    for n in names:
        if n.startswith("Ordinal_"):
            sym = "_" + n; hdr = struct.pack("<HHHHIIHH", 0, 0xFFFF, 0, 0x14C, 0, len(sym) + len(dll) + 2, int(n[8:]), 1 | (0 << 2))      # DATA, ordinal
            syms = ["__imp_" + sym]
        else:
            sym = "_%s@%d" % (n, 4 * nargs.get(n, 0)); hdr = struct.pack("<HHHHIIHH", 0, 0xFFFF, 0, 0x14C, 0, len(sym) + len(dll) + 2, 0, 0 | (3 << 2))      # CODE, undecorate
            syms = ["__imp_" + sym, sym]
        mem.append((syms, hdr + sym.encode() + b"\0" + dll.encode() + b"\0"))
    def mh(name, data): return name.ljust(16).encode() + b"0".ljust(12) + b"0".ljust(6) + b"0".ljust(6) + b"0".ljust(8) + str(len(data)).ljust(10).encode() + b"`\n"
    allsyms = [(x, i) for i, (ss, _) in enumerate(mem) for x in ss]
    def size(nm): return (60 + nm + 1) & ~1
    strs1 = b"".join(x.encode() + b"\0" for x, _ in allsyms)
    l1 = 4 + 4 * len(allsyms) + len(strs1); l1 = (60 + l1 + 1) & ~1
    srt = sorted(allsyms)
    strs2 = b"".join(x.encode() + b"\0" for x, _ in srt)
    l2 = 4 + 4 * len(mem) + 4 + 2 * len(srt) + len(strs2); l2 = (60 + l2 + 1) & ~1
    off = 8 + l1 + l2; offs = []
    for ss, b in mem: offs.append(off); off += (60 + len(b) + 1) & ~1
    d1 = struct.pack(">I", len(allsyms)) + b"".join(struct.pack(">I", offs[i]) for _, i in allsyms) + strs1
    d2 = struct.pack("<I", len(mem)) + b"".join(struct.pack("<I", o) for o in offs) + struct.pack("<I", len(srt)) + b"".join(struct.pack("<H", i + 1) for _, i in srt) + strs2
    out_ = bytearray(b"!<arch>\n")
    for nm, dt in (("/", d1), ("/", d2)): out_ += mh(nm, dt) + dt + (b"\n" if len(dt) % 2 else b"")
    for (ss, b) in mem: out_ += mh(dll + "/", b) + b + (b"\n" if len(b) % 2 else b"")
    open(path + ".new", "wb").write(out_); os.replace(path + ".new", path)      # lib.exe ran as root: replace the file, do not write into it
for d, names in imports.items():
    base = os.path.splitext(d)[0].lower()
    with open(os.path.join(out, base + ".def"), "w") as f:
        f.write("LIBRARY %s\nEXPORTS\n" % d)
        for n in sorted(set(names)): f.write(("%s @%s NONAME\n" % (n, n[8:])) if n.startswith("Ordinal_") else ("%s@%d\n" % (n, 4 * nargs.get(n, 0))))      # decorated entry: the import is by the undecorated name (NAME_UNDECORATE)
    subprocess.run([os.path.join(HERE, "vc7.sh"), out, "lib.exe", "/nologo", "/machine:ix86", "/def:%s.def" % base, "/out:%s.lib" % base], capture_output=True, text=True, env=dict(os.environ, VC7_CONTAINER=os.environ.get("VC7_CONTAINER", "ac")))
    write_implib(os.path.join(out, base + ".lib"), d, sorted(set(names)), nargs)
    print("%-14s %4d imports  %s.lib" % (d, len(set(names)), base))
