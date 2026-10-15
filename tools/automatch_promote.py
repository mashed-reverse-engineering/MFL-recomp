#!/usr/bin/env python3
"""Turn the byte identical autocompiled functions into source files: src/automatch/am_<chunk>.c holds the whole chunk with the bodies of the functions that are NOT byte identical
replaced by an `extern` declaration of their signature (calls to them go to the original code), so only verified functions are compiled.
Usage: automatch_promote.py <autocompile dir> <automatch.csv from automatch.py>     (rewrites src/automatch/, src/automatch_names.txt = matched function list with VA)"""
import csv, glob, os, re, shutil, sys
TIER2 = "--tier2" in sys.argv
ONLY = None
if "--names" in sys.argv:          # only these functions (a file with one name per line): the survivors of a group search
    ONLY = {l.strip() for l in open(sys.argv[sys.argv.index("--names") + 1]) if l.strip()}
argv = [a for i, a in enumerate(sys.argv) if a != "--tier2" and a != "--names" and not (i > 0 and sys.argv[i - 1] == "--names")]
d, csvp = argv[1], argv[2]
R = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "src")
OUT = os.environ.get("PROMOTE_OUT") or os.path.join(R, "automatch_near" if TIER2 else "automatch")      # PROMOTE_OUT=<dir>: a separate candidate set (tools/tier2_census.py), src/ is not touched        # tier 2: compiled but not byte identical (behaviour checked by the sweeps / conformance tests only)
os.makedirs(OUT, exist_ok=True)
for old in glob.glob(os.path.join(OUT, "a[mn]_*.c")): os.unlink(old)      # files of an earlier promotion would define functions twice
for h in ("prelude.h", "ghidra_types.h"):
    shutil.copy(os.path.join(d, h), os.path.join(OUT, h))
import glob
have = set()
if "--tier2" in sys.argv:       # tier 2 never takes a function that tier 1 (byte identical) already has
    try: have |= {l.split()[1] for l in open(os.path.join(R, "automatch_names.txt"))}
    except OSError: pass          # functions already defined by hand written / harness sources: never take a second copy (overlay.py aborts on duplicates)
for g in glob.glob(os.path.join(R, "*.c")) + glob.glob(os.path.join(R, "harness*", "*.c")) + glob.glob(os.path.join(R, "bridges*", "*.c")) + glob.glob(os.path.join(R, "tier1", "*.c")) + glob.glob(os.path.join(R, "automatch_agent", "*.c")):
    have |= set(re.findall(r"^(?:__declspec\(naked\)\s+)?[A-Za-z_][\w\s\*]*?\b(\w+)\s*\([^;{}]*\)\s*(?:/\*.*?\*/)?\s*\n\s*\{", open(g, errors="replace").read(), re.M | re.S))
# names that exist at several addresses (two different functions named course_get_rpworld ...): the overlay resolves a name to ONE address, so a function that is one of them or mentions one is not taken
addr_of = {}
for l in open(os.path.join(R, "symbols.txt")):
    p = l.split()
    if len(p) == 2: addr_of.setdefault(p[0], set()).add(p[1])
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import cmpfn
def jmp_target(va):
    b = cmpfn.exe_bytes(va, 5)
    return (va + 5 + int.from_bytes(b[1:5], "little", signed=True)) & 0xFFFFFFFF if b[:1] == b"\xe9" else None
def thunk_pair(addrs):
    """all addresses are the same function seen through `jmp` thunks (Ghidra shows a thunk under the name of its target): calling any of them is the same call (overlay.py's relocation check verifies it)"""
    a = [int(x, 16) for x in addrs]
    finals = set()
    for x in a:
        seen = 0
        while seen < 4 and jmp_target(x) is not None: x = jmp_target(x); seen += 1
        finals.add(x)
    return len(finals) == 1
dup = {n for n, a in addr_of.items() if len(a) > 1 and not thunk_pair(a)}
first = {}
for l in open(os.path.join(R, "symbols.txt")):
    p_ = l.split()
    if len(p_) == 2:
        try: first.setdefault(p_[0], int(p_[1], 16))
        except ValueError: pass
matched = {}
skipped_va = []
for f, n, va, st, *_ in csv.reader(open(csvp)):
    if (st in ("STRUCTURE", "OPERANDS", "REGISTERS") if TIER2 else st == "MATCH") and n not in have and (ONLY is None or n in ONLY):
        if first.get(n) != int(va, 16):       # the overlay patches the FIRST symbols.txt address of a name: the body must belong to that function
            skipped_va.append(n); continue
        matched.setdefault(f, {})[n] = va
DEF = re.compile(r"^[A-Za-z_][\w\s\*]*?\b(\w+)\(([^;{]*)\)\s*$")
names = []
for f, ms in sorted(matched.items()):
    lines = open(os.path.join(d, f), errors="replace").read().split("\n")
    out, n = [], 0
    i = 0
    while i < len(lines):
        if lines[i] == "{":
            while out and not out[-1].strip(): out.pop()             # the decompiler leaves a blank line between signature and brace
            k = len(out)
            while k > 0 and out[k - 1].strip() and not out[k - 1].startswith(("/*", "#", "extern")): k -= 1      # the signature: the non blank lines in front of the brace (may span several lines)
            sig = out[k:]
            j = i
            while j < len(lines) and lines[j] != "}": j += 1
            m = re.search(r"(\w+)\s*\(", " ".join(x.strip() for x in sig))
            body = "\n".join(sig + lines[i:j + 1])
            name = m.group(1) if m else None
            if name and name in ms and name not in dup and not (set(re.findall(r"[A-Za-z_]\w*", body)) & dup):
                out += lines[i:j + 1]; names.append((ms[name], name, f)); n += 1
            else:
                del out[k:]
                out.append("extern " + " ".join(x.strip() for x in sig) + ";")
            i = j + 1
        else:
            out.append(lines[i]); i += 1
    if n:
        open(os.path.join(OUT, ("an_" if TIER2 else "am_") + f), "w").write("\n".join(out) + "\n")
with open(os.path.join(OUT, "names.txt") if os.environ.get("PROMOTE_OUT") else os.path.join(R, "automatch_near_names.txt" if TIER2 else "automatch_names.txt"), "w") as o:
    for va, n, f in sorted(names): o.write("%s %s %s\n" % (va, n, f))
print(len(skipped_va), "skipped (name resolves to another address):", " ".join(skipped_va[:10]))
print(len(names), "matched functions in", len([1 for f in matched if matched[f]]), "files ->", OUT)
