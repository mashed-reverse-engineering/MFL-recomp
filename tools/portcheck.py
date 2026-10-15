#!/usr/bin/env python3
"""portcheck.py [--update-baseline] [--quiet]: portability gate v0 for the all-C / Switch (AArch64 LP64, 8 byte pointers) build.  Warning gate: exit 1 only when a class count grows past
output/portcheck_baseline.txt (rewritten ONLY by --update-baseline).  Output: output/portcheck.csv (file, function, class, count, note) + summary.
Scope: src/tier1, src/automatch_agent, src/automatch_near, hand src/*.c (not bridges*, not harness*); PORT_CANDIDATES=1 adds output/tier2_candidates*/ (informational, never part of the baseline).  __declspec(naked) functions are bridge asm: class asm_naked, nothing else is scanned in them.
Compiler classes (cc_*): clang --target=aarch64-linux-gnu if installed, else native `gcc -m64` (LP64 like AArch64; same two cast warnings), -fsyntax-only with tools/port/msvc_shim.h.
  cc_int2ptr / cc_ptr2int  -Wint-to-pointer-cast / -Wpointer-to-int-cast diagnostics;  cc_error  any other error (VC7 constructs the shim does not cover, recorded not failed)
Heuristic classes (Ghidra style, text based, per function; declared pointer / int locals and parameters are parsed):
  h_addr_cast    `(int|uint|undefined4|dword|ulong|long)&x`          h_ptr_to_int   `(int)p` with p a declared pointer
  h_int_to_ptr   `*(T*)(i + 0xNN)` / `(T*)i` with i a declared int / undefined4     h_int_field_ptr  `(T*)s.field_0x..` (generic int typed field holding a pointer; name based)
  h_stride       line with a pointer cast and a `* 4` / `* 0x78` / `<< 2` constant stride    layout_assert  sizeof()== / C_ASSERT (struct size fixed by the original layout)
  asm_inline     `__asm`        x87_intrin  `_ftol` / `_CI*` / float10      asm_naked  __declspec(naked) functions
A function is `portable as is` when no class (compiler or heuristic, naked included) is hit in it.  Float results are not bit-exact across x87 / SSE / NEON (diffemu needs a tolerance mode there)."""
import csv, glob, os, re, shutil, subprocess, sys
from concurrent.futures import ThreadPoolExecutor
H = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(H, "..")); sys.path.insert(0, H)
import rawaddr
SHIM = os.path.join(H, "port", "msvc_shim.h")
CSV = os.path.join(G, "output", "portcheck.csv"); BASE = os.path.join(G, "output", "portcheck_baseline.txt")
CLASSES = ["cc_int2ptr", "cc_ptr2int", "cc_error", "h_addr_cast", "h_ptr_to_int", "h_int_to_ptr", "h_int_field_ptr", "h_stride", "layout_assert", "asm_inline", "x87_intrin", "asm_naked"]
INTT = r"(?:unsigned\s+)?(?:int|uint|undefined4|dword|ulong|long|short|DWORD|LONG|undefined)"
RX = {
 "h_addr_cast": re.compile(r"\(\s*" + INTT + r"\s*\)\s*&"),
 "h_int_field_ptr": re.compile(r"\(\s*[A-Za-z_][\w ]*\*+\s*\)\s*[A-Za-z_]\w*(?:\[[^\]]*\])*(?:\.|->)(?:field|unk|undefined|dword)\w*\b(?!\s*[\(\w])"),
 "layout_assert": re.compile(r"sizeof\s*\([^)]*\)\s*==|_Static_assert|\bC_ASSERT\b|STATIC_ASSERT"),
 "asm_inline": re.compile(r"\b_{1,2}asm\b"),
 "x87_intrin": re.compile(r"\b_ftol2?\b|\b_CI\w+|\bfloat10\b"),
}
CASTP = re.compile(r"\(\s*[A-Za-z_][\w ]*\*+\s*\)"); STRIDE = re.compile(r"\*\s*(?:0[xX][0-9a-fA-F]+|[4-9]|[1-9]\d+)\b|<<\s*[2-7]\b")
P2I = re.compile(r"\(\s*" + INTT + r"\s*\)\s*\(?\s*(\w+)\b"); I2P = re.compile(r"\(\s*[A-Za-z_][\w ]*\*+\s*\)\s*\(?\s*(\w+)\b")
DPTR = re.compile(r"(?m)^\s*(?!return|else|case|goto)(?:struct\s+|const\s+)?[A-Za-z_]\w*\s*\*+\s*(\w+)\s*[;=,\[]"); PPTR = re.compile(r"[A-Za-z_]\w*\s*\*+\s*(\w+)\s*[,)]")
DINT = re.compile(r"\b" + INTT + r"\s+(\w+)\s*[;,=)\[]")

def scope():
    r = os.path.join(G, "src"); fl = []
    for d in ("tier1", "automatch_agent", "automatch_near"): fl += glob.glob(os.path.join(r, d, "*.c"))
    fl += glob.glob(os.path.join(r, "*.c"))
    if os.environ.get("PORT_CANDIDATES"): fl += glob.glob(os.path.join(G, "output", "tier2_candidates", "*.c")) + glob.glob(os.path.join(G, "output", "tier2_candidates2", "*.c"))      # informational only: candidates are not part of the build, so they must not move the gate baseline
    return sorted(fl)

def heur(body, hdr):
    """-> {class: count} for one non-naked function (masked text)"""
    c = {}; text = hdr + body
    ptrs = set(DPTR.findall(text)) | set(PPTR.findall(hdr)); ints = set(DINT.findall(text)) - ptrs
    for k, rx in RX.items():
        n = len(rx.findall(body))
        if n: c[k] = n
    n = sum(1 for m in P2I.finditer(body) if m.group(1) in ptrs);  c["h_ptr_to_int"] = n
    n = sum(1 for m in I2P.finditer(body) if m.group(1) in ints);  c["h_int_to_ptr"] = n
    c["h_stride"] = sum(1 for l in body.split("\n") if CASTP.search(l) and STRIDE.search(l))
    return {k: v for k, v in c.items() if v}

def compile_diag(path, cc):
    d = os.path.dirname(path); R = os.path.join(G, "src")
    cmd = cc + ["-fsyntax-only", "-include", SHIM, "-I", d, "-I", os.path.join(R, "auto"), "-I", R, "-Wint-to-pointer-cast", "-Wpointer-to-int-cast", "-fmax-errors=0", "-ferror-limit=0", path] if cc[0] == "clang" else \
          cc + ["-fsyntax-only", "-include", SHIM, "-I", d, "-I", os.path.join(R, "auto"), "-I", R, "-Wint-to-pointer-cast", "-Wpointer-to-int-cast", "-fmax-errors=0", path]
    r = subprocess.run(cmd, capture_output=True, text=True, errors="replace"); out = []
    for l in r.stderr.split("\n"):
        m = re.match(r"(.*?):(\d+):\d+: (warning|error|fatal error): (.*)", l)
        if not m or os.path.realpath(m.group(1)) != os.path.realpath(path): continue
        k = "cc_int2ptr" if "-Wint-to-pointer-cast" in l else "cc_ptr2int" if "-Wpointer-to-int-cast" in l else "cc_error" if m.group(3) != "warning" else None
        if k: out.append((int(m.group(2)), k))
    return out

def scan(path, diags):
    t = open(path, errors="surrogateescape").read(); m = rawaddr.mask(t).replace("__declspec(naked)", "naked_attr_______"); rows = []; lines = []
    for name, s, e in rawaddr.functions(m):
        body = m[s:e]; b0 = body.find("{"); hdr, bd = body[:b0], body[b0:]; l0 = m[:s].count("\n") + 1; l1 = l0 + body.count("\n"); lines.append((l0, l1, name))
        if "naked_attr" in hdr: rows.append((name, "asm_naked", 1, "")); continue
        for k, v in heur(bd, hdr).items(): rows.append((name, k, v, ""))
        cc = {}
        for ln, k in diags:
            if l0 <= ln <= l1: cc[k] = cc.get(k, 0) + 1
        for k, v in cc.items(): rows.append((name, k, v, ""))
    for ln, k in diags:
        if not any(a <= ln <= b for a, b, _ in lines): rows.append(("-", k, 1, "outside functions line %d" % ln))
    return rows, [(a, b, n) for a, b, n in lines], len(m)

def load_sizes():
    z = {}
    for l in open(os.path.join(G, "output", "fnsizes.txt")):
        p = l.split()
        if len(p) == 3: z[p[1]] = int(p[2])
    return z

def main(argv):
    clang = shutil.which("clang"); cc = ["clang", "--target=aarch64-linux-gnu", "-Wno-everything", "-Wint-to-pointer-cast", "-Wpointer-to-int-cast"] if clang else ["gcc", "-m64"]
    cc = [cc[0]] + cc[1:] if clang else cc
    files = scope(); sizes = load_sizes()
    with ThreadPoolExecutor(2) as ex: diags = list(ex.map(lambda f: compile_diag(f, cc), files))
    tot = {k: 0 for k in CLASSES}; per = {}; rows = []; nfn = nport = bytes_all = bytes_port = 0
    for f, dg in zip(files, diags):
        rs, fns, _ = scan(f, dg); rel = os.path.relpath(f, G); hit = {(n) for n, k, v, _n in rs}
        for n, k, v, note in rs: rows.append((rel, n, k, v, note)); tot[k] += v; per[rel] = per.get(rel, 0) + v
        for _, _, n in fns:
            sz = sizes.get(n, 0); nfn += 1; bytes_all += sz
            if n not in hit: nport += 1; bytes_port += sz
    ta = open(os.path.join(G, "src", "mfl_types.h"), errors="replace").read().count("MFL_STATIC_ASSERT(") - 1    # minus the macro definition
    if ta > 0: rows.append(("src/mfl_types.h", "-", "layout_assert", ta, "MFL_STATIC_ASSERT offsets of the original struct layouts")); tot["layout_assert"] += ta
    os.makedirs(os.path.dirname(CSV), exist_ok=True)
    with open(CSV, "w", newline="") as o:
        w = csv.writer(o); w.writerow(["file", "function", "class", "count", "note"]); w.writerows(rows)
    q = "--quiet" in argv; sumline = "portcheck (%s): %d files, %d functions, portable as is %d (%.1f%%), %d of %d bytes (%.1f%%)" % (
        "clang aarch64" if clang else "gcc -m64, clang absent", len(files), nfn, nport, 100.0 * nport / max(nfn, 1), bytes_port, bytes_all, 100.0 * bytes_port / max(bytes_all, 1))
    if not q:
        print(sumline); [print("  %-16s %6d" % (k, tot[k])) for k in CLASSES]
        print("top files:"); [print("  %6d %s" % (v, k)) for k, v in sorted(per.items(), key=lambda x: -x[1])[:10]]
    if "--update-baseline" in argv:
        with open(BASE, "w") as o:
            o.write("# tools/portcheck.py baseline: a class count above this fails the gate; only `portcheck.py --update-baseline` rewrites it\n# %s\n" % sumline)
            for k in CLASSES: o.write("%s %d\n" % (k, tot[k]))
        print("baseline written"); return 0
    if not os.path.exists(BASE): print("portcheck: no baseline (run --update-baseline)"); return 0
    base = {l.split()[0]: int(l.split()[1]) for l in open(BASE) if l.strip() and not l.startswith("#")}
    grew = ["%s %d > %d" % (k, tot[k], base.get(k, 0)) for k in CLASSES if tot[k] > base.get(k, 0)]
    if q: print(sumline)
    print("portcheck: " + ("GREW: " + ", ".join(grew) if grew else "within baseline")); return 1 if grew else 0
if __name__ == "__main__": sys.exit(main(sys.argv[1:]))
