#!/usr/bin/env python3
"""Generate the conformance test table (src/harness_conf/conf_table.c) from src/manifest.txt and the `// FUNCTION: MFL 0x..` definitions of src/*.c.
Only plain functions are tested: cdecl or __stdcall, not naked, all parameters scalar (integer types, float, double), return void / integer / float / double.
Usage: conftest_gen.py [--skip name,name,...] [--only name,...]     writes conf_table.c and conf_names.txt (index, va, name, file, kinds, ret) next to it.
Functions that are dangerous to call with made up arguments (save game, exit, wait loops ...) are excluded by DENY; a function that hangs is added with --skip by the driver."""
import os, re, struct, sys
HERE = os.path.dirname(os.path.abspath(__file__))
R = os.path.join(HERE, "..", "src")
OUT = os.path.join(R, "harness_conf")
EXE = os.path.join(HERE, "..", "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")
DENY = re.compile(r"gamesave|autosave|exit|quit|wait|sleep|destroy|free|shutdown|release|loading_start|app_|movie|video_dialog|lua_|dinput|message_show|menu_page|race_begin|race_restart|race_finish")
INT = re.compile(r"^(?:(?:unsigned|signed|const|volatile)\s+)*(?:int|long|short|char|uint|ushort|uchar|byte|bool|BOOL|DWORD|WORD|BYTE|undefined[1248]?|u?int(?:8|16|32)_t|size_t)$")

def entries():
    for l in open(os.path.join(R, "manifest.txt")):
        p = l.split()
        if len(p) >= 3 and not l.startswith("#") and p[0] != "(none)":
            yield p[0], p[1], int(p[2], 16)

def definition(lines, va):
    for i, l in enumerate(lines):
        if l.strip() == "// FUNCTION: MFL 0x%08x" % va:
            j = i + 1
            while lines[j].startswith("__declspec"): j += 1
            return lines[j], i
    return None, None

def classify(defn):
    if "naked" in defn or "__fastcall" in defn or "__thiscall" in defn: return None
    m = re.match(r"^((?:__declspec\(\w+\)\s+)?[A-Za-z_][\w\s\*]*?)\s*(__stdcall|__cdecl)?\s*(\w+)\s*\(([^)]*)\)", defn)
    if not m: return None
    ret, conv, name, params = m.group(1).strip(), m.group(2), m.group(3), m.group(4).strip()
    if "*" in ret: return None
    rk = "v" if ret == "void" else "f" if ret == "float" else "d" if ret == "double" else "i" if INT.match(ret) else None
    if rk is None: return None
    kinds = ""
    if params and params != "void":
        for p in params.split(","):
            p = p.strip()
            if "*" in p or "[" in p or "..." in p: return None
            t = re.sub(r"\s*\b\w+$", "", p) if re.search(r"\s\w+$", p) else p
            if t == "float": kinds += "f"
            elif t == "double": kinds += "d"
            elif INT.match(t): kinds += "i"
            else: return None
    return name, kinds, rk, conv == "__stdcall"

def data_range():
    d = open(EXE, "rb").read()
    e = struct.unpack_from("<I", d, 0x3C)[0]
    ns = struct.unpack_from("<H", d, e + 6)[0]; osz = struct.unpack_from("<H", d, e + 20)[0]
    base = struct.unpack_from("<I", d, e + 24 + 28)[0]
    lo = hi = None
    for i in range(ns):
        o = e + 24 + osz + i * 40
        vs, va, rs, rp = struct.unpack_from("<IIII", d, o + 8)
        ch = struct.unpack_from("<I", d, o + 36)[0]
        if ch & 0x80000000 and not ch & 0x20:       # writable, not code
            lo = base + va if lo is None else min(lo, base + va); hi = base + va + max(vs, rs) if hi is None else max(hi, base + va + max(vs, rs))
    return lo, hi

def tier2_entries():
    """functions of src/automatch_near (tier 2, tools/automatch_promote.py --tier2): (name, va, definition text) read from the an_*.c files"""
    names = os.path.join(R, "automatch_near_names.txt")
    if not os.path.exists(names): return
    for l in open(names):
        p = l.split()
        if len(p) < 3: continue
        va, nm, ch = int(p[0], 16), p[1], p[2]
        path = os.path.join(R, "automatch_near", "an_" + ch)
        if not os.path.exists(path): continue
        t = open(path, encoding="latin-1").read()
        m = re.search(r"^((?:[A-Za-z_][\w \*]*?)\b%s\s*\([^;{}]*\))\s*\n\s*\{" % re.escape(nm), t, re.M)
        if m: yield nm, va, ch, re.sub(r"\s+", " ", re.sub(r"/\*.*?\*/", "", m.group(1), flags=re.S)).strip()

def main():
    skip, only = set(), None
    a = sys.argv[1:]
    if "--skip" in a: skip = set(a[a.index("--skip") + 1].split(","))
    if "--only" in a: only = set(a[a.index("--only") + 1].split(","))
    mode = a[a.index("--pass") + 1] if "--pass" in a else "all"      # A: functions with arguments or a return value (mostly getters/setters, rarely hang), B: void functions without arguments (state updaters / renderers: many hang without game state)
    rows, cache = [], {}
    for f, sym, va in entries():
        if sym in skip or (only and sym not in only) or DENY.search(sym): continue
        lines = cache.setdefault(f, open(os.path.join(R, f), encoding="latin-1").read().split("\n"))
        defn, _ = definition(lines, va)
        if not defn: continue
        c = classify(defn)
        if not c or c[0] != sym: continue
        zero_void = (c[1] == "" and c[2] == "v")
        if (mode == "A" and zero_void) or (mode == "B" and not zero_void): continue
        rows.append((va, sym, f, c[1], c[2], c[3]))
    for nm, va, ch, defn in tier2_entries():
        if nm in skip or (only and nm not in only) or DENY.search(nm): continue
        c = classify(defn)
        if not c or c[0] != nm: continue
        zero_void = (c[1] == "" and c[2] == "v")
        if (mode == "A" and zero_void) or (mode == "B" and not zero_void): continue
        rows.append((va, nm, "automatch_near/" + ch, c[1], c[2], c[3]))
    lo, hi = data_range()
    assert hi - lo <= 0x400000, "data range too big for the snapshot buffer"
    os.makedirs(OUT, exist_ok=True)
    with open(os.path.join(OUT, "conf_table.c"), "w") as o:
        o.write("/* generated by tools/conftest_gen.py: do not edit */\nstruct ConfEnt { unsigned va; int nargs; const char *kinds; int stdcall; int ret; };\n")
        o.write("struct ConfEnt conf_table[] = {\n")
        for va, sym, f, k, rk, sc in rows:
            o.write('    { 0x%08x, %d, "%s", %d, %d },   /* %s */\n' % (va, len(k), k, int(sc), ord(rk), sym))
        o.write("    { 0, 0, \"\", 0, 0 }\n};\nint conf_count = %d;\nunsigned conf_data_lo = 0x%08x, conf_data_hi = 0x%08x;\n" % (len(rows), lo, hi))
    with open(os.path.join(OUT, "conf_names.txt"), "w") as o:
        for i, (va, sym, f, k, rk, sc) in enumerate(rows):
            o.write("%d %08x %s %s %s %s\n" % (i, va, sym, f, k or "-", rk))
    print("%d functions in the table, data range %08x-%08x" % (len(rows), lo, hi))

main()
