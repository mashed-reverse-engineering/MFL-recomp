"""File scope data declarations of our C (src/**/*.c, *.h), single line form only.  Shared by globgen.py (T2 scalar types) and types.py (T0/T1).
scan(names) -> {name: [dict(file, extern, base, ptr, dims, init)]}      base = normalised type without const/volatile/signed, ptr = number of '*', dims = [int|None]"""
import os, re

G = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
DECL = re.compile(r"^(extern[ \t]+)?((?:(?:const|volatile|unsigned|signed|struct|union|enum)[ \t]+)*[A-Za-z_]\w*(?:[ \t]+(?:int|long|char|short|double))*(?:[ \t]*\*+)?)[ \t*]*?[ \t]+\**[ \t]*(\w+)((?:\[[^\]\n]*\])*)[ \t]*(=[^;\n]*)?;", re.M)
SIZE = {"char": 1, "short": 2, "int": 4, "long": 4, "float": 4, "double": 8, "__int64": 8, "long long": 8, "unsigned": 4, "short int": 2, "long int": 4, "long double": 8}
CLS = {"char": "i1", "short": "i2", "short int": "i2", "int": "i4", "long": "i4", "long int": "i4", "unsigned": "i4", "float": "f4", "double": "f8", "__int64": "i8", "long long": "i8"}


def norm(t):
    t = re.sub(r"\b(const|volatile|signed|extern)\b", " ", t)
    unsigned = bool(re.search(r"\bunsigned\b", t))
    t = re.sub(r"\bunsigned\b", " ", t)
    t = " ".join(t.replace("*", " ").split())
    return (t or "unsigned"), unsigned


def scan(names, root=None):
    names = set(names)
    out = {}
    for r, ds, fs in os.walk(root or os.path.join(G, "src")):
        ds[:] = [d for d in ds if not d.startswith("harness")]       # test harnesses define private copies
        for f in fs:
            if not f.endswith((".c", ".h")): continue
            p = os.path.join(r, f)
            t = open(p, errors="replace").read()
            for m in DECL.finditer(t):
                nm = m.group(3)
                if nm not in names: continue
                line_start = t.rfind("\n", 0, m.start()) + 1
                if m.start() != line_start: continue
                if re.search(r"\b(static|typedef|return)\b", t[m.start():m.end()]): continue
                base, uns = norm(m.group(2))
                full = t[m.start():m.end()]
                ptr = full.count("*", 0, full.index(nm))
                dims = [int(x) if x.strip().isdigit() else None for x in re.findall(r"\[([^\]]*)\]", m.group(4) or "")]
                out.setdefault(nm, []).append(dict(file=os.path.relpath(p, G), extern=bool(m.group(1)), base=base, unsigned=uns, ptr=ptr, dims=dims, init=bool(m.group(5))))
    return out


def scalar_class(decls):
    """-> class string ('i1','i2','i4','f4', with 'u' prefix for unsigned) if every declaration is the same non-array, non-pointer builtin scalar, else None"""
    cs = set()
    for d in decls:
        if d["ptr"] or d["dims"] or d["base"] not in CLS: return None
        cs.add(("u" if d["unsigned"] and CLS[d["base"]][0] == "i" else "") + CLS[d["base"]])
    return cs.pop() if len(cs) == 1 else None


def probe_sizes(bases, work, vc7):
    """sizeof() of non-builtin types (struct X / typedef names) as cl 13.00 computes it, with the real headers.  -> {base: size or None}.
    work: empty scratch dir below the container mounts.  Types the headers do not define are dropped round by round (None)."""
    import subprocess
    bases = sorted(bases)
    res = {b: None for b in bases}
    live = list(bases)
    for _ in range(4):
        src = ['#include "mfl_externs.h"'] + ["char sz_%d[sizeof(%s)];" % (i, b) for i, b in enumerate(live)]
        open(os.path.join(work, "probe.c"), "w").write("\n".join(src) + "\n")
        p = subprocess.run([vc7, work, "cl.exe", "/nologo", "/c", "/FA", "/I" + os.path.join(G, "src"), "probe.c"], capture_output=True, text=True, env=dict(os.environ, VC7_CONTAINER=os.environ.get("VC7_CONTAINER", "globgen")))
        bad = {int(m.group(1)) - 2 for m in re.finditer(r"probe\.c\((\d+)\) : error", p.stdout)}
        if not bad: break
        live = [b for i, b in enumerate(live) if i not in bad]
    asm = open(os.path.join(work, "probe.asm"), errors="replace").read() if os.path.exists(os.path.join(work, "probe.asm")) else ""
    for m in re.finditer(r"COMM\s+_sz_(\d+):BYTE:([0-9A-Fa-f]+)H", asm):
        res[live[int(m.group(1))]] = int(m.group(2), 16)
    return res
