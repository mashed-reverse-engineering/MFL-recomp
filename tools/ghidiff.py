#!/usr/bin/env python3
"""ghidiff.py <name>... | @listfile | --fn <name> <file.c> <va>   Ghidra round-trip diff.
Compiles OUR current C of each function (extract_fn standalone file, cl /nologo /c /Gy /O2, like cmpfn), places the .obj in a copy of MFL.exe with overlay.py --no-patch,
then decompiles the ORIGINAL (at its VA) and OUR copy (at its overlay VA) with the same settings and signature in ONE Ghidra headless run (scripts/DecompPair.java, scratch COPY of the
project, -readOnly -noanalysis), normalises both pseudo-C texts and writes output/ghidiff/<name>.txt: header, normalised pseudo-C diff (max 40 lines), one LEVER line per hunk.
A source for a bare <name> is found in output/nearmiss_triage.csv (chunk + srcdir), src/automatch_agent/ag_<name>.c or any src/**/*.c that defines it.
Env: GHIDIFF_TMP (scratch, must be below $TMPDIR for the compiler container), GHIDIFF_PROJ (project copy, default <tmp>/gd_proj), GHIDIFF_OUT (default output/ghidiff).
The verdict (MATCH or not) always comes from tools/check_one.py, run here too; the pseudo-C diff only suggests which source lever to try."""
import csv, difflib, fcntl, glob, os, re, shutil, subprocess, sys
H = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
T = os.path.join(H, "tools")
sys.path.insert(0, T)
EXE = os.environ.get("MFL_EXE", os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "game", "MFL.exe"))
GHIDRA = os.path.join(os.environ.get("GHIDRA_HOME", os.path.expanduser("~/tools/ghidra_12.1.4_PUBLIC")), "support", "analyzeHeadless")
TMP = os.environ.get("GHIDIFF_TMP", os.path.join(os.environ.get("TMPDIR", "/tmp/claude-%d" % os.getuid()), "ghidiff"))
PROJ = os.environ.get("GHIDIFF_PROJ", os.path.join(TMP, "gd_proj"))
OUT = os.environ.get("GHIDIFF_OUT", os.path.join(H, "output", "ghidiff"))
os.environ.setdefault("VC7_CONTAINER", "cmpfn")


def symtab():
    d = {}
    for l in open(os.path.join(H, "src", "symbols.txt")):
        p = l.split()
        if len(p) == 2: d.setdefault(p[0], int(p[1], 16))
    return d


def find_source(name):
    """-> (path, kind)  kind 'chunk' (extract_fn needed) or 'file'"""
    c = os.path.join(H, "src", "automatch_agent", "ag_%s.c" % name)
    if os.path.exists(c): return c, "chunk"
    p = os.path.join(H, "output", "nearmiss_triage.csv")
    if os.path.exists(p):
        for r in csv.DictReader(open(p)):
            if r["name"] == name and os.path.exists(os.path.join(r["srcdir"], r["chunk"])):
                return os.path.join(r["srcdir"], r["chunk"]), "chunk"
    pat = re.compile(r"^(?!extern)[A-Za-z_][\w \*]*\b%s\(.*\)\s*$" % re.escape(name), re.M)
    for f in sorted(glob.glob(os.path.join(H, "src", "**", "*.c"), recursive=True)):
        if "/gen/" in f or "/bridges" in f: continue
        try:
            if pat.search(open(f, errors="replace").read()): return f, "chunk"
        except OSError: pass
    sys.exit("no source for %s (use --fn)" % name)


def prepare(name, src, va, w):
    """standalone f.c (+ headers) in w, compile, keep f.obj; returns True when f.obj exists"""
    os.makedirs(w, exist_ok=True)
    f = os.path.join(w, "f.c")
    r = subprocess.run([sys.executable, os.path.join(T, "extract_fn.py"), src, name, f], capture_output=True, text=True)
    if r.returncode != 0:
        shutil.copy(src, f)
        for h in os.listdir(os.path.dirname(os.path.abspath(src))):
            if h.endswith(".h"): shutil.copy(os.path.join(os.path.dirname(os.path.abspath(src)), h), w)
    if os.path.exists(os.path.join(w, "f.obj")): os.unlink(os.path.join(w, "f.obj"))
    subprocess.run([os.path.join(T, "vc7.sh"), w, "cl.exe", "/nologo", "/c", "/Gy", "/O2", "f.c"], capture_output=True, text=True, timeout=300)
    return os.path.exists(os.path.join(w, "f.obj"))


def overlay(name, w):
    out = os.path.join(w, "out.exe")
    sy = os.path.join(w, "syms.txt")      # symbols.txt + the anonymous func_0x<va> / DAT_<va> names the chunk uses
    extra = {m.group(0): m.group(2) for m in re.finditer(r"\b(func_0x|DAT_|PTR_DAT_)([0-9a-fA-F]{8})\b", open(os.path.join(w, "f.c"), errors="replace").read())}
    open(sy, "w").write(open(os.path.join(H, "src", "symbols.txt")).read() + "".join("%s %s\n" % kv for kv in extra.items()))
    r = subprocess.run([sys.executable, os.path.join(T, "overlay.py"), EXE, out, sy, "--no-patch", os.path.join(w, "f.obj")], capture_output=True, text=True)
    try:
        for l in open(out + ".map"):
            p = l.split()
            if len(p) == 2 and p[0] == name: return out, int(p[1], 16)
    except OSError: pass
    return None, (r.stdout + r.stderr)[-200:]


# ---------------------------------------------------------------- normalisation
KW = {"if", "else", "while", "for", "do", "return", "switch", "case", "break", "goto", "sizeof", "void", "int", "char", "float", "bool", "short", "long", "uint", "undefined", "double", "NULL"}


def normalise(text, name, ovl_va):
    t = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    t = re.sub(r"//[^\n]*", "", t)
    t = re.sub(r"\b%s\b" % re.escape(name), "SELF", t)
    if ovl_va:   # addresses inside our overlay section (literal pool / jump tables): offset only
        t = re.sub(r"\b([A-Za-z_]*)(?:0x)?0*([0-9a-fA-F]{6,8})\b", lambda m: ("%sOVL_%x" % (m.group(1), int(m.group(2), 16) - ovl_va)) if 0 <= int(m.group(2), 16) - ovl_va < 0x10000 and m.group(2).lower().startswith(("9", "a", "b", "c")) else m.group(0), t)
    lines = [l.rstrip() for l in t.split("\n") if l.strip()]
    # locals: the decl block is the run of `type name;` lines right after the opening brace
    try: ob = lines.index("{")
    except ValueError: ob = -1
    k = ob + 1
    decl_re = re.compile(r"^\s+[A-Za-z_][\w \*]*?[ \*]\w+(\[\d+\])?;$")
    while ob >= 0 and k < len(lines) and decl_re.match(lines[k]) and not lines[k].lstrip().startswith("return"): k += 1
    decls, body = lines[ob + 1:k], lines[k:]
    order = {}
    for l in body:
        for m in re.finditer(r"\b([a-z]+)Var(\d+)\b", l):
            order.setdefault(m.group(0), len(order) + 1)
    for l in decls:
        for m in re.finditer(r"\b([a-z]+)Var(\d+)\b", l): order.setdefault(m.group(0), len(order) + 1)   # unused in body
    def ren(l): return re.sub(r"\b([a-z]+)Var(\d+)\b", lambda m: "%s_V%d" % (m.group(1), order[m.group(0)]), l)
    decls = sorted(ren(l) for l in decls)
    return (lines[:ob + 1] if ob >= 0 else []) + decls + [ren(l) for l in body]


# ---------------------------------------------------------------- hunk classification
OPS = {"<": ">", ">": "<", "<=": ">=", ">=": "<=", "==": "==", "!=": "!="}
NEG = {"<": ">=", ">": "<=", "<=": ">", ">=": "<", "==": "!=", "!=": "=="}
ATOM = r"[\w\.\[\]\->\*&]+(?:\([^()]*\))?"
CMP = re.compile(r"(%s)\s*(<=|>=|==|!=|<|>)\s*(%s)" % (ATOM, ATOM))
NOTCMP = re.compile(r"!\((%s)\s*(<=|>=|==|!=|<|>)\s*(%s)\)" % (ATOM, ATOM))
CAST = re.compile(r"\((?:u?(?:int|char|short|long|byte)|ushort|uchar|byte|undefined\d?|short|char|int)\)")


FLAG = re.compile(r"\((%s) < (%s) != \(\1 == \2\)\)" % (ATOM, ATOM))      # Ghidra's rendering of an x87 `a <= b` flag test


def canon_cmp(s, neg=True, swap=True):
    s = FLAG.sub(lambda m: "(%s <= %s)" % (m.group(1), m.group(2)), s)
    if neg: s = NOTCMP.sub(lambda m: "%s %s %s" % (m.group(1), NEG[m.group(2)], m.group(3)), s)
    def f(m):
        a, op, b = m.groups()
        return "%s %s %s" % (b, OPS[op], a) if swap and op in (">", ">=") else "%s %s %s" % (a, op, b)
    return CMP.sub(f, s)


def classify(rem, add):
    """rem/add: removed (original) and added (ours) lines of one hunk -> lever string"""
    r, a = "\n".join(l.strip() for l in rem), "\n".join(l.strip() for l in add)
    tr = re.sub(r"\b(bool|int|char|short|uint|undefined\d?|byte)\b", "T", r)
    ta = re.sub(r"\b(bool|int|char|short|uint|undefined\d?|byte)\b", "T", a)
    if (r != a) and (tr == ta) and re.search(r"\b(bool|int)\b", r + a) and re.search(r"\bbool\b", r + a): return "int flag"
    if r != a and canon_cmp(r, False, True) == canon_cmp(a, False, True) and canon_cmp(r, False, False) != canon_cmp(a, False, False): return "operand swap"
    if r != a and canon_cmp(r, True, True) == canon_cmp(a, True, True): return "negated compare"
    if len(CAST.findall(r)) != len(CAST.findall(a)): return "width/cast"
    lp = lambda s: len(re.findall(r"\b(while|for|do)\b", s))
    if lp(r) != lp(a) or (lp(r) and re.findall(r"\b(while|for|do)\b", r) != re.findall(r"\b(while|for|do)\b", a)): return "loop form"
    dl = lambda ls: sum(1 for l in ls if re.match(r"^[A-Za-z_][\w \*]*[ \*]\w+(\[\d+\])?;$", l.strip()))
    asg = lambda ls: sum(1 for l in ls if re.match(r"^\w+\s*=[^=]", l.strip()))
    if dl(rem) != dl(add) or asg(rem) != asg(add): return "split/merge expression"
    return "other"


def src_line(srcf, add, rem):
    """a source line that mentions the most distinctive identifier of the hunk"""
    try: src = open(srcf, errors="replace").read().split("\n")
    except OSError: return None
    ids = [w.lstrip("_") for l in add + rem for w in re.findall(r"[A-Za-z_]\w{3,}", l) if w not in KW and not re.match(r"[a-z]+_V\d+$|SELF|OVL_", w)]
    for w in sorted(set(ids), key=lambda x: -len(x)):
        for i, l in enumerate(src):
            if re.search(r"\b%s\b" % re.escape(w), l) and not l.startswith("extern") and "/*" not in l[:2]: return i + 1
    return None


def diff_report(orig, ours, srcf):
    o, u = normalise(orig[0], orig[1], 0), normalise(ours[0], ours[1], ours[2])
    ud = list(difflib.unified_diff(o, u, "orig", "ours", lineterm="", n=2))
    levers = []
    sm = difflib.SequenceMatcher(None, o, u, autojunk=False)
    for grp in sm.get_grouped_opcodes(0):
        rem = [l for tag, i1, i2, j1, j2 in grp for l in o[i1:i2]]
        add = [l for tag, i1, i2, j1, j2 in grp for l in u[j1:j2]]
        if not rem and not add: continue
        lv = classify(rem, add)
        sl = src_line(srcf, add, rem)
        levers.append("LEVER: %s @ %s:%s   [- %s | + %s]" % (lv, os.path.basename(srcf), sl or "?", " ".join(x.strip() for x in rem)[:70], " ".join(x.strip() for x in add)[:70]))
    return ud, levers


# ---------------------------------------------------------------- driver
def main():
    a = sys.argv[1:]
    if not a: sys.exit(__doc__)
    jobs = []      # (name, src, va)
    syms = symtab()
    if a[0] == "--fn":
        jobs.append((a[1], os.path.abspath(a[2]), int(a[3], 16)))
    else:
        names = []
        for x in a:
            names += [l.split()[0] for l in open(x[1:]) if l.strip() and not l.startswith("#")] if x.startswith("@") else [x]
        for n in names:
            if n not in syms: print("skip %s: not in symbols.txt" % n); continue
            jobs.append((n, find_source(n)[0], syms[n]))
    os.makedirs(TMP, exist_ok=True); os.makedirs(OUT, exist_ok=True)
    if not os.path.isdir(PROJ): shutil.copytree(os.path.expanduser("~/ghidra-projects/mashed"), PROJ)
    ready = []
    jf = os.path.join(TMP, "jobs.txt")
    raw = os.path.join(TMP, "raw")
    shutil.rmtree(raw, ignore_errors=True)
    with open(jf, "w") as jfh:
        for name, src, va in jobs:
            w = os.path.join(TMP, "work", name)
            if not prepare(name, src, va, w): print("%s: compile error" % name); continue
            out, ova = overlay(name, w)
            if out is None: print("%s: overlay failed: %s" % (name, ova)); continue
            jfh.write("%s %08x %s %08x\n" % (name, va, out, ova))
            ready.append((name, src, va, ova, w))
    lock = open(os.path.join(TMP, "ghidra.lock"), "w")
    fcntl.flock(lock, fcntl.LOCK_EX)       # never two Ghidra runs at once
    r = subprocess.run([GHIDRA, PROJ, "mfl", "-process", "MFL.exe", "-readOnly", "-noanalysis", "-scriptPath", os.path.join(H, "scripts"), "-postScript", "DecompPair.java", jf, raw], capture_output=True, text=True)
    fcntl.flock(lock, fcntl.LOCK_UN)
    open(os.path.join(TMP, "ghidra.log"), "w").write(r.stdout + r.stderr)
    for l in (r.stdout + r.stderr).splitlines():
        if "pair FAIL" in l or "ERROR" in l: print(l[:200])
    import cmpfn
    for name, src, va, ova, w in ready:
        po, pu = os.path.join(raw, name + ".orig.c"), os.path.join(raw, name + ".ours.c")
        if not (os.path.exists(po) and os.path.exists(pu)): print("%s: no decompile output" % name); continue
        ck = subprocess.run([sys.executable, os.path.join(T, "check_one.py"), os.path.join(w, "f.c"), name], capture_output=True, text=True, env=dict(os.environ))
        cl = ck.stdout.splitlines()
        asm = [l for l in cl if l[:1] in "+-" and not l.startswith(("---", "+++"))]
        verdict = next((l for l in cl if l.startswith(("MATCH", "DIFFERENT"))), "DIFFERENT" if ck.returncode == 1 else "ERROR rc=%d" % ck.returncode)
        cat = "-"
        if not verdict.startswith("MATCH"):
            try:
                det = cmpfn.check(os.path.join(w, "f.c"), [(name, va)], ["/O2"])[name][1]
                cat = (re.match(r"\[(\w+) [\d.]+\]", det) or [None, "-"])[1]
            except Exception: pass
        ud, levers = diff_report((open(po).read(), name), (open(pu).read(), name, ova), os.path.join(w, "f.c"))
        if not ud and not verdict.startswith("MATCH"): levers = ["LEVER: not visible in source: regalloc/sched, try permute"]
        txt = ["name: %s" % name, "va: %08x" % va, "size: %s" % cmpfn.FNSZ.get(va, "?"), "check_one: %s" % verdict[:160],
               "asm diff lines: %d" % len(asm), "category: %s" % cat, "source: %s" % src, "", "--- normalised pseudo-C diff (orig -, ours +), max 40 lines ---"]
        txt += (ud[:40] + (["... (%d more)" % (len(ud) - 40)] if len(ud) > 40 else [])) if ud else ["(empty)"]
        txt += [""] + levers + ["", "check: python3 tools/check_one.py <edited copy of %s> %s" % (os.path.join(w, "f.c"), name)]
        open(os.path.join(OUT, name + ".txt"), "w").write("\n".join(txt) + "\n")
        print("%-40s %s asm=%d cat=%s hunks=%d %s" % (name, "MATCH" if verdict.startswith("MATCH") else "diff ", len(asm), cat, len(levers), "EMPTY" if not ud else ""))
    shutil.copytree(raw, os.path.join(OUT, "raw"), dirs_exist_ok=True)


if __name__ == "__main__":
    main()
