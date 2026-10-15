#!/usr/bin/env python3
"""Reorganise the hand written recomp sources (batch4..28.c, race_flow.c, ...) into subsystem modules (stdlib only).
Usage: reorg.py <out dir> [--sources a.c,b.c,...]

* every function (with the comment block directly above it) is moved verbatim to the module of its name prefix (PREFIX table below; unknown prefixes go to `misc` and are listed)
* modules are split by the `#pragma auto_inline(off)` flag of the source file (it changes the generated code, so functions with and without the pragma never share a file)
* all top level `extern` declarations and local struct definitions of the sources become ONE generated header `mfl_externs.h`; names declared with different types in
  different sources are conflicts (report `conflicts.txt`): the functions of the sources that use a non canonical variant are written to `keep_<source>.c` with the
  complete preamble of their source file, so nothing changes silently
* `moved.csv` (function,old file,new file) lets tools/reorg_manifest.py rewrite src/manifest.txt
Verification is done by the caller: tools/cmpfn.py --manifest on the new tree must give the same statuses as before."""
import collections, glob, os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
RECOMP = os.path.join(HERE, "..", "src")
PREFIX = {
    "car": "car", "cars": "car", "vehicle": "vehicle", "course": "course", "player": "player", "players": "player", "weapon": "weapons", "rocket": "weapons", "ktc": "weapons",
    "powerup": "weapons", "fx": "effects", "effect": "effects", "effects": "effects", "particles": "effects", "trail": "effects", "ptank": "effects", "rain": "effects",
    "race": "race", "time": "race", "replay": "race", "round": "race", "intro": "race", "career": "career", "cup": "career", "attract": "career",
    "gamesave": "system", "autosave": "system", "load": "system", "loading": "system", "app": "system", "input": "system", "dinput": "system", "game": "system",
    "frontend": "system", "movie": "system", "message": "system", "controllers": "system", "lua": "lua", "rwp": "physics", "rw": "physics", "physics": "physics", "smplfzx": "physics",
    "spatial": "geometry", "bbox": "geometry", "line": "geometry", "find": "geometry", "get": "geometry", "random": "geometry", "menu": "ui", "ui": "ui", "btb": "ui", "hud": "ui",
    "text": "ui", "wstring": "ui", "camera": "render", "viewport": "render", "scene": "render", "aux": "render", "gfx": "render", "clump": "render", "clumps": "render",
    "object": "render", "atomic": "render", "material": "render", "materials": "render", "world": "render", "pool": "render", "table": "render", "sound": "audio", "voice": "audio",
    "crown": "misc", "triggers": "misc", "ai": "ai",
}


def strip_code(line):
    line = re.sub(r'"(\\.|[^"\\])*"', '""', line)
    line = re.sub(r"'(\\.|[^'\\])+'", "' '", line)
    return line


def parse(path):
    """-> list of items (kind, text, name) ; kinds: pragma include define extern type func comment"""
    lines = open(path, errors="replace").read().split("\n")
    items, buf, depth, incomment, pending = [], [], 0, False, []
    i = 0
    while i < len(lines):
        l = lines[i]
        if depth == 0 and not buf:
            s = l.strip()
            if incomment or s.startswith("/*") or s.startswith("//"):
                pending.append(l)
                if not incomment and s.startswith("/*") and "*/" not in s: incomment = True
                elif incomment and "*/" in s: incomment = False
                i += 1
                continue
            if s == "":
                if pending:
                    items.append(("comment", "\n".join(pending), None)); pending = []
                i += 1
                continue
            if s.startswith("#"):
                if pending: items.append(("comment", "\n".join(pending), None)); pending = []
                kind = "pragma" if s.startswith("#pragma") else ("include" if s.startswith("#include") else "define")
                items.append((kind, l, None)); i += 1; continue
        buf.append(l)
        code = re.sub(r"/\*.*?\*/", "", strip_code(l))
        code = code.split("//")[0]
        depth += code.count("{") - code.count("}")
        if depth == 0 and (code.rstrip().endswith(";") or code.rstrip().endswith("}")):
            text = "\n".join(buf)
            flat = " ".join(text.split())
            if flat.startswith("extern "):
                items.append(("extern", text, None))
            elif re.match(r"(struct|typedef|union|enum)\b", flat) and "{" in flat and not re.search(r"\)\s*\{", flat.split("{", 1)[0] + "{"):
                items.append(("type", text, None))
            elif "{" in text and "(" in text.split("{", 1)[0]:
                head = text.split("{", 1)[0]
                nm = re.findall(r"\b(\w+)\s*\(", re.sub(r"__declspec\(naked\)", "", head))
                items.append(("func", ("\n".join(pending) + "\n" if pending else "") + text, nm[0] if nm else None)); pending = []
            else:
                items.append(("other", text, None))
            buf = []
        i += 1
    return items


def split_externs(text):
    """-> list of (name, normalized declaration text) for one extern item (a variable list or a prototype)"""
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    text = re.sub(r"//[^\n]*", "", text)
    flat = " ".join(text.split()).rstrip(";").strip()
    body = flat[len("extern "):]
    if "(" in body.split("[")[0] and not re.match(r".*\(\*\s*\w+\)\s*\(", body.split(",")[0]):
        m = re.match(r"(.*?)\b(\w+)\s*\((.*)\)\s*$", body)
        if m and "," not in body.split("(")[0]: return [("F:" + m.group(2), flat + ";")]
        names = re.findall(r"\b(\w+)\s*\(", body)
        return [("F:" + names[0], flat + ";")] if names else []
    parts = [p.strip() for p in body.split(",")]
    m = re.match(r"(.*?[\s\*])(\w+)((?:\[[^\]]*\])*)$", parts[0])
    if not m: return []
    base = re.match(r"((?:volatile |const |unsigned |struct )*[A-Za-z_]\w*(?: (?:int|char|short|long))?)", m.group(1).strip()).group(1)
    res = [(m.group(2), "extern %s;" % (m.group(1).strip() + " " + m.group(2) + m.group(3)))]
    for p in parts[1:]:
        mm = re.match(r"(\**)\s*(\w+)((?:\[[^\]]*\])*)$", p)
        if mm: res.append((mm.group(2), "extern %s %s%s%s;" % (base, mm.group(1), mm.group(2), mm.group(3))))
    return res


def canon(t):
    t = re.sub(r"\s+", "", t)
    return t.replace("[]", "[0]") if False else t


def main():
    out = os.path.abspath(sys.argv[1])
    os.makedirs(out, exist_ok=True)
    if "--sources" in sys.argv:
        srcs = sys.argv[sys.argv.index("--sources") + 1].split(",")
    else:
        srcs = []
        for f in sorted(glob.glob(os.path.join(RECOMP, "*.c"))):
            if "tools/reorg.py" in open(f, errors="replace").read(400): continue      # generated by this tool earlier: not a source
            srcs.append(os.path.basename(f))
    legacy = set(sys.argv[sys.argv.index("--legacy") + 1].split(",")) if "--legacy" in sys.argv else set()
    parsed = {s: parse(os.path.join(RECOMP, s)) for s in srcs}
    # declarations: name -> {canonical text: [sources]}
    decls = collections.defaultdict(lambda: collections.defaultdict(list))
    first = {}
    for s, items in parsed.items():
        for kind, text, name in items:
            if kind == "extern":
                for n, t in split_externs(text):
                    decls[n][canon(t)].append(s); first.setdefault((n, canon(t)), t)
    conflicts = {n: v for n, v in decls.items() if len(v) > 1}
    # a variant that differs only in spacing / array size is not a conflict: compare after dropping array extents and `int [] / int[]` forms
    def loose(c): return re.sub(r"\[[^\]]*\]", "[]", c)
    real = {}
    for n, v in conflicts.items():
        if len({loose(c) for c in v}) > 1: real[n] = v
    canonical = {}
    for n, v in decls.items():
        if n in real: continue
        best = max(v, key=lambda c: (len(v[c]), "[" in c and not c.endswith("[]") if False else 0))
        canonical[n] = first[(n, best)]
    types = {}
    type_conf = set()
    for s, items in parsed.items():
        for kind, text, name in items:
            if kind == "type":
                key = " ".join(text.split()).split("{")[0].strip()
                if key in types and " ".join(types[key].split()) != " ".join(text.split()): type_conf.add(key)
                types.setdefault(key, text)
    # functions
    modules = collections.defaultdict(list)       # module file -> [(func text, name, source)]
    keep = collections.defaultdict(list)          # source -> [(func text, name)]
    misc = []
    moved = []
    for s, items in parsed.items():
        pragma_off = any(k == "pragma" and "auto_inline(off)" in t for k, t, n in items)
        for kind, text, name in items:
            if kind != "func" or not name: continue
            if s in legacy:                  # a source whose functions changed when merged (the compiler uses what it knows of callees of the same translation unit): kept as it is
                moved.append((name, s, s)); continue
            used = set(re.findall(r"[A-Za-z_]\w*", text))
            if any(("F:" + n if "F:" + n in real else n) in real and n in used for n in [x[2:] if x.startswith("F:") else x for x in real]):
                # uses a name with conflicting declarations: keep the function with the preamble of its own source
                variant_src = [src for nm_, v in real.items() for c, srcs_ in v.items() for src in srcs_ if nm_.replace("F:", "") in used]
                if s in variant_src:
                    keep[s].append((text, name)); continue
            pre = re.match(r"[a-z]+", name)
            sub = PREFIX.get(pre.group(0) if pre else "", None)
            if sub is None:
                sub = "misc"; misc.append(name)
            fn = "%s%s.c" % (sub, "" if pragma_off else "_np")
            modules[fn].append((text, name, s))
            moved.append((name, s, fn))
    protos = {}
    for s_, items in parsed.items():
        for kind, text, name in items:
            if kind == "func" and name:
                head = re.sub(r"/\*.*?\*/", " ", text, flags=re.S).split("{", 1)[0]
                head = re.sub(r"//[^\n]*", " ", head)
                protos[name] = " ".join(head.replace("__declspec(naked)", " ").split()) + ";"        # naked is only allowed on definitions
    # write
    hdr = ["/* MFL.exe: all top level declarations of the recomp sources (generated by tools/reorg.py; names declared with different types in different sources are NOT here,",
           "   their functions stay in keep_*.c). */", "#ifndef MFL_EXTERNS_H", "#define MFL_EXTERNS_H", ""]
    seen_inc = []
    for s_, items in parsed.items():
        for kind, text, name in items:
            if kind == "include" and text.strip() not in seen_inc: seen_inc.append(text.strip())
    hdr += seen_inc + [""]
    seen_def = {}
    for s_, items in parsed.items():
        for kind, text, name in items:
            if kind == "define":
                m = re.match(r"#\s*define\s+(\w+)", text.strip())
                if m and m.group(1) not in seen_def:
                    seen_def[m.group(1)] = text
                    hdr.append(text)
                elif m and " ".join(seen_def[m.group(1)].split()) != " ".join(text.split()):
                    type_conf.add("#define " + m.group(1))
    hdr.append("")
    for key, text in types.items():
        if key not in type_conf: hdr.append(text); hdr.append("")
    for n in sorted(canonical):
        if n.startswith("F:") and n[2:] in protos: continue        # a function we define: the prototype comes from its definition
        hdr.append(canonical[n])
    hdr.append("")
    for n in sorted(protos): hdr.append(protos[n])
    hdr += ["", "#endif", ""]
    open(os.path.join(out, "mfl_externs.h"), "w").write("\n".join(hdr))
    for fn, fs in sorted(modules.items()):
        pragma = "#pragma auto_inline(off)\n" if not fn.endswith("_np.c") else ""
        origin = sorted({s for _, _, s in fs})
        body = "/* MFL.exe %s: functions moved verbatim from %s by tools/reorg.py (shared declarations: mfl_externs.h). */\n%s#include \"mfl_types.h\"\n#include \"mfl_externs.h\"\n\n" % (
            fn[:-2], ", ".join(origin), pragma)
        body += "\n\n".join(t for t, n, s in fs) + "\n"
        open(os.path.join(out, fn), "w").write(body)
    for s in sorted(legacy):
        open(os.path.join(out, s), "w").write(open(os.path.join(RECOMP, s)).read())
    for s, fs in keep.items():
        items = parsed[s]
        pre = [t for k, t, n in items if k in ("pragma", "include", "define", "extern", "type", "other")]
        body = "/* functions of %s that use a name declared with a different type in another source (tools/reorg.py); the preamble of %s is kept as it was */\n" % (s, s)
        declared = set()
        for k, t, n in items:
            if k == "extern":
                declared |= set(re.findall(r"[A-Za-z_]\w*", re.sub(r"/\*.*?\*/", "", t, flags=re.S)))
        defined_here = {n for t, n in fs}
        used = set(re.findall(r"[A-Za-z_]\w*", " ".join(t for t, n in fs)))
        extra = [protos[n] for n in sorted(used & set(protos)) if n not in declared and n not in defined_here]
        body += "\n".join(pre) + "\n/* prototypes of functions that live in other modules */\n" + "\n".join(extra) + "\n\n" + "\n\n".join(t for t, n in fs) + "\n"
        open(os.path.join(out, "keep_" + s), "w").write(body)
        for t, n in fs: moved.append((n, s, "keep_" + s))
    with open(os.path.join(out, "moved.csv"), "w") as f:
        f.write("function,old,new\n")
        for n, o, nw in moved: f.write("%s,%s,%s\n" % (n, o, nw))
    with open(os.path.join(out, "conflicts.txt"), "w") as f:
        for n, v in sorted(real.items()):
            f.write("%s: %s\n" % (n, "; ".join("%s [%s]" % (c, ",".join(sorted(set(sc)))) for c, sc in v.items())))
        for k in sorted(type_conf): f.write("type %s declared twice with different bodies\n" % k)
        for n in misc: f.write("no module for %s\n" % n)
    nf = sum(len(v) for v in modules.values())
    print("modules: %d files, %d functions moved, %d kept (%d sources), %d real conflicts, %d in misc" % (len(modules), nf, sum(len(v) for v in keep.values()), len(keep), len(real), len(misc)))


if __name__ == "__main__":
    main()
