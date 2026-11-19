#!/usr/bin/env python3
"""layout_lint.py [--csv F] [--top N] [--selftest] [files...]: read-only lint for the b7 / b27 class (W15b CLS-LAY): a LOOSE local (not a member of a
`struct { .. } frame;`) whose address reaches more bytes than the local owns.  The original keeps its locals as one block in a fixed order and the
code reads the neighbours through that address (car_world_collision_test: 16 words copied from local_40 into the event record); ours declares the
neighbours separately, so cl lays them out in its own order and the read gets other locals / stack garbage.
Reach of a local x (bytes from &x):
  index   `(&x)[k]`, `((T *)&x)[k]`, `*(T *)((int)&x + c)`, array `x[k]` / `x + k` (k a literal, or a variable whose loop bound `k < K` is a literal)
  call    `&x` / array `x` passed to a callee: read / write extent of the ORIGINAL callee through that argument (stack_lint.extents: the same
          in-process capstone scan stack_lint class A uses; no objdump); memcpy / memmove / memset with a literal size
Original side: the Ghidra names local_N / *Stack_N are the original's frame offsets, so the over-read range names the original neighbours.
  high    the range covers other Ghidra-named locals that ours declares separately (the b27 form; fix = one struct in the original's order)
  medium  Ghidra-named x, no named neighbour in the range (a gap in the original's frame: its own stack garbage, rule 55 first)
  low     readable-named x (offset unknown: read the original's frame, rule 54)
Gaps stack_lint leaves: class A bounds a loose local_N by the FRAME TOP (assumes the original's layout), class M only checks frame struct members.
Output: ranked CSV (default output/layout_lint.csv, or $LAYOUTLINT_OUT / --csv) + the top N rows.  Warning tool, exit 0."""
import os, sys
H = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, H)
import stack_lint as S      # re-execs into the diffemu venv (capstone)
import csv, re
D = S.D; G = S.G
LOC = re.compile(r"^(?:local|[a-z]*Stack)_([0-9a-fA-F]+)$")
DECL = re.compile(r"^\s+((?:(?:unsigned|signed|const|volatile|struct)\s+)*[A-Za-z_]\w*)\s*(\**)\s*([A-Za-z_]\w*)\s*((?:\[\s*(?:0x[0-9a-fA-F]+|\d+)\s*\])*)\s*;", re.M)
NUM = r"(0x[0-9a-fA-F]+|-?\d+)"
COPY = {"memcpy": 2, "memmove": 2, "memset": 2, "_memcpy": 2, "_memset": 2}


def decls(body):
    """loose locals of a function body: name -> (size, elem size, is_array, Ghidra offset N or None, type)"""
    head = re.sub(r"\bstruct\s*\{.*?\}\s*\w+\s*;", "", body, flags=re.S)      # frame structs are the fixed form
    head = head[head.find("{") + 1:]
    out = {}
    for m in DECL.finditer(head):
        ty = m.group(1) + m.group(2)
        if m.group(1).split()[0] in ("return", "goto", "else") or "static" in m.group(1): continue
        es = S.tsize(ty)
        if not es: continue
        n = 1
        for d in re.findall(r"\[\s*(0x[0-9a-fA-F]+|\d+)\s*\]", m.group(4)): n *= int(d, 0)
        g = LOC.match(m.group(3))
        out[m.group(3)] = (es * n, es, bool(m.group(4)), int(g.group(1), 16) if g else None, ty)
    return out


def loop_bound(body, v):
    """largest literal K of `v < K` / `v <= K` / `v != K` in the body (exclusive bound), None = none"""
    ks, ne = [], []      # `!=` only when no `<` bound exists (a `v != 0x12` test of a reused variable is no loop bound)
    for m in re.finditer(r"\b%s\s*(<=|<|!=)\s*%s\b" % (re.escape(v), NUM), body):
        k = int(m.group(2), 0); (ne if m.group(1) == "!=" else ks).append(k + 1 if m.group(1) == "<=" else k)
    return max(ks or ne) if ks or ne else None


def index_reach(body, x, info):
    """(lo, hi, site text) byte ranges reached from &x by indexing in the body"""
    size, es, arr, _, ty = info; out = []; X = re.escape(x); body = DECL.sub("", body)      # `int x[4];` is not an index
    def idx(k, elem, base=0):
        if re.fullmatch(NUM, k.strip()): i = int(k.strip(), 0); return (base + i * elem, base + (i + 1) * elem)
        if re.fullmatch(r"\w+", k.strip()):
            K = loop_bound(body, k.strip())
            if K: return (base, base + K * elem)
        return None
    pats = [(r"\(\s*&%s\s*\)\s*\[([^\]]+)\]" % X, lambda m: idx(m.group(1), es if not arr else size)),
            (r"\(\s*\(\s*([\w ]+?\**)\s*\)\s*&%s\s*\)\s*\[([^\]]+)\]" % X, lambda m: idx(m.group(2), S.tsize(m.group(1)) or 0)),
            (r"\(\s*int\s*\)\s*%s%s\s*\+\s*(\w+)\b" % ("&?" if arr else "&", X), lambda m: (lambda r: r and (r[0], r[1] + (3 if re.fullmatch(NUM, m.group(1)) else 0)))(idx(m.group(1), 1)))]      # byte offset: literal = 4 byte access, loop variable = up to its bound
    if arr: pats += [(r"(?<![\w.&>])%s\s*\[([^\]]+)\]" % X, lambda m: idx(m.group(1), es)),
                     (r"(?<![\w.&>)])%s\s*\+\s*(\w+)\b(?!\s*\[)" % X, lambda m: idx(m.group(1), es))]
    for p, f in pats:
        for m in re.finditer(p, body):
            r = f(m)
            if r and r[1] > r[0] and (r[0] < 0 or r[1] > size): out.append((r[0], r[1], " ".join(m.group(0).split())))
    return out


def call_reach(L, body, x, info):
    """(lo, hi, site text) reached by callees given &x / array x"""
    size, es, arr = info[:3]; out = []
    for m in re.finditer(r"\b([A-Za-z_]\w*)\s*\(", body):
        f = m.group(1)
        if f in S.KW: continue
        j = D.balanced_after(body, m.end() - 1)
        if j < 0: continue
        args = [a.strip() for a in D.split_args(body[m.end():j]) if a.strip()]
        hit = {}      # argument index -> byte offset from &x
        for i, a in enumerate(args):
            a = S.strip_casts(a)
            if a == "&" + x or arr and a == x: hit[i] = 0
            q = arr and (re.fullmatch(r"%s\s*\+\s*%s" % (re.escape(x), NUM), a) or re.fullmatch(r"&\s*%s\s*\[\s*%s\s*\]" % (re.escape(x), NUM), a))
            if q: hit[i] = int(q.group(1), 0) * es
        if not hit: continue
        st = " ".join(body[m.start():j + 1].split())[:120]
        if f in COPY and len(args) > COPY[f] and re.fullmatch(NUM, S.strip_casts(args[COPY[f]])):
            n = int(S.strip_casts(args[COPY[f]]), 0)
            if 0 in hit and hit[0] + n > size: out.append((hit[0], hit[0] + n, st))
            continue
        va = S.va_of(L, f)
        if not va: continue
        va = va.lower(); reg = S.core.T().reg.get(int(va, 16))
        if reg and reg[0]: continue      # register-convention callee: argument slots do not map
        ex = S.extents(int(va, 16), len(args))
        for i, o in hit.items():
            n = max(ex[i])
            if o + n > size: out.append((o, o + n, st + "  [callee 0x%s %s %d]" % (va, "writes" if ex[i][1] >= ex[i][0] else "reads", n)))
    return out


def reached(fn, fva, scen):
    for k in (fn, fn[:-2] if fn.endswith("_c") else fn, "FUN_%s" % fva if fva else None):
        if k in scen: return scen[k]
    return 0


def lint(rel, text, gd, scen):
    L = D.Lint(rel, text, gd); rows = []
    for fn, params, s, e in L.functions():
        body = "\n".join(L.lines[s:e + 1]); loc = decls(body)
        for x, info in loc.items():
            sites = index_reach(body, x, info) + call_reach(L, body, x, info)
            if not sites: continue
            lo = min(r[0] for r in sites); hi = max(r[1] for r in sites); N = info[3]
            nb = []
            if N is not None:      # original address of x = top - N; y at top - M
                for y, yi in loc.items():
                    if y != x and yi[3] is not None and N - yi[3] < hi and N - yi[3] + yi[0] > lo: nb.append(y)
            sev = "high" if nb else "medium" if N is not None else "low"
            fva = D.NAMEVA.get(fn) or D.NAMEVA.get(fn[:-2] if fn.endswith("_c") else fn)
            rows.append(dict(sev=sev, reached=reached(fn, fva, scen), over=max(hi - info[0], -lo), file=rel, line=s + 1, function=fn, local=x,
                             size=info[0], reach="%d..%d" % (lo, hi), neighbours=" ".join(sorted(nb, key=lambda y: -loc[y][3])), sites=" | ".join(r[2] for r in sites[:3])))
    return rows


SELF = """void bad_block(int *p)
{
  int i;
  int local_40[4];
  float local_30;
  float local_2c;
  int local_28;
  local_40[0] = p[0]; local_30 = 1.0; local_2c = 2.0; local_28 = 3;
  i = 0;
  do { p[i] = *(local_40 + i); i = i + 1; } while (i < 0x10);
}
void bad_scalar(int *p)
{
  int local_14;
  int local_10;
  p[0] = (&local_14)[1];
}
void good_struct(int *p)
{
  int i;
  struct {
    int local_40[4];
    float local_30;
  } ev;
  i = 0;
  do { p[i] = ev.local_40[i]; i = i + 1; } while (i < 5);
}
void good_inside(int *p)
{
  int i;
  int local_40[4];
  i = 0;
  do { p[i] = local_40[i]; i = i + 1; } while (i < 4);
}
void good_bytes(int *p)
{
  int i;
  int local_20[4];
  int local_10;
  i = 0;
  do { *(int *)((int)local_20 + i) = p[i]; i = i + 4; } while (i < 0x10);
  p[0] = *(int *)((int)&local_10 + 0);
}
"""
def selftest():
    D.load_gt(); rows = lint("selftest.c", SELF, D.load_decls(), {})
    got = sorted((r["function"], r["local"], r["sev"]) for r in rows)
    assert got == [("bad_block", "local_40", "high"), ("bad_scalar", "local_14", "high")], got
    b = [r for r in rows if r["function"] == "bad_block"][0]
    assert b["reach"] == "0..64" and b["neighbours"] == "local_30 local_2c local_28", b
    print("layout_lint selftest OK")


def main():
    a = sys.argv[1:]
    if "--selftest" in a: return selftest()
    out_csv = os.environ.get("LAYOUTLINT_OUT") or os.path.join(G, "output", "layout_lint.csv"); top = 40
    if "--csv" in a: i = a.index("--csv"); out_csv = a[i + 1]; del a[i:i + 2]
    if "--top" in a: i = a.index("--top"); top = int(a[i + 1]); del a[i:i + 2]
    D.load_gt(); gd = D.load_decls()
    scen = {}
    p = os.path.join(G, "output", "fn_scenarios.csv")
    if os.path.exists(p):
        for r in csv.DictReader(open(p)): scen[r["name"]] = len([x for x in r["scenarios"].split(";") if x])
    rows = []
    for f in S.files_of([x for x in a if not x.startswith("--")]):
        rel = os.path.relpath(f, G)
        try: rows += lint(rel, D.incexpand.read_expanded(f), gd, scen)
        except Exception as e: print("layout_lint: %s: %s %s" % (rel, type(e).__name__, e), file=sys.stderr)
    rk = {"high": 0, "medium": 1, "low": 2}
    rows.sort(key=lambda r: (rk[r["sev"]], -(r["reached"] > 0), -len(r["neighbours"].split()), -r["over"], r["file"], r["line"]))
    with open(out_csv, "w", newline="") as fh:
        w = csv.DictWriter(fh, list(rows[0]) if rows else ["sev"]); w.writeheader(); w.writerows(rows)
    print("layout_lint: %d finding(s) (high %d, medium %d, low %d; race-reached %d) -> %s" % (len(rows), *(sum(r["sev"] == k for r in rows) for k in rk), sum(r["reached"] > 0 for r in rows), out_csv))
    for r in rows[:top]:
        print("%-6s sc%-3d %s:%d %s %s(%dB) reach %s nb[%s] %s" % (r["sev"], r["reached"], r["file"], r["line"], r["function"], r["local"], r["size"], r["reach"], r["neighbours"], r["sites"][:90]))
    return 0


if __name__ == "__main__": sys.exit(main())
