#!/usr/bin/env python3
"""similar_fns.py <name|va> [k=5] [--eval]: top-k byte-identical (matched) functions whose ORIGINAL machine code is most similar (TF-IDF over normalized
instruction 2-/3-grams, pure python), with the path of their C source.  Index cache output/similar_index.pkl, rebuilt when output/tier_ledger.csv or manifest changes.
--eval: leave-one-out idiom agreement of the nearest neighbour.  Import: load(), similar(name_or_va, k, exclude_self), c_source(entry, maxlines)."""
import csv, math, os, pickle, re, subprocess, sys, tempfile, hashlib
from collections import Counter
HERE = os.path.dirname(os.path.abspath(__file__)); ROOT = os.path.join(HERE, "..")
sys.path.insert(0, HERE)
LEDGER, MAN = os.path.join(ROOT, "output", "tier_ledger.csv"), os.path.join(ROOT, "src", "manifest.txt")
CACHE = os.path.join(ROOT, "output", "similar_index.pkl")
REG32 = {"eax", "ebx", "ecx", "edx", "esi", "edi"}
def norm_line(s, va, size):
    s = re.sub(r"\s*#.*|\s*<.*>", "", s).strip()
    m = re.match(r"(\w+)\s*(.*)", s)
    if not m: return None
    mn, ops = m.groups()
    if mn in ("int3", "nop") or mn.startswith("rep") and not ops: return None
    if re.match(r"(call|j\w+)$", mn) and re.fullmatch(r"0x[0-9a-f]+", ops):
        t = int(ops, 16)
        return mn + (" L" if va <= t < va + size else " F" if mn in ("call", "jmp") else " L")   # in-function target vs external callee
    def num(x):
        v = int(x.group(0), 16)
        return x.group(0) if v < 0x100 else "ADDR" if 0x400000 <= v < 0xa00000 else "IMM"
    ops = re.sub(r"0x[0-9a-f]+", num, ops)
    ops = re.sub(r"\b(?:e[abcd]x|e[sd]i)\b", "R", ops)
    ops = re.sub(r"\b(?:[abcd][lh]|[abcd]x|si|di)\b", "r8", ops)
    return mn + " " + ops
def orig_tokens(va, size):
    import cmpfn
    b = cmpfn.exe_bytes(va, size)
    t = tempfile.NamedTemporaryFile(delete=False, suffix=".bin"); t.write(b); t.close()
    out = subprocess.run(["objdump", "-w", "-D", "-b", "binary", "-mi386", "-M", "intel", "--adjust-vma=0x%x" % va, t.name], capture_output=True, text=True).stdout
    os.unlink(t.name)
    toks = []
    for l in out.splitlines():
        q = re.match(r"\s*[0-9a-f]+:\s+(?:[0-9a-f]{2} )+\s*(.*)", l)
        if q and q.group(1).strip():
            n = norm_line(q.group(1).strip(), va, size)
            if n: toks.append(n)
    return toks
def grams(toks): return Counter(" | ".join(toks[i:i + n]) for n in (2, 3) for i in range(len(toks) - n + 1))
def entries():
    man = {}
    for l in open(MAN):
        p = l.split()
        if len(p) >= 5 and p[4] == "MATCH": man[p[1]] = os.path.join("src", p[0])
    sizes = {int(l.split()[0], 16): int(l.split()[2]) for l in open(os.path.join(ROOT, "output", "fnsizes.txt")) if len(l.split()) == 3}
    es = []
    for r in csv.DictReader(open(LEDGER)):
        if not r["evidence"].startswith("bytes"): continue
        n, va = r["name"], int(r["va"], 16)
        cands = {"tier1": "src/tier1/%s.c" % n, "agent": "src/automatch_agent/ag_%s.c" % n}
        src = cands.get(r["tier"]) or man.get(n)
        if not src or not os.path.exists(os.path.join(ROOT, src)) or va not in sizes: continue
        es.append(dict(name=n, va=va, size=sizes[va], src=src))
    return es
def stamp(): return hashlib.md5(open(LEDGER, "rb").read() + open(MAN, "rb").read()).hexdigest()
def load():
    st = stamp()
    if os.path.exists(CACHE):
        d = pickle.load(open(CACHE, "rb"))
        if d["stamp"] == st: return d
    es = entries(); tf = []
    for e in es:
        try: tf.append(grams(orig_tokens(e["va"], e["size"])))
        except Exception: tf.append(Counter())
    df = Counter(g for c in tf for g in c); N = len(es)
    idf = {g: math.log((1 + N) / (1 + d)) + 1 for g, d in df.items()}
    vecs = []
    for c in tf:
        v = {g: (1 + math.log(x)) * idf[g] for g, x in c.items()}
        nr = math.sqrt(sum(x * x for x in v.values())) or 1
        vecs.append({g: x / nr for g, x in v.items()})
    d = dict(stamp=st, entries=es, vecs=vecs, idf=idf)
    pickle.dump(d, open(CACHE, "wb")); return d
def cos(a, b):
    if len(a) > len(b): a, b = b, a
    return sum(x * b.get(g, 0) for g, x in a.items())
def similar(key, k=5, d=None, vec=None):
    """key: name or va (int/hex str) of an indexed function, or vec= a prepared vector; the function itself is excluded"""
    d = d or load(); es = d["entries"]; me = None
    if vec is None:
        for i, e in enumerate(es):
            if key == e["name"] or str(key).lower().replace("0x", "").zfill(8) == "%08x" % e["va"]: me = i
        if me is None: raise KeyError("%s is not a matched function in the index" % key)
        vec = d["vecs"][me]
    sc = sorted(((cos(vec, v), i) for i, v in enumerate(d["vecs"]) if i != me), reverse=True)[:k]
    return [(s, es[i]) for s, i in sc]
def query_vec(va, size, d):
    """vector for a NOT yet matched function (original bytes of the exe)"""
    c = grams(orig_tokens(va, size)); v = {g: (1 + math.log(x)) * d["idf"][g] for g, x in c.items() if g in d["idf"]}
    nr = math.sqrt(sum(x * x for x in v.values())) or 1
    return {g: x / nr for g, x in v.items()}
def c_source(e, maxlines=60):
    """the function's C text (whole file for tier1/agent files, the function body for hand files), trimmed"""
    t = open(os.path.join(ROOT, e["src"])).read()
    if not e["src"].startswith(("src/tier1", "src/automatch_agent")):
        m = re.search(r"^[^\n;{}]*\b%s\s*\([^;{]*\)\s*\{" % re.escape(e["name"]), t, re.M)
        if m:
            dep, i = 0, m.end() - 1
            while i < len(t):
                dep += (t[i] == "{") - (t[i] == "}")
                if dep == 0: break
                i += 1
            t = t[m.start():i + 1]
    ls = [l for l in t.split("\n") if not l.startswith("#include")]
    return "\n".join(ls[:maxlines]) + ("\n/* ... trimmed */" if len(ls) > maxlines else "")
def tags(e):
    t = c_source(e, 400)
    return frozenset(n for n, p in (("loop", r"\b(for|while|do)\b"), ("switch", r"\bswitch\b"), ("float", r"\b(float|double)\b"), ("if", r"\bif\b"), ("call", r"\w+\s*\([^)]*\)\s*;"),
        ("index", r"\w\[[^\]]+\]"), ("ptrcast", r"\(\s*\w+\s*\*\s*\)"), ("member", r"->|\.\w"), ("void", r"\bvoid\s+(__\w+\s+)?\w+\s*\("), ("dllimport", r"dllimport"), ("global", r"\bg_\w+")) if re.search(p, t))
def evaluate():
    import random
    d = load(); es = d["entries"]; T = [tags(e) for e in es]; rnd = random.Random(1)
    jac = lambda a, b: len(a & b) / (len(a | b) or 1)
    best = []
    for i in range(len(es)):
        s, e = similar(es[i]["name"], 1, d)[0]; best.append((s, i, es.index(e)))
    nn = [jac(T[i], T[j]) for _, i, j in best]; rb = [jac(T[i], T[rnd.randrange(len(es))]) for _, i, _ in best]
    same = lambda i, j: T[i] == T[j]
    print("%d functions; NN tag-Jaccard mean %.3f vs random %.3f; identical idiom tag set: NN %.1f%% vs random %.1f%%" % (len(es), sum(nn) / len(nn), sum(rb) / len(rb),
        100 * sum(same(i, j) for _, i, j in best) / len(best), 100 * sum(same(i, rnd.randrange(len(es))) for _, i, _ in best) / len(best)))
    ss = sorted(s for s, _, _ in best); q = lambda p: ss[int(p * (len(ss) - 1))]
    print("NN cosine: min %.2f p10 %.2f p25 %.2f median %.2f p75 %.2f p90 %.2f max %.2f" % (ss[0], q(.1), q(.25), q(.5), q(.75), q(.9), ss[-1]))
    for lo, hi in ((0, .3), (.3, .5), (.5, .7), (.7, .9), (.9, 1.01)):
        g = [(jac(T[i], T[j]), same(i, j)) for s, i, j in best if lo <= s < hi]
        if g: print("  cos %.1f-%.1f: n=%3d  Jaccard %.2f  same tag set %.0f%%" % (lo, hi, len(g), sum(x for x, _ in g) / len(g), 100 * sum(y for _, y in g) / len(g)))
    print("examples (every %d-th by score):" % (len(best) // 10))
    for s, i, j in sorted(best)[len(best) // 20::len(best) // 10][:10]:
        print("  %.2f %-34s ~ %-34s tags %s | %s" % (s, es[i]["name"], es[j]["name"], sorted(T[i]), sorted(T[j])))
if __name__ == "__main__":
    a = [x for x in sys.argv[1:] if not x.startswith("--")]
    if "--eval" in sys.argv: evaluate(); sys.exit()
    k = int(a[1]) if len(a) > 1 else 5
    for s, e in similar(a[0], k): print("%.3f %-36s %08x %4d B  %s" % (s, e["name"], e["va"], e["size"], e["src"]))
