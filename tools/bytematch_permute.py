#!/usr/bin/env python3
"""bytematch_permute.py: permuter-style multi-site search that turns behaviour-verified tier-2 functions (src/automatch_near/an_*.c) into byte-identical ones.
Usage: bytematch_permute.py [--work DIR] [--names a,b|@file] [--maxdiff N] [--maxinsn N] [--gs] [--budget N] [--batch B] [--jobs J] [--seed S] [--flags "/O2"] [--list]
  Candidate selection: --names, or from the census csv (output/bytediff_causes_2026-10-29.csv, rerun tools/bytediff_causes.py first) every name with 0 < bytes_diff_count <= --maxdiff
  (--maxinsn: instruction-aligned diff <= N, computed from the census scan pickle DIR/../scan.pkl or by compiling), --gs: names whose original loads the stack cookie.
Search per function: hill climb over the source text.  A batch = B mutants (1-2 mutations each, drawn from the mutators below) of the current text, compiled in ONE translation unit as
  copies name__v0..name__vB-1 (cl keeps per-function state apart), each compared with the original bytes (cmpfn.check: relocation fields masked, RELCHECK target rules).
  Score = (instruction-aligned diff count of the normalised disassembly, differing bytes).  The best mutant of a batch replaces the current text when its score is <= the current (plateau moves
  allowed: arm order alone often changes nothing until the compare order is fixed); after 25 batches without a new best the climb restarts from the best.  --budget = distinct variants compiled per
  function (default 400, the original brief asked for up to 3000).  Everything seen is cached in DIR/cache/<name>.json (score per sha1 of the function text, best text): re-running resumes.
  Hits (byte-identical, size ok) are written to DIR/hits/<name>.c (the whole chunk with the function replaced) and listed in DIR/hits.txt; src/ is never touched.
  Install with tools/bytematch_install.py --work DIR/hitsw (hitsw/out/<chunk>) after reviewing the readability of the diff (printed).
Mutators (each `text -> [variant]`, comments are never edited): rel_swap (a<b -> b>a, mirrored operator: NaN-safe), ifelse_not (swap if/else arms with the condition written !( ) ; == / != / ! are flipped
  directly, relational operators never), mul_swap (a*b, a+b operand order, only where no precedence/float-association changes), decl_swap (two leading local declarations), stmt_swap, inline_temp,
  loop_form (for<->while, not with `continue`), case_swap (adjacent case blocks that both end in break/return/goto and follow a terminated block), cast_toggle (drop a (float)/(double)/(int) cast,
  or wrap the right-hand side of an assignment to a float/double local in (float)(double)/(double)), lit_width (1.0f <-> 1.0), regroup (a + b + c <-> a + (b + c)), type_swap (`*(undefined4 *)` <-> float/int/uint pointer cast).
Not mutated: types, semantics of pointer compares ((int)ptr casts grow portcheck), anything needing liveness (local reuse, temp introduction with a new type)."""
import difflib, hashlib, json, os, random, re, shutil, sys, threading, time
from concurrent.futures import ThreadPoolExecutor
HERE = os.path.dirname(os.path.abspath(__file__)); R = os.path.join(HERE, "..", "src")
sys.path.insert(0, HERE)
import cmpfn, asmdiff, bytediff_causes as bc, bytematch_rewrite as rw

COMMENT = re.compile(r"/\*.*?\*/|//[^\n]*", re.S)


def in_comment(t, pos):
    return any(m.start() <= pos < m.end() for m in COMMENT.finditer(t))


def first_diff(a, b):
    n = min(len(a), len(b)); i = 0
    while i < n and a[i] == b[i]: i += 1
    return i


def code_only(f):
    """wrap a text->variants function: variants whose first edit lies inside a comment are dropped"""
    def g(t):
        spans = [(m.start(), m.end()) for m in COMMENT.finditer(t)]
        out = []
        for v in f(t):
            if v == t: continue
            p = first_diff(t, v)
            if any(s <= p < e or (p == e) and False for s, e in spans): continue
            out.append(v)
        return out
    return g


def has_call(x): return bool(re.search(r"[A-Za-z_]\w*\s*\(", re.sub(r"\((?:float|double|int|short|char|unsigned|undefined\d?|void|struct \w+)[\s\*]*\)", "", x)))


def m_rel_swap(t):
    out = []
    flip = {"<": ">", ">": "<", "<=": ">=", ">=": "<="}
    for s, i, e, op, L, Rr in rw.rel_sites(t):
        if re.search(r"\|\||&&|\||\^|\?|(?<=[\w\)\]])\s*&(?!&)", L + " " + Rr): continue      # a lower-precedence operator inside the span
        if has_call(L) and has_call(Rr): continue
        out.append(t[:s] + "%s %s %s" % (Rr, flip[op], L) + t[e:])
    return out


def m_mul_swap(t):
    out = []; b = rw.body_span(t)
    for m in rw.BIN.finditer(t, b):
        op = m.group(2)
        pre = t[:m.start()].rstrip(); post = t[m.end():].lstrip()
        if pre and not (pre[-1] in "(=,;{}[?:" or pre.endswith("return")): continue      # a*b+c / a-b+c / a+b+c regrouping would change
        if post[:1] in ("*", "/", "%") and op == "+": continue
        if has_call(m.group(1)) and has_call(m.group(3)): continue
        out.append(t[:m.start()] + "%s %s %s" % (m.group(3), op, m.group(1)) + t[m.end():])
    return out


def m_ifelse_not(t):
    out = []; b = rw.body_span(t)
    for m in re.finditer(r"\bif\s*\(", t[b:]):
        i = b + m.start(); po = b + m.end() - 1; pe = rw._paren_end(t, po)
        if pe < 0: continue
        cond = t[po + 1:pe - 1].strip()
        j = pe
        while t[j] in " \n\t": j += 1
        if t[j] != "{": continue
        e1 = rw._block_end(t, j); k = e1
        while k < len(t) and t[k] in " \n\t": k += 1
        if not t.startswith("else", k): continue
        k += 4
        while t[k] in " \n\t": k += 1
        if t[k] != "{": continue
        e2 = rw._block_end(t, k)
        if e1 < 0 or e2 < 0: continue
        A, B = t[j:e1], t[k:e2]
        if re.fullmatch(r"!\s*\((.*)\)", cond, re.S) and rw._paren_end(cond, cond.index("(")) == len(cond): nc = cond[cond.index("(") + 1:-1].strip()
        elif re.fullmatch(r"!\w+", cond): nc = cond[1:]
        elif re.fullmatch(r"\w+", cond): nc = "!" + cond
        else:
            fm = re.fullmatch(r"(.+?)\s*(==|!=)\s*(.+)", cond)
            if fm and not re.search(r"[<>=!&|]", fm.group(1) + fm.group(3)): nc = "%s %s %s" % (fm.group(1), "!=" if fm.group(2) == "==" else "==", fm.group(3))
            else: nc = "!(%s)" % cond
        out.append(t[:i] + "if (%s) %s else %s" % (nc, B, A) + t[e2:])
    return out


def m_decl_swap(t):
    head, rest = rw._body_lines(t)
    n = 0
    while n < len(rest) and rw.DECL.match(rest[n]) and "," not in rest[n].split("=")[0]: n += 1
    if n < 2: return []
    names = [re.search(r"(\w+)(\[\d+\])?(\s*=|;)", d).group(1) for d in rest[:n]]
    for d in rest[:n]:
        if "=" in d and any(re.search(r"\b%s\b" % x, d.split("=", 1)[1]) for x in names): return []
    out = []
    for i in range(n):
        for j in range(i + 1, n):
            D = rest[:]; D[i], D[j] = D[j], D[i]
            out.append("\n".join(head + D))
    return out


def m_stmt_swap(t): return rw.pat_stmt_swap(t, {})
def m_inline_temp(t): return rw.pat_inline_temp(t, {})


def m_loop_form(t):
    if re.search(r"\bcontinue\b", t): return []
    return rw.pat_loop_form(t, {})


def m_case_swap(t):
    out = []
    parts = list(re.finditer(r"(?m)^(\s*)(case [^:\n]+:|default:)(.*?)(?=^\s*(?:case [^:\n]+:|default:)|^\s*\}\s*$)", t, re.S))
    term = re.compile(r"(break|return[^;]*|goto \w+);\s*$")
    for idx, (a, b) in enumerate(zip(parts, parts[1:])):
        if a.end() != b.start(): continue
        if not term.search(a.group(0).rstrip()) or not term.search(b.group(0).rstrip()): continue
        pre = t[:a.start()].rstrip()
        if not (pre.endswith("{") or term.search(pre)): continue
        out.append(t[:a.start()] + b.group(0) + a.group(0) + t[b.end():])
    return out


CASTS = re.compile(r"\((?:float|double|int)\)\s*")


def m_cast_toggle(t):
    out = []; b = rw.body_span(t)
    for m in CASTS.finditer(t, b):
        out.append(t[:m.start()] + t[m.end():])
    fl = set(re.findall(r"^\s{2}(?:float|double)\s+(\w+)\s*[;,=]", t[b:], re.M)) | set(re.findall(r"^\s{2}(?:float|double)\s+\w+\s*,\s*(\w+)", t[b:], re.M))
    for m in re.finditer(r"(?m)^(\s{2,}(\w+) = )([^;{}\n]+);", t[b:]):
        if m.group(2) not in fl or CASTS.match(m.group(3)): continue
        s = b + m.start(3); e = b + m.end(3)
        for c in ("(float)(double)", "(double)", "(float)"):
            out.append(t[:s] + "%s(%s)" % (c, t[s:e]) + t[e:])
    return out


def m_lit_width(t):
    out = []; b = rw.body_span(t)
    for m in re.finditer(r"(?<![\w.])(\d+\.\d*|\.\d+)([fF]?)(?![\w.])", t[b:]):
        s = b + m.start(); e = b + m.end()
        out.append(t[:s] + m.group(1) + ("" if m.group(2) else "f") + t[e:])
    return out


def m_regroup(t):
    out = []; b = rw.body_span(t)
    A = r"[\w.\]\[>\-]+?|\([^()]*\)"
    for m in re.finditer(r"(?<![\w.\)\]])(%s)\s*\+\s*(%s)\s*\+\s*(%s)(?![\w.\(\[*/%%])" % (A, A, A), t[b:]):
        s = b + m.start(); e = b + m.end()
        out.append(t[:s] + "%s + (%s + %s)" % m.groups() + t[e:])
    for m in re.finditer(r"(?<![\w.\)\]])(%s)\s*\+\s*\((%s)\s*\+\s*(%s)\)(?![\w.\(\[*/%%])" % (A, A, A), t[b:]):
        s = b + m.start(); e = b + m.end()
        out.append(t[:s] + "%s + %s + %s" % m.groups() + t[e:])
    return out


PTYPES = ["undefined4", "float", "int", "uint"]


def m_type_swap(t):
    """retype one dereferencing pointer cast `*(T *)` (T in undefined4/float/int/uint): a word copy as float load/store (fld/fstp), signed/unsigned compares"""
    out = []; b = rw.body_span(t)
    for m in re.finditer(r"\*\s*\((%s) \*\)" % "|".join(PTYPES), t[b:]):
        for T in PTYPES:
            if T != m.group(1):
                s_ = b + m.start(1); e_ = b + m.end(1); out.append(t[:s_] + T + t[e_:])
    return out


MUT = {"type_swap": m_type_swap, "rel_swap": m_rel_swap, "ifelse_not": m_ifelse_not, "mul_swap": m_mul_swap, "decl_swap": m_decl_swap, "stmt_swap": m_stmt_swap, "inline_temp": m_inline_temp,
       "loop_form": m_loop_form, "case_swap": m_case_swap, "cast_toggle": m_cast_toggle, "lit_width": m_lit_width, "regroup": m_regroup}
MUT = {k: code_only(v) for k, v in MUT.items()}


def mutants(t, rng, n):
    """n random (1-2 mutation) variants of t: [(text, 'mutator+mutator')]"""
    cache = {}
    def get(k):
        if k not in cache:
            try: cache[k] = MUT[k](t)
            except Exception: cache[k] = []
        return cache[k]
    live = [k for k in MUT if get(k)]
    out = {}
    tries = 0
    while len(out) < n and live and tries < n * 6:
        tries += 1
        k = rng.choice(live); v = rng.choice(get(k)); tag = k
        if rng.random() < 0.35:      # second mutation on top
            try:
                k2 = rng.choice(list(MUT)); c = MUT[k2](v)
                if c: v = rng.choice(c); tag += "+" + k2
            except Exception: pass
        if v != t: out[v] = tag
    return list(out.items())


# ---------------------------------------------------------------- compile + score
def score_of(code, masked, va):
    ob = cmpfn.exe_bytes(va, cmpfn.FNSZ.get(va) or len(code))
    nd = sum(1 for i, (a, b) in enumerate(zip(code, ob)) if a != b and i not in masked) + abs(len(code) - len(ob))
    ours = bc.norm(asmdiff.disasm(bc.fill(code, masked, ob))); org = bc.norm(asmdiff.disasm(ob))
    sm = difflib.SequenceMatcher(None, ours, org, autojunk=False)
    ni = sum(max(o[2] - o[1], o[4] - o[3]) for o in sm.get_opcodes() if o[0] != "equal")
    return ni, nd


class Ctx:
    pass


def eval_batch(cx, texts):
    """compile the function texts as copies of one TU; -> [(score|None, ok, det)]"""
    n = len(texts)
    syms = ["%s__v%d" % (cx.name, k) for k in range(n)]
    body = "\n".join(re.sub(r"\b%s\b" % re.escape(cx.name), s, t) for s, t in zip(syms, texts))
    uid = "%d_%d" % (os.getpid(), threading.get_ident())
    fp = os.path.join(cx.tmp, "p%s_%s.c" % (uid, cx.name))
    with open(fp, "w", errors="surrogateescape") as f: f.write(cx.pre + "\n" + body + "\n")
    try:
        res = cmpfn.check(fp, [(s, cx.va) for s in syms], cx.flags)
    finally:
        try: os.unlink(fp)
        except OSError: pass
    out = []
    for s in syms:
        ok, det = res[s]
        last = cmpfn.LAST.pop(s, None)
        if det.startswith("compile error") or last is None: out.append((None, False, det))
        else: out.append((score_of(last[0], last[1], cx.va), ok, det))
    if n > 1 and all(o[0] is None and o[2].startswith("compile error") for o in out):      # one bad variant kills the TU: bisect
        h = n // 2
        return eval_batch(cx, texts[:h]) + eval_batch(cx, texts[h:])
    return out


def prep(name, va, ch, src_dir, flags):
    cx = Ctx(); cx.name = name; cx.va = va; cx.chunk = ch
    if not os.path.exists(os.path.join(src_dir, ch)): return None
    t = open(os.path.join(src_dir, ch), errors="surrogateescape").read()
    ms = [m for m in rw.DEF.finditer(t)]
    m = next((m for m in ms if m.group(1) == name), None)
    if not m: return None
    cx.fn = m.group(0); cx.span = (m.start(), m.end()); cx.chunk_text = t
    cx.pre = t[:ms[0].start()]
    cx.flags = flags + (["/GS"] if "compile: /GS" in t and "/GS" not in flags else [])
    cx.tmp = os.path.join(src_dir, "_tmp"); os.makedirs(cx.tmp, exist_ok=True)
    for h in os.listdir(src_dir):      # headers next to the generated TU
        if h.endswith(".h") and not os.path.exists(os.path.join(cx.tmp, h)): shutil.copy(os.path.join(src_dir, h), cx.tmp)
    return cx


def loadwait(lm):
    while os.getloadavg()[0] > lm: time.sleep(20)


def search(cx, work, budget, B, seed, lm, log):
    cd = os.path.join(work, "cache"); os.makedirs(cd, exist_ok=True)
    cf = os.path.join(cd, cx.name + ".json")
    st = json.load(open(cf)) if os.path.exists(cf) else {"seen": {}, "best": None, "best_score": None, "hit": False}
    if st["hit"]: return st
    if st.get("full"): cx.pre = cx.chunk_text[:cx.span[0]] + "\n" + cx.chunk_text[cx.span[1]:]
    rng = random.Random(seed ^ hash(cx.name) & 0xffff)
    h = lambda s: hashlib.sha1(s.encode("utf-8", "surrogateescape")).hexdigest()[:16]
    if st["best"] is None:
        loadwait(lm); (sc, ok, det), = eval_batch(cx, [cx.fn])
        if sc is None:      # static helpers / structs defined between the functions: retry with the whole chunk (other functions kept) as the preamble
            st["full"] = True; cx.pre = cx.chunk_text[:cx.span[0]] + "\n" + cx.chunk_text[cx.span[1]:]; (sc, ok, det), = eval_batch(cx, [cx.fn])
        if sc is None: st["error"] = det[:200]; json.dump(st, open(cf, "w")); return st
        st.pop("error", None); st["seen"][h(cx.fn)] = list(sc); st["best"] = cx.fn; st["best_score"] = list(sc); st["base_score"] = list(sc); st["hit"] = ok
        log("%s base score %s" % (cx.name, sc))
    cur, cur_sc = st["best"], tuple(st["best_score"]); since = 0
    while not st["hit"] and len(st["seen"]) < budget:
        muts = [(v, tag) for v, tag in mutants(cur, rng, B * 2) if h(v) not in st["seen"]][:B]
        if not muts:
            if cur != st["best"]: cur = st["best"]; cur_sc = tuple(st["best_score"]); since = 0; continue
            if not mutants(st["best"], rng, 1): break
            since += 1
            if since > 60: break
            continue
        loadwait(lm)
        res = eval_batch(cx, [v for v, _ in muts])
        bi = None
        for (v, tag), (sc, ok, det) in zip(muts, res):
            st["seen"][h(v)] = list(sc) if sc else None
            if sc is None: continue
            if ok:
                st["hit"] = True; st["best"] = v; st["best_score"] = list(sc); st["tag"] = tag; bi = None; log("HIT %s via %s after %d variants" % (cx.name, tag, len(st["seen"]))); break
            if bi is None or sc < res[bi][0]: bi = muts.index((v, tag))
        if st["hit"]: break
        if bi is not None and res[bi][0] <= cur_sc:
            if res[bi][0] < cur_sc: since = 0
            cur, cur_sc = muts[bi][0], res[bi][0]
            if cur_sc < tuple(st["best_score"]): st["best"] = cur; st["best_score"] = list(cur_sc); log("%s -> %s (%s)" % (cx.name, cur_sc, muts[bi][1]))
        else:
            since += 1
        if since >= 25: cur = st["best"]; cur_sc = tuple(st["best_score"]); since = 0
        json.dump(st, open(cf, "w"))
    json.dump(st, open(cf, "w"))
    return st


def main():
    args = sys.argv[1:]
    def opt(k, d):
        if k in args: i = args.index(k); v = args[i + 1]; del args[i:i + 2]; return v
        return d
    work = opt("--work", os.path.join(os.environ.get("TMPDIR", "/tmp"), "bytematch_permute")); names_a = opt("--names", None); maxdiff = int(opt("--maxdiff", "0")); maxinsn = int(opt("--maxinsn", "0"))
    budget = int(opt("--budget", "400")); B = int(opt("--batch", "6")); jobs = min(3, int(opt("--jobs", "3"))); seed = int(opt("--seed", "1")); flags = opt("--flags", "/O2").split()
    lm = float(os.environ.get("LOADMAX", "9")); only_list = "--list" in args; gs = "--gs" in args
    src = opt("--src", None) or os.path.join(work, "src"); os.makedirs(src, exist_ok=True)
    if not os.path.exists(os.path.join(src, "mfl_tier2.h")) or "--resync" in args:
        for h in os.listdir(R):
            if h.endswith(".h"): shutil.copy(os.path.join(R, h), src)
        nd = os.path.join(R, "automatch_near")
        for f in os.listdir(nd):
            if f.endswith((".c", ".h")) and not os.path.islink(os.path.join(nd, f)): shutil.copy(os.path.join(nd, f), src)
    nm = bc.names(); cand = []
    if names_a:
        cand = [l.split()[0] for l in open(names_a[1:])] if names_a.startswith("@") else names_a.split(",")
    else:
        import csv
        for r in csv.DictReader(open(os.path.join(HERE, "..", "output", "bytediff_causes_2026-10-29.csv"))):
            if r["name"] not in nm or not r["bytes_diff_count"].isdigit() or r["category"] == "MATCH": continue
            va = nm[r["name"]][0]
            if gs and not (cmpfn.FNSZ.get(va) and b"\xd8\xc5\x62\x00" in cmpfn.exe_bytes(va, cmpfn.FNSZ[va])): continue
            if maxdiff and int(r["bytes_diff_count"]) > maxdiff and not gs: continue
            cand.append((int(r["bytes_diff_count"]), r["name"]))
        cand = [n for _, n in sorted(cand)]
    cand = [n for n in cand if n in nm]
    print("candidates:", len(cand), flush=True)
    if only_list: print(" ".join(cand)); return
    os.makedirs(os.path.join(work, "hits"), exist_ok=True)
    hits = open(os.path.join(work, "hits.txt"), "a"); lock = threading.Lock()
    def log(s):
        with lock: print(time.strftime("%H:%M:%S"), s, flush=True)
    def one(n):
        va, ch = nm[n]
        cx = prep(n, va, ch, src, flags)
        if not cx: log("%s: definition not found" % n); return
        try: st = search(cx, work, budget, B, seed, lm, log)
        except Exception as e: log("%s: EXC %r" % (n, e)); return
        if st.get("error"): log("%s: baseline error %s" % (n, st["error"][:100]))
        elif st["hit"]:
            new = cx.chunk_text[:cx.span[0]] + st["best"] + cx.chunk_text[cx.span[1]:]
            os.makedirs(os.path.join(work, "hitsw", "out"), exist_ok=True)
            tgt = os.path.join(work, "hitsw", "out", ch)
            if os.path.exists(tgt): # several hits in one chunk: apply onto the previous hit text
                t0 = open(tgt, errors="surrogateescape").read(); m = next((m for m in rw.DEF.finditer(t0) if m.group(1) == n), None)
                if m: new = t0[:m.start()] + st["best"] + t0[m.end():]
            open(tgt, "w", errors="surrogateescape").write(new)
            open(os.path.join(work, "hits", n + ".c"), "w", errors="surrogateescape").write(st["best"])
            with lock: hits.write("%s %s %s\n" % (n, ch, st.get("tag", ""))); hits.flush()
            log("HIT-SAVED %s (%s)" % (n, ch))
        else: log("%s: no hit, best %s of base %s, %d variants" % (n, st["best_score"], st.get("base_score"), len(st["seen"])))
    with ThreadPoolExecutor(jobs) as ex: list(ex.map(one, cand))


if __name__ == "__main__":
    main()
