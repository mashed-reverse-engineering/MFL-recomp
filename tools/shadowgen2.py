#!/usr/bin/env python3
"""shadowgen2.py <in dir (src/automatch_near)> <out dir> [names.txt] [--changed [--settled]]: shadow build of tier 2 (plain cdecl / stdcall functions).
Every definition `RET [conv] F(PARAMS)` in the an_*.c files becomes `F_c` (our C version) and a wrapper F is added that, for the first SHADOW_CALLS calls after the race has
started, runs the ORIGINAL (through a trampoline = its first whole instructions + jmp back), restores the data section snapshot, runs F_c, and logs the differences of the
return value and of the changed words (src/harness_shadow/shadow.c, log dumped by race_script.sh, report: wine-run/shadow_report.py <log> <out>/shadow_names.txt).
Not covered (written to <out>/shadow_skipped.txt): functions whose first instructions cannot be moved, 64 bit returns, function pointer / array / varargs parameters.
Heap objects are only compared in 0x200 byte windows at pointer arguments; a function with heap side effects runs them twice (it can crash the test: bisect with names.txt)."""
import os, re, shutil, subprocess, sys, tempfile
HERE = os.path.dirname(os.path.abspath(__file__)); ROOT = os.path.join(HERE, "..")
EXE = os.path.join(ROOT, "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")
# --changed (anywhere after <out dir>; combinable with names.txt): only the tier 2 functions whose source hash differs from output/shadow_cache.csv or whose last result was not OK (tools/shadow_cache.py)
args = [x for x in sys.argv[1:] if x not in ("--changed", "--settled")]
src, out = args[0], args[1]
only = {l.strip() for l in open(args[2])} if len(args) > 2 else None
if "--changed" in sys.argv:
    sys.path.insert(0, HERE); import shadow_cache
    ch = set(shadow_cache.changed(only, "--settled" in sys.argv)); print("--changed: %d of %d candidate functions need a shadow re-wrap" % (len(ch), len(only) if only is not None else len(shadow_cache.hashes())))
    only = ch
sym = {}
for l in open(os.path.join(ROOT, "src", "symbols.txt")):
    p = l.split()
    if len(p) >= 2: sym.setdefault(p[0], int(p[1], 16))
MASK = {"byte": 0xff, "char": 0xff, "uchar": 0xff, "undefined": 0xff, "undefined1": 0xff, "bool": 0xff, "short": 0xffff, "ushort": 0xffff, "undefined2": 0xffff}

def tramp(addr):
    with open(EXE, "rb") as f: f.seek(addr - 0x400000); code = f.read(32)
    t = tempfile.NamedTemporaryFile(delete=False); t.write(code); t.close()
    dis = subprocess.run(["objdump", "-w", "-D", "-b", "binary", "-mi386", "-M", "intel", t.name], capture_output=True, text=True).stdout
    os.unlink(t.name)
    n = 0
    for l in dis.splitlines():
        m = re.match(r"\s*([0-9a-f]+):\s+((?:[0-9a-f]{2} )+)\s*(\w+)\s*(.*)", l)
        if not m: continue
        op, args = m.group(3), m.group(4)
        if op.startswith("j") or op in ("call", "ret", "retn", "loop", "int3", "hlt") or "eip" in args or "(bad)" in l: return None
        n += len(m.group(2).split())
        if n >= 5: return code[:n]
    return None

os.makedirs(out, exist_ok=True)
for f in os.listdir(out):
    if f.endswith((".c", ".h", ".txt")): os.remove(os.path.join(out, f))
names, skipped, hdr = [], [], []
DEF = re.compile(r"^(?!extern|/\*|typedef|static)([A-Za-z_][\w \*]*?)\b(\w+)\((.*)\)$")
for fn in sorted(os.listdir(src)):
    if not fn.endswith((".c", ".h")): continue
    if fn.endswith(".h"): shutil.copy(os.path.join(src, fn), os.path.join(out, fn)); continue
    lines = open(os.path.join(src, fn), errors="replace").read().split("\n")
    res, i = [], 0
    while i < len(lines):
        l = lines[i]; m = DEF.match(l)
        if m and i + 1 < len(lines) and lines[i + 1].startswith("{") and m.group(2) in sym and (only is None or m.group(2) in only):
            pre, name, params = m.group(1), m.group(2), m.group(3).strip()
            conv = "__stdcall" if "__stdcall" in pre else "__fastcall" if "__fastcall" in pre or "__thiscall" in pre else "__cdecl"
            ret = re.sub(r"\b__(stdcall|cdecl|fastcall|thiscall)\b", "", pre).strip()
            plist = [] if params in ("", "void") else [x.strip() for x in params.split(",")]
            why = None
            if conv == "__fastcall" or "(" in params or "[" in params or "..." in params: why = "parameters not expressible"
            elif re.match(r"(longlong|ulonglong|undefined8|double|float10)$", ret) and False: why = ""
            elif re.match(r"(longlong|ulonglong|undefined8)$", ret): why = "64 bit return"
            args = [re.search(r"(\w+)$", p).group(1) if re.search(r"(\w+)$", p) else None for p in plist]
            if not why and (None in args or any(a == p for a, p in zip(args, plist))): why = "unnamed parameter"
            tb = tramp(sym[name]) if not why else None
            if not why and tb is None: why = "no movable prologue"
            if why: skipped.append("%s\t%s" % (name, why)); res.append(l); i += 1; continue
            fid = len(names); names.append(name)
            isvoid = ret == "void"; isf = ret in ("float", "double", "float10")
            a = ", ".join(args)
            bits = ["0"] * 4
            for k, p in enumerate(plist[:4]):
                t = p[:p.rfind(args[k])].strip()
                if "*" not in t and not t.endswith("Ptr"): continue      # only typed pointers get a memory window (an int / float that looks like an address would fault)
                bits[k] = "(*(unsigned *)&(%s))" % args[k] if t == "float" else "((unsigned)(%s))" % args[k]
            mk = MASK.get(ret)
            def raw(v): return ("(*(unsigned *)&%s)" % v if False else "((unsigned)(%s)%s)" % (v, " & 0x%x" % mk if mk else ""))
            res.append(l.replace(name + "(", name + "_c(", 1))
            j = i + 1; depth = 0
            while True:      # copy the body
                res.append(lines[j]); depth += lines[j].count("{") - lines[j].count("}"); j += 1
                if depth == 0 and j > i + 1: break
            W = ["", "typedef %s (%s *%s_fn)(%s);" % (ret, conv, name, params or "void"), "extern void %s_tramp(void);" % name,
                 "extern int shadow_take(int); extern volatile int shadow_depth; extern void shadow_begin(unsigned, unsigned, unsigned, unsigned);",
                 "extern void shadow_save1(void); extern void shadow_restore0(void); extern void shadow_restore1(void); extern void shadow_compare(int, unsigned, unsigned, int);",
                 "%s %s_c(%s);" % (conv + " " + ret if conv != "__cdecl" else ret, name, params or "void") if False else "",
                 "__declspec(naked) void %s_tramp(void)\n{\n __asm {\n" % name + "".join("  _emit 0x%02x\n" % b for b in tb) + "  push 0x%x\n  ret\n }\n}" % (sym[name] + len(tb)),
                 "%s %s %s(%s)" % (ret, conv, name, params or "void"), "{"]
            if not isvoid: W.append("    %s ra;\n    %s rb;" % (ret, ret))
            if isf: W.append("    float fa, fb;")
            W.append(("    if (!shadow_take(%d)) { " + ("" if isvoid else "return ") + "F_C(%s);" + (" return; }" if isvoid else " }")).replace("F_C", name + "_c") % (fid, a))
            W.append("    shadow_depth = 1;")
            W.append("    shadow_begin(%s);" % ", ".join(bits))
            W.append("    %s((%s_fn)%s_tramp)(%s);" % ("" if isvoid else "ra = ", name, name, a))
            W.append("    shadow_save1(); shadow_restore0();")
            W.append("    %s%s_c(%s);" % ("" if isvoid else "rb = ", name, a))
            if isvoid: W.append("    shadow_compare(%d, 0, 0, 0);" % fid)
            elif isf: W.append("    fa = (float)ra; fb = (float)rb;\n    shadow_compare(%d, *(unsigned *)&fa, *(unsigned *)&fb, 1);" % fid)
            else: W.append("    shadow_compare(%d, %s, %s, 0);" % (fid, raw("ra"), raw("rb")))
            W += ["    shadow_restore1();", "    shadow_depth = 0;"]
            if not isvoid: W.append("    return ra;")
            W += ["}", ""]
            res += W; i = j; continue
        res.append(l); i += 1
    open(os.path.join(out, fn), "w").write("\n".join(res))
rt = open(os.path.join(ROOT, "src", "harness_shadow", "shadow.c")).read()      # own copy of the runtime; EARLY=1: test from process start (loading / menu calls too), default: the script pokes shadow_enabled when the race runs
if os.environ.get("EARLY"): rt = rt.replace("volatile int shadow_enabled = 0;", "volatile int shadow_enabled = 1;")
open(os.path.join(out, "an_shadow_rt.c"), "w").write(rt)
open(os.path.join(out, "shadow_names.txt"), "w").write("\n".join(names) + "\n")
open(os.path.join(out, "shadow_skipped.txt"), "w").write("\n".join(skipped) + "\n")
print("%d functions wrapped, %d skipped" % (len(names), len(skipped)))
