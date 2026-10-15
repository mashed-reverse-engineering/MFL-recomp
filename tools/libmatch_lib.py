#!/usr/bin/env python3
"""Find the functions of a static library (.lib, MS archive of COFF objects: the exact compiler's own CRT) in MFL.exe by bytes (relocation fields wildcarded).
Unlike libmatch.py it needs no source and also finds the hand written assembler functions of the CRT.
Usage: libmatch_lib.py <out.csv> <lib> [<lib> ...]
Output csv: VA(hex),function name,library:member,size[,note]; unique byte matches only, ambiguous ones get a note.  Names starting with '$' or '??' are skipped."""
import os, re, struct, sys, tempfile, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
src = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "libmatch.py")).read().replace("\nmain()\n", "\n")
ns = {"__file__": os.path.join(os.path.dirname(os.path.abspath(__file__)), "libmatch.py")}
exec(compile(src, "libmatch", "exec"), ns)
text_of_exe, obj_functions = ns["text_of_exe"], ns["obj_functions"]

def members(path):
    d = open(path, "rb").read()
    assert d[:8] == b"!<arch>\n"
    p, longnames = 8, b""
    while p + 60 <= len(d):
        name, size = d[p:p + 16].decode().strip(), int(d[p + 48:p + 58])
        body = d[p + 60:p + 60 + size]
        if name == "//": longnames = body
        elif name not in ("/", ""):
            if name.startswith("/") and name[1:].isdigit():
                o = int(name[1:]); name = re.split(rb"[\n\0]", longnames[o:o + 260])[0].decode().rstrip("/")
            yield name.rstrip("/"), body
        p += 60 + size + (size & 1)

def main():
    out, libs = sys.argv[1], sys.argv[2:]
    text, base = text_of_exe()
    seen, rows = set(), []
    tmp = tempfile.mkdtemp(prefix="libmatch_lib_")
    for lib in libs:
        n = 0
        for mem, body in members(lib):
            f = os.path.join(tmp, "m.obj"); open(f, "wb").write(body)
            try: fns = list(obj_functions(f))
            except Exception: continue
            for name, code, masked in fns:
                if len(code) < 6 or name.startswith(("$", "??")) or (name, code) in seen: continue
                seen.add((name, code))
                pat = b"".join(b"." if i in masked else re.escape(bytes([b])) for i, b in enumerate(code))
                hits = [m.start() for m in re.finditer(pat, text, re.S)]
                if hits:
                    for h in (hits if len(hits) <= 8 else hits[:1]):      # identical code at several addresses (two objects with the same helper): every address is a hit of this member
                        rows.append((base + h, name, "%s:%s" % (os.path.basename(lib), mem), len(code), "" if len(hits) == 1 else "%d candidates" % len(hits)))
                    n += 1
        print("%s: %d functions found" % (lib, n))
    os.unlink(os.path.join(tmp, "m.obj")) if os.path.exists(os.path.join(tmp, "m.obj")) else None
    with open(out, "w") as f:
        for va, name, mem, sz, note in sorted(rows):
            f.write("%08x,%s,%s,%d,%s\n" % (va, name, mem, sz, note))
    print(len(rows), "rows ->", out)

main()
