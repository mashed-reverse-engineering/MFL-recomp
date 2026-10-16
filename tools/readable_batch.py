#!/usr/bin/env python3
"""readable_batch.py <names file | fn ...> [--src-dir D] [--work W] [--agent CMD] [--prepare] [--finish] [--no-install]
Readability pass of tier 2 functions with a cheap agent (tools/readable_prompt.md; README "Readability track for tier 2").
names file: one function name per line (# comments ok).  src-dir: where the files live (default src/automatch_near; the file that defines the function is found by name).
Per function: W/<fn>/ gets before.c (frozen), work.c (agent edits this), the headers, TASK.md (prompt + dossier: callee prototypes used, struct fields, strings),
then the agent runs (--agent CMD, `{dir}` / `{task}` placeholders), then tools/readable_check.py before.c work.c decides: SAME -> work.c replaces the source file (backup <file>.orig
beside it, once) unless --no-install; otherwise the source is untouched.  --prepare: only build the dossiers; --finish: only verify / install what the agents left in W.
One line per function: KEEP | UNCHANGED | FAIL <why>.  Compiles run one after another (VC7 container); the agent runs serially (add `&` / xargs -P outside if you want more, the compile is the bottleneck)."""
import os, re, shutil, subprocess, sys, time
HERE = os.path.dirname(os.path.abspath(__file__)); ROOT = os.path.dirname(HERE)
def opt(n, d=None):
    if n in sys.argv: return sys.argv[sys.argv.index(n) + 1]
    return d
SRC = os.path.abspath(opt("--src-dir", os.path.join(ROOT, "src", "automatch_near")))
WORK = os.path.abspath(opt("--work", os.path.join(os.environ.get("TMPDIR", "/tmp/claude-%d" % os.getuid()), "readable", "work")))
AGENT = opt("--agent"); PREP = "--prepare" in sys.argv; FIN = "--finish" in sys.argv; INSTALL = "--no-install" not in sys.argv
skip = {sys.argv[i + 1] for i, a in enumerate(sys.argv) if a in ("--src-dir", "--work", "--agent")}
pos = [a for a in sys.argv[1:] if not a.startswith("--") and a not in skip]
names = []
for a in pos:
    if os.path.isfile(a): names += [l.split("#")[0].strip() for l in open(a) if l.split("#")[0].strip()]
    else: names.append(a)
DEF = r'^(?!extern|/\*|#)[A-Za-z_][^\n;(]*?\b%s\('
def find(fn):
    for f in sorted(os.listdir(SRC)):
        if f.endswith(".c") and re.search(DEF % re.escape(fn), open(os.path.join(SRC, f), errors="replace").read(), re.M): return f
def body_of(t, fn):
    m = re.search(DEF % re.escape(fn), t, re.M); return t[m.start():] if m else t
def structs(h):
    out = {}
    for m in re.finditer(r"^struct (\w+) \{\n(.*?)^\};", h, re.M | re.S): out[m.group(1)] = m.group(2)
    return out
def offsets(srcdir, ss, used):
    """byte offset of every named member of the structs (host gcc -m32 probe of ghidra_types.h; anonymous union members included); {} if gcc fails"""
    import tempfile
    probe = "#define offsetof(T,m) ((int)&((T*)0)->m)\n#include \"prelude.h\"\n#include \"ghidra_types.h\"\n"; names = []
    for s_ in used:
        for l in ss[s_].splitlines():
            m = re.search(r"(\w+)(\[[^\]]*\])*;", l.split("/*")[0])
            if m and not m.group(1).startswith("_gap"): names.append((s_, m.group(1)))
    probe += "".join("const int o_%d = offsetof(struct %s,%s);\n" % (i, a, b) for i, (a, b) in enumerate(names))
    d = tempfile.mkdtemp(prefix="off_", dir=WORK if os.path.isdir(WORK) else None)
    try:
        for h in ("prelude.h", "ghidra_types.h"): shutil.copy(os.path.join(srcdir, h), d)
        open(os.path.join(d, "o.c"), "w").write(probe)
        r = subprocess.run(["gcc", "-m32", "-D__int64=long long", "-S", "-w", "-o", "o.s", "o.c"], cwd=d, capture_output=True, text=True)
        if r.returncode: return {}
        asm = open(os.path.join(d, "o.s")).read(); out = {}
        for m in re.finditer(r"^o_(\d+):\n(?:\s+\.(?:size|type|align|globl)[^\n]*\n)*\s+\.(long|zero)\s*(\d*)", asm, re.M):
            out[names[int(m.group(1))]] = 0 if m.group(2) == "zero" else int(m.group(3) or 0)
        return out
    except Exception: return {}
    finally: shutil.rmtree(d, ignore_errors=True)
def dossier(fn, text, hdr, srcdir=None):
    b = body_of(text, fn); ids = set(re.findall(r"[A-Za-z_]\w*", b)); L = ["# Dossier for %s\n" % fn]
    ext = [l for l in text.splitlines() if l.startswith(("extern", "/*0")) and "extern" in l]
    protos, glob_ = [], []
    for l in ext:
        nm = re.findall(r"(\w+)\s*(?:\(|\[\d*\]|;)", l)
        if nm and nm[0] in ids: (protos if "(" in l else glob_).append(l.strip())
    L.append("## Callees (prototypes; use their names and parameter names to name your variables)\n" + ("\n".join(protos) or "(none)"))
    L.append("\n## Globals used\n" + ("\n".join(glob_) or "(none)"))
    ss = structs(hdr); used = [s for s in ss if re.search(r"\b%s\b" % s, b + "\n".join(protos + glob_))]
    for u in list(used):      # one level of member structs (Car -> Wheel, ContactPoint)
        used += [x for x in ss if x not in used and re.search(r"\bstruct %s\b" % x, ss[u])]
    L.append("\n## Struct fields (ghidra_types.h) of the structs named in the function or its externs")
    for s in used[:10]: L.append("struct %s {\n%s}" % (s, "\n".join(ss[s].splitlines()[:80])))
    if not used: L.append("(none)")
    off = offsets(srcdir, ss, used[:10]) if srcdir else {}
    if off:
        L.append("\n## Member offsets (byte offset -> member; use to turn *(T *)((char *)p + 0x2c) into p->member; array members: element = offset + 4*i)")
        for s_ in used[:10]: L.append("struct %s: " % s_ + ", ".join("%s +0x%x" % (b, v) for (a, b), v in sorted(off.items(), key=lambda kv: kv[1]) if a == s_))
    L.append("\n## String literals\n" + ("\n".join(sorted(set(re.findall(r'"(?:[^"\\\n]|\\.)*"', b)))) or "(none)"))
    L.append("\n## Known enums (names/MFL_enums.txt; compare with the integer constants in the body, #define the names you use as (value))")
    en = os.path.join(ROOT, "names", "MFL_enums.txt"); cur = None; rows = {}
    if os.path.exists(en):
        for l in open(en, errors="replace"):
            q = l.strip().split(",")
            if q[0] == "enum": cur = q[1]; rows[cur] = []
            elif q[0] == "v" and cur and len(q) >= 3: rows[cur].append("%s=%s" % (q[1], q[2]))
    for k, v in rows.items(): L.append("%s: %s" % (k, " ".join(v)))
    return "\n".join(L) + "\n"
def run_check(wd):
    r = subprocess.run([sys.executable, os.path.join(HERE, "readable_check.py"), os.path.join(wd, "before.c"), os.path.join(wd, "work.c")], capture_output=True, text=True, timeout=1800)
    return r.returncode, (r.stdout + r.stderr).strip()
for fn in names:
    f = find(fn)
    if not f: print("FAIL %s not defined in %s" % (fn, SRC)); continue
    wd = os.path.join(WORK, fn); src = os.path.join(SRC, f); t0 = time.time()
    if not FIN:
        os.makedirs(wd, exist_ok=True)
        for h in os.listdir(SRC):
            if h.endswith(".h"): shutil.copy(os.path.join(SRC, h), wd)
        shutil.copy(src, os.path.join(wd, "before.c")); shutil.copy(src, os.path.join(wd, "work.c"))
        text = open(src, errors="replace").read(); hdr = open(os.path.join(SRC, "ghidra_types.h"), errors="replace").read()
        open(os.path.join(wd, "DOSSIER.md"), "w").write(dossier(fn, text, hdr, SRC))
        task = os.path.join(wd, "TASK.md")
        open(task, "w").write(open(os.path.join(HERE, "readable_prompt.md")).read().replace("python3 tools/readable_check.py", "python3 %s/tools/readable_check.py" % ROOT) + "\n---\nFunction: %s\nDirectory: %s (work.c, before.c, DOSSIER.md; run the check with cwd = this directory)\n\n" % (fn, wd) + open(os.path.join(wd, "DOSSIER.md")).read())
        if PREP: print("PREPARED %s %s" % (fn, wd)); continue
        if AGENT: subprocess.run(AGENT.format(dir=wd, task=task, fn=fn, root=ROOT), shell=True, cwd=wd, timeout=1800)
    if not os.path.exists(os.path.join(wd, "work.c")): print("FAIL %s no work.c" % fn); continue
    if open(os.path.join(wd, "work.c")).read() == open(os.path.join(wd, "before.c")).read(): print("UNCHANGED %s" % fn); continue
    rc, out = run_check(wd)
    if rc != 0:
        ls = out.splitlines(); why = next((l for l in ls if l.startswith("DIFF") or "error" in l), ls[-1] if ls else "?")
        print("FAIL %s %s" % (fn, why[:300])); continue
    if INSTALL:
        if open(src).read() != open(os.path.join(wd, "before.c")).read(): print("FAIL %s source changed since prepare, not installed" % fn); continue
        if not os.path.exists(src + ".orig"): shutil.copy(src, src + ".orig")
        shutil.copy(os.path.join(wd, "work.c"), src)
    print("KEEP %s (%s, %.0fs)" % (fn, "installed" if INSTALL else "verified only", time.time() - t0))
