#!/usr/bin/env python3
"""shadow_cache.py: tier 2 shadow results keyed by the hash of the function's source (output/shadow_cache.csv: name,hash,result).
  shadow_cache.py changed [names.txt]  prints the tier 2 functions (src/automatch_near/*.c definitions, optionally only those in names.txt) that need a shadow re-wrap:
                                       hash differs from the cache, no entry, or last result not clean.  clean = OK (--settled: also CALLED / UNTESTED, i.e. the same
                                       scenarios would only repeat the result).  Used by `shadowgen2.py <in> <out> --changed`.
  shadow_cache.py record <table>       store the current hash + result of every line of a shadow_merge.py table (name STATUS ...); run via `shadow_merge.py ... --record`
                                       only after a shadow run of the CURRENT sources.
  shadow_cache.py --selftest
Hash = sha1 of the definition text (signature to closing brace, whitespace trimmed) in the tier 2 source: a changed header / typedef is not seen (use a full shadow at milestones)."""
import hashlib, os, re, sys
HERE = os.path.dirname(os.path.abspath(__file__)); ROOT = os.path.join(HERE, "..")
CSV = os.path.join(ROOT, "output", "shadow_cache.csv")
DEF = re.compile(r"^(?!extern|/\*|typedef|static)([A-Za-z_][\w \*]*?)\b(\w+)\((.*)\)$")      # same as shadowgen2.py

def hashes(src=os.path.join(ROOT, "src", "automatch_near")):
    h = {}
    for fn in sorted(os.listdir(src)):
        if not fn.endswith(".c") or fn == "an_shadow_rt.c": continue
        L = open(os.path.join(src, fn), errors="replace").read().split("\n"); i = 0
        while i < len(L):
            m = DEF.match(L[i])
            if m and i + 1 < len(L) and L[i + 1].startswith("{"):
                j, d = i + 1, 0
                while True:
                    d += L[j].count("{") - L[j].count("}"); j += 1
                    if d == 0 or j >= len(L): break
                h[m.group(2)] = hashlib.sha1("\n".join(x.rstrip() for x in L[i:j]).encode()).hexdigest()[:12]
                i = j; continue
            i += 1
    return h

def load():
    r = {}
    if os.path.exists(CSV):
        for l in open(CSV).read().split("\n")[1:]:
            if l: n, h, s = l.split(","); r[n] = (h, s)
    return r

def changed(only=None, settled=False, cur=None, cache=None):
    cur = hashes() if cur is None else cur; cache = load() if cache is None else cache
    ok = ("OK", "CALLED", "UNTESTED") if settled else ("OK",)
    return [n for n in sorted(cur) if (only is None or n in only) and not (n in cache and cache[n][0] == cur[n] and cache[n][1] in ok)]

def record(table, cur=None):
    cur = hashes() if cur is None else cur; cache = load()
    for l in open(table):
        p = l.split()
        if len(p) >= 2 and p[0] in cur: cache[p[0]] = (cur[p[0]], p[1])
    os.makedirs(os.path.dirname(CSV), exist_ok=True)
    open(CSV, "w").write("name,hash,result\n" + "".join("%s,%s,%s\n" % (n, h, s) for n, (h, s) in sorted(cache.items())))
    return len(cache)

if __name__ == "__main__":
    a = sys.argv[1:]
    if a[:1] == ["--selftest"]:
        cur = {"a": "1", "b": "2", "c": "3", "d": "4"}; ca = {"a": ("1", "OK"), "b": ("9", "OK"), "c": ("3", "DIFF"), "d": ("4", "UNTESTED")}
        assert changed(cur=cur, cache=ca) == ["b", "c", "d"] and changed(cur=cur, cache=ca, settled=True) == ["b", "c"] and changed({"a", "c"}, cur=cur, cache=ca) == ["c"]
        print("ok"); sys.exit()
    s = "--settled" in a; a = [x for x in a if x != "--settled"]
    if a[:1] == ["changed"]:
        only = {l.strip() for l in open(a[1])} if len(a) > 1 else None
        print("\n".join(changed(only, s)))
    elif a[:1] == ["record"] and len(a) == 2: print("%d entries -> output/shadow_cache.csv" % record(a[1]))
    else: sys.exit(__doc__)
