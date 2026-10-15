#!/usr/bin/env python3
"""propagate_dups.py [--out DIR] [--max N] [--install]
Duplicate propagation (BFM-decomp wiki: "multipliers"): finds UNMATCHED functions whose normalised instruction sequence (tools/similar_fns.py tokens) AND size equal those of a byte identical
function with a standalone source (src/tier1/<fn>.c, src/automatch_agent/ag_<fn>.c), copies that source under the new name, replaces the globals / callees / constants that differ
(taken by position from the operands of the two original code blobs, names from src/symbols.txt, declarations from output/decls.h) and runs the byte gate (cmpfn.check, /O2).
Only the byte gate claims a match.  Passing candidates are written to DIR/<fn>.c (default scratchpad-free: output/propagate/) and listed in DIR/found.txt (new, source, state);
--install copies them to src/automatch_agent/ag_<fn>.c (tier 1; verify with tools/agent_verify.py, then check_all.sh)."""
import collections, os, pickle, re, shutil, subprocess, sys, tempfile
from concurrent.futures import ThreadPoolExecutor
HERE = os.path.dirname(os.path.abspath(__file__)); ROOT = os.path.join(HERE, "..")
sys.path.insert(0, HERE)
import cmpfn, similar_fns as S

R = os.path.join(ROOT, "src")
arg = lambda k, d=None: sys.argv[sys.argv.index(k) + 1] if k in sys.argv else d
OUT = os.path.abspath(arg("--out", os.path.join(ROOT, "output", "propagate")))
MAXN = int(arg("--max", "100000"))
SYM, BYADDR = {}, collections.defaultdict(list)
for l in open(os.path.join(R, "symbols.txt")):
    p = l.split()
    if len(p) == 2:
        try: v = int(p[1], 16)
        except ValueError: continue
        SYM.setdefault(p[0], set()).add(v); BYADDR[v].append(p[0])
DECL = {}
for l in open(os.path.join(ROOT, "output", "decls.h"), errors="replace"):
    m = re.match(r"/\*([0-9a-f]{8})\*/ (extern .*)", l)
    if m:
        mm = re.search(r"([A-Za-z_]\w*)\s*(?:\(|\[|;)", re.sub(r"__declspec\([^)]*\)", "", m.group(2)).split("(")[0] + ("(" if "(" in m.group(2) else ";") if "(" in m.group(2) else m.group(2))
        nm = re.search(r"(\w+)\s*(?:\[[^\]]*\])?\s*;\s*$", m.group(2)) if "(" not in m.group(2) else re.search(r"(\w+)\s*\(", re.sub(r"__declspec\([^)]*\)", "", m.group(2)))
        if nm: DECL.setdefault(nm.group(1), "/*%s*/ %s" % (m.group(1), m.group(2)))
sizes = {int(l.split()[0], 16): (l.split()[1], int(l.split()[2])) for l in open(os.path.join(ROOT, "output", "fnsizes.txt")) if len(l.split()) == 3}


def dis(va, size):
    b = cmpfn.exe_bytes(va, size)
    t = tempfile.NamedTemporaryFile(delete=False, suffix=".bin"); t.write(b); t.close()
    out = subprocess.run(["objdump", "-w", "-D", "-b", "binary", "-mi386", "-M", "intel", "--adjust-vma=0x%x" % va, t.name], capture_output=True, text=True).stdout
    os.unlink(t.name)
    rows = []
    for l in out.splitlines():
        q = re.match(r"\s*[0-9a-f]+:\s+(?:[0-9a-f]{2} )+\s*(.*)", l)
        if q and q.group(1).strip(): rows.append(re.sub(r"\s*#.*|\s*<.*>", "", q.group(1)).strip())
    return rows


def operands(rows, va, size):
    """per instruction: the list of hex numbers, internal branch targets dropped"""
    out = []
    for r in rows:
        nums = re.findall(r"0x[0-9a-f]+", r)
        if re.match(r"(call|j\w+)\s", r) and nums and va <= int(nums[0], 16) < va + size: nums = nums[1:]
        out.append([int(x, 16) for x in nums])
    return out


def index_unmatched():
    cp = os.path.join(ROOT, "output", "propagate_tokens.pkl")      # token cache of the ORIGINAL code: independent of our sources, valid as long as the exe is
    cache = pickle.load(open(cp, "rb")) if os.path.exists(cp) else {}
    for va, (n, sz) in sizes.items():
        if va not in cache and 6 <= sz <= 1500:
            try: cache[va] = tuple(S.orig_tokens(va, sz))
            except Exception: pass
    pickle.dump(cache, open(cp, "wb"))
    return cache


def make_candidate(M, U_va, U_name, toks_ok=True):
    """-> C text or None"""
    mva, mname, msize, src = M["va"], M["name"], M["size"], os.path.join(ROOT, M["src"])
    rm, ru = dis(mva, msize), dis(U_va, msize)
    if len(rm) != len(ru): return None
    om, ou = operands(rm, mva, msize), operands(ru, U_va, msize)
    mp = {}
    for a, b in zip(om, ou):
        if len(a) != len(b): return None
        for x, y in zip(a, b):
            if x != y:
                if mp.setdefault(x, y) != y: return None
    text = open(src, errors="replace").read()
    text = re.sub(r"\b%s\b" % re.escape(mname), U_name, text)
    for x, y in mp.items():
        if 0x400000 <= x < 0xa00000 and (0x400000 <= y < 0xa00000):      # address: replace the symbol name (the one that occurs in the text)
            names = [n for n in BYADDR.get(x, []) if re.search(r"\b%s\b" % re.escape(n), text)]
            new = [n for n in BYADDR.get(y, []) if n in DECL]
            if not names or not new: return None
            for n in names: text = re.sub(r"\b%s\b" % re.escape(n), new[0], text)
        else:                                                              # constant: the literal in hex or decimal
            if not re.search(r"\b(0x%x|%d)\b" % (x, x), text, re.I): return None
            text = re.sub(r"\b(?:0x%x|%d)\b" % (x, x), "0x%x" % y if y > 9 else str(y), text, flags=re.I)
    # declarations of every identifier that has none yet
    have = set(re.findall(r"([A-Za-z_]\w*)\s*(?:\[[^\]]*\])?\s*;|([A-Za-z_]\w*)\s*\(", text))
    ids = set(re.findall(r"[A-Za-z_]\w*", re.sub(r"/\*.*?\*/", "", text, flags=re.S)))
    declared = set(m.group(1) for m in re.finditer(r"^/\*[0-9a-f]{8}\*/ extern .*?(\w+)\s*(?:\[[^\]]*\])?\s*;", text, re.M))
    add = [DECL[n] for n in sorted(ids) if n in DECL and n not in declared and n != U_name and not re.search(r"^[^\n#]*\b%s\s*\([^;{]*\)\s*\n?\s*\{" % re.escape(n), text, re.M)]
    inc = [l for l in text.split("\n") if l.startswith("#include")]
    EXT = re.compile(r"^/\*[0-9a-f]{8}\*/ extern .*?(\w+)\s*(?:\[[^\]]*\])?\s*;")
    ext = []
    for l in text.split("\n"):
        m = EXT.match(l)
        if m: ext.append(DECL.get(m.group(1), l))      # the address comment of a copied declaration is the old function's: take the declaration of the NEW name
    body = "\n".join(l for l in text.split("\n") if not l.startswith("#include") and not EXT.match(l) and not l.startswith("/*") and l.strip() != "")      # descriptive comments belong to the source function
    body = "/* %s: same machine code as %s (tools/propagate_dups.py), byte gate checked */\n" % (U_name, mname) + body
    return "\n".join(inc + ext + add) + "\n" + body + "\n"


def main():
    ms = [e for e in S.entries() if e["src"].startswith(("src/tier1/", "src/automatch_agent/"))]
    matched_names = {e["name"] for e in S.entries()}
    cache = index_unmatched()
    key = collections.defaultdict(list)
    for e in ms:
        t = cache.get(e["va"])
        if t: key[(t, e["size"])].append(e)
    have_def = set()
    for g in [os.path.join(R, f) for f in os.listdir(R) if f.endswith(".c")] + [os.path.join(R, d, f) for d in ("tier1", "automatch_agent", "automatch", "harness", "bridges") if os.path.isdir(os.path.join(R, d)) for f in os.listdir(os.path.join(R, d)) if f.endswith(".c")]:
        have_def |= set(re.findall(r"^(?:__declspec\(naked\)\s+)?[A-Za-z_][\w\s\*]*?\b(\w+)\s*\([^;{}]*\)\s*(?:/\*.*?\*/)?\s*\n\s*\{", open(g, errors="replace").read(), re.M | re.S))
    pairs = []
    for va, (n, sz) in sorted(sizes.items()):
        if n in matched_names or n in have_def or len(SYM.get(n, ())) != 1 or va not in SYM[n]: continue
        t = cache.get(va)
        if t and (t, sz) in key and cmpfn.exe_bytes(va, 1) != b"\xe9": pairs.append((va, n, key[(t, sz)]))
    print("%d matched sources, %d unmatched functions with an identical normalised sequence (%d candidates pairs)" % (len(ms), len(pairs), sum(len(p[2]) for p in pairs)))
    os.makedirs(OUT, exist_ok=True)
    work = tempfile.mkdtemp(prefix="prop_", dir=cmpfn.WORKROOT)

    def one(p):
        va, n, Ms = p
        for M in Ms[:6]:
            try: c = make_candidate(M, va, n)
            except Exception as e: c = None
            if not c: continue
            d = tempfile.mkdtemp(dir=work)
            f = os.path.join(d, "f.c")
            open(f, "w").write(c)
            for h in ("prelude.h", "ghidra_types.h"): shutil.copy(os.path.join(R, "automatch_agent", h), d)
            ok, det = cmpfn.check(f, [(n, va)], ["/O2"])[n]
            if ok:
                open(os.path.join(OUT, n + ".c"), "w").write(c)
                return n, M["name"], "MATCH"
        return n, Ms[0]["name"], "no candidate" if True else ""
    res = []
    with ThreadPoolExecutor(2) as ex:
        for r in ex.map(one, pairs[:MAXN]): res.append(r)
    shutil.rmtree(work, ignore_errors=True)
    with open(os.path.join(OUT, "found.txt"), "w") as o:
        for n, m, st in res:
            if st == "MATCH": o.write("%s %s\n" % (n, m))
    ok = [r for r in res if r[2] == "MATCH"]
    print("byte identical: %d of %d tried" % (len(ok), len(res)))
    if "--install" in sys.argv:
        for n, m, _ in ok:
            shutil.copy(os.path.join(OUT, n + ".c"), os.path.join(R, "automatch_agent", "ag_%s.c" % n))
        print("installed %d into src/automatch_agent/ (run tools/agent_verify.py and check_all.sh)" % len(ok))
        if subprocess.run([os.path.join(HERE, "install_check.sh")]).returncode != 0:      # a broken install must not stay in the tree
            for n, m, _ in ok: os.unlink(os.path.join(R, "automatch_agent", "ag_%s.c" % n))
            print("BUILD FAILED after the install: the %d installed files were removed again" % len(ok)); sys.exit(1)
        print("install_check.sh: tier 1 build links")


main()
