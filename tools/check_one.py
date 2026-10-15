#!/usr/bin/env python3
"""check_one.py <file.c> <function> [flags=/O2 ...]: compile file.c (headers next to it are copied), compare ONE function with the original code of MFL.exe.
Exit 0 = byte identical (relocation fields masked); 1 = differs (prints an assembly diff, ours '+' vs original '-'); 2 = compile error / function missing.
Meant for matching work (also by agents): edit the C of the function, run again.  Address and flags come from src/symbols.txt (override: VA=0x... env)."""
import difflib, os, re, subprocess, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import cmpfn
src, fn = sys.argv[1], sys.argv[2]
args_ = sys.argv[3:]
for a_ in [a for a in args_ if a.startswith("--va=")]:      # --va=0x... instead of the VA environment variable (for tools that cannot set env vars in front of a command)
    os.environ["VA"] = a_[5:]; args_.remove(a_)
flags = args_ or ["/O2"]
va = None
if os.environ.get("VA"): va = int(os.environ["VA"], 16)
else:
    for p in (os.path.join(os.path.dirname(os.path.abspath(src)), "addrs.txt"), os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "src", "symbols.txt")):
        if os.path.exists(p):
            for l in open(p):
                q = l.split()
                if len(q) == 2 and q[0] == fn: va = int(q[1], 16); break
        if va: break
if va is None: sys.exit("no address for %s (set VA=0x...)" % fn)
r = cmpfn.check(src, [(fn, va)], flags)
ok, det = r[fn]
if det.startswith("compile error") or "not found" in det:
    print(det); sys.exit(2)
if ok:
    print("MATCH %s %s" % (fn, det)); sys.exit(0)
if det.startswith("RELOC TARGET"):
    print("DIFFERENT " + det); sys.exit(1)
code, masked = cmpfn.LAST[fn]
gs = cmpfn.FNSZ.get(va)
orig = cmpfn.exe_bytes(va, max(len(code), gs or 0) + 4)
orig = orig[:gs] if gs else orig[:len(code) + 16]
shown = bytearray(code)
for o in masked:                      # relocated fields (call targets, global addresses) hold 0 in the object: show them as the original's bytes so only real differences remain
    if o < len(shown) and o < len(orig): shown[o] = orig[o]
def dis(b):
    t = tempfile.NamedTemporaryFile(delete=False, suffix=".bin"); t.write(bytes(b)); t.close()
    out = subprocess.run(["objdump", "-w", "-D", "-b", "binary", "-mi386", "-M", "intel", t.name], capture_output=True, text=True).stdout
    os.unlink(t.name)
    rows = []
    for l in out.splitlines():
        q = re.match(r"\s*([0-9a-f]+):\s+((?:[0-9a-f]{2} )+)\s*(.*)", l)
        if q: rows.append(q.group(3).strip())
    return rows
a, b = dis(shown), dis(orig)
print("lengths: ours %d, original %d (Ghidra size %s), relocated fields masked" % (len(code), len(orig), gs))
for l in difflib.unified_diff(b, a, "original", "ours", lineterm="", n=2): print(l)
# callee dossier (idea from REA's function dossiers): what is known about every direct callee of the ORIGINAL function: register arguments (usercall: not expressible in tier 1 C), `ret N`, declaration, fixer flags
H = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
def load_names(fn):
    p = os.path.join(H, "names", fn)
    return {l.split()[0] for l in open(p) if l.strip() and not l.startswith("#")} if os.path.exists(p) else set()
t = tempfile.NamedTemporaryFile(delete=False, suffix=".bin"); t.write(bytes(orig)); t.close()
od = subprocess.run(["objdump", "-w", "-D", "-b", "binary", "-mi386", "-M", "intel", "--adjust-vma=0x%x" % va, t.name], capture_output=True, text=True).stdout
os.unlink(t.name)
calls = []
for l in od.splitlines():
    q = re.search(r"\b(?:call|jmp)\s+0x([0-9a-f]+)", l)
    if q and int(q.group(1), 16) not in calls and not (va <= int(q.group(1), 16) < va + len(orig)): calls.append(int(q.group(1), 16))      # direct calls and tail jumps out of the function
if calls:
    by_addr = {}
    for l in open(os.path.join(H, "src", "symbols.txt")):
        pp = l.split()
        if len(pp) == 2:
            try: by_addr.setdefault(int(pp[1], 16), pp[0])
            except ValueError: pass
    regin = {}
    for l in open(os.path.join(H, "output", "reginputs.csv")):
        pp = l.strip().split(",")
        if len(pp) == 4 and pp[0] != "name": regin[int(pp[1], 16)] = (pp[2], pp[3])
    decl = {}
    for l in open(os.path.join(H, "output", "decls.h"), errors="replace"):
        m = re.match(r"/\*([0-9a-f]{8})\*/ (extern .*)", l)
        if m: decl[int(m.group(1), 16)] = m.group(2)
    wide, conv = load_names("MFL_ret_wide.txt"), {l.split()[0]: l.split()[1] for l in open(os.path.join(H, "names", "MFL_conv_fix.txt")) if len(l.split()) == 2 and not l.startswith("#")} if os.path.exists(os.path.join(H, "names", "MFL_conv_fix.txt")) else {}
    print("callees of the original (register arguments = usercall, cannot be written in plain C):")
    for a in calls:
        nm = by_addr.get(a, "?")
        ri, rn = regin.get(a, ("", ""))
        print("  %08x %-34s %s%s%s%s" % (a, nm, ("REGISTER ARGS: " + ri + "  ") if ri else "", ("ret %s  " % rn) if rn not in ("", "0") else "", decl.get(a, "")[:90], ("  [ret_wide]" if nm in wide else "") + ("  [conv fix: %s]" % conv[nm] if nm in conv else "")))
sys.exit(1)
