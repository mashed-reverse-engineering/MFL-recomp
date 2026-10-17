#!/usr/bin/env python3
"""cluster_auto.py [ROOTS_FILE | --all | --calib | --root-only] [--max N] [--maxbytes B] [--maxmembers K] [--run DIR] [--cases N] [--plan] [--batch N]
Scripted cluster production (like t2_auto.py for tier 2): for usercall ROOTS (register convention function + its not-yet-C callers, output/callgraph_clusters.csv) it
  (a) takes the Ghidra text of every member from output/named/MFL_app_named.c (the root keeps the parameters Ghidra gave it, `reg_eax` becomes the natural parameter `in_eax`;
      the register inputs of output/reginputs.csv / the stack types of output/regsigs.csv correct the signature: lost registers / surplus Ghidra registers are fixed in the root
      and in the argument lists of the calls to it, a lost stack parameter makes the cluster a failure),
  (b) runs the t2_auto generation stage (autocompile rounds + scripted body fixes) on the members; calls of the root stay plain natural calls, calls of OTHER register callees
      outside the cluster become natural `extern` calls + the stub option `callee=@eax,s0` (derived from the bridgegen F_r description), no naked adapter stays in the source,
  (c) t2_postfix kinds per member, merges the members into ONE file <run>/c/<root>.c (+ prelude.h, ghidra_types.h) and a clusters.txt row (root | file | entries | order | options),
  (d) compiles it and (e) runs `diffemu --cluster` (in process, one cluster at a time), (f) defectlint / cheatgrep / ptrcast+rawaddr re-verify.
ISOLATED: writes only below the run dir (default $TMPDIR/clrun_a1): results.csv (one row per cluster), c/clusters.txt + c/results.csv (cluster_pilot format: use
`cluster_install.py --dir <run>/c ROOT` after copying <run>/c to output/cluster_pilotN), names_equal.txt (every entry EQUAL), names_pass.txt (EQUAL + lint clean), failures/<root>.md, summary.txt.
Needs the diffemu venv (re-execs itself) and a docker container for cl 13.00 (own container mfl_t2_<run id>).  Never touches src/ or the shared output files."""
import os, sys
try: import unicorn
except ImportError:
    py = os.environ.get("DIFFEMU_PY") or os.path.expanduser("~/.cache/mfl_diffemu_venv/bin/python"); os.execv(py, [py] + sys.argv)
import collections, csv, glob, re, shutil, signal, subprocess, time
HERE = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(HERE, ".."))
A = sys.argv[1:]
def opt(n, d, t=str): return t(A[A.index(n) + 1]) if n in A else d
BASE = os.path.abspath(opt("--run", os.path.join(os.environ.get("TMPDIR", "/tmp"), "clrun_a1")))
os.makedirs(BASE, exist_ok=True)
sys.argv = [sys.argv[0], "--run", os.path.join(BASE, "t2")]       # t2_auto reads --run at import: its work dir, overlay, container, diffemu out all live below BASE/t2
sys.path[:0] = [HERE, os.path.join(HERE, "diffemu")]
import t2_auto as T2
import core, diffemu, cluster as clmod, t2_postfix, ptrcast
import bridgegen as bg
P = lambda *a: os.path.join(G, *a)
CDIR = os.path.join(BASE, "c"); FAILS = os.path.join(BASE, "failures")
for d in (CDIR, FAILS, T2.FILES): os.makedirs(d, exist_ok=True)
LOSTREG = "--no-lostreg" not in A
FLOATRET = ("float", "double", "RwReal", "float10")

# ------------------------------------------------------------------------------------------------------------- data
def csvrows(p): return {r["name"]: r for r in csv.DictReader(open(P(*p.split("/"))))}
RI = csvrows("output/reginputs.csv"); RS = csvrows("output/regsigs.csv")
def regs_of(n): return [x for x in RI.get(n, {}).get("reg_inputs", "").split("|") if x]
def words(t): return 2 if re.search(r"\b(double|float10|longlong|ulonglong|undefined8|__int64)\b", t) and "*" not in t else 1

def split_top(s):
    out, d, cur = [], 0, ""
    for ch in s:
        d += {"(": 1, ")": -1, "[": 1, "]": -1}.get(ch, 0)
        if ch == "," and d == 0: out.append(cur); cur = ""
        else: cur += ch
    return out + [cur] if cur.strip() else out

def rewrite_calls(body, name, new, drop, nparams, ins=()):
    """calls `name(args)` -> `new(args without the positions in drop)`; returns (body, [calls with fewer arguments than nparams])"""
    out, pos, short = "", 0, []
    for m in re.finditer(r"(?<![\w.>])%s\s*\(" % re.escape(name), body):
        if m.start() < pos: continue
        i, d = m.end(), 1
        while i < len(body) and d:
            d += {"(": 1, ")": -1}.get(body[i], 0); i += 1
        if d: continue
        args = split_top(body[m.end():i - 1])
        keep = [a for k, a in enumerate(args) if k not in drop]
        for idx, txt in ins: keep.insert(min(idx, len(keep)), txt)      # a register Ghidra lost: placeholder local (t2_postfix argsite / stalearg takes the value from the original call site)
        if len(keep) < nparams: short.append(len(keep))
        out += body[pos:m.start()] + new + "(" + ",".join(keep) + ")"; pos = i
    return out + body[pos:], short

def head_of(body, name):
    return re.search(r"^(?P<ret>[A-Za-z_][^\n(;]*?)\b%s\s*\((?P<ps>[^)]*)\)(?P<tail>\s*)\{" % re.escape(name), body, re.M)

class Fail(Exception):
    def __init__(s, cls, detail): s.cls, s.detail = cls, detail

def root_signature(root, body):
    """-> (new body of the root with natural parameters in_<reg>, params [(type, name, kind, where)], drop set (Ghidra argument positions removed), ret type, order tokens)"""
    m = head_of(body, root)
    if not m: raise Fail("sig:no-head", "definition head of the root not found")
    R = regs_of(root); stk = [t for t in RS.get(root, {}).get("stack", "").split(";") if t]
    ps = [p.strip() for p in split_top(m.group("ps")) if p.strip() and p.strip() != "void"]
    kept, drop, seen, sw = [], set(), set(), 0
    for gi, p in enumerate(ps):
        nm = re.search(r"(\w+)\s*$", p)
        if not nm: raise Fail("sig:unparsed", p)
        n = nm.group(1); mr = re.fullmatch(r"reg_(e(?:ax|bx|cx|dx|si|di|bp))", n)
        if mr:
            if mr.group(1) in R and mr.group(1) not in seen: seen.add(mr.group(1)); kept.append((re.sub(r"\b%s\b" % n, "in_" + mr.group(1), p), "in_" + mr.group(1), "reg", mr.group(1), p[:nm.start()].strip()))
            else: drop.add(gi)
        else:
            t = p[:nm.start()].strip(); kept.append((p, n, "stack", sw, t)); sw += words(t)
    miss = [r for r in R if r not in seen]
    cc = re.search(r"__(fastcall|thiscall)", m.group("ret"))
    if cc and miss:      # Ghidra's fastcall / thiscall: the first named stack-kind parameter(s) ARE ecx (edx)
        sk = [j for j, k in enumerate(kept) if k[2] == "stack"]
        for j, rg in zip(sk, ["ecx", "edx"][:2 if cc.group(1) == "fastcall" else 1]):
            if rg in miss: kept[j] = (kept[j][0], kept[j][1], "reg", rg, kept[j][4]); miss.remove(rg)
        sw = 0
        for q, k in enumerate(kept):
            if k[2] == "stack": kept[q] = (k[0], k[1], "stack", sw, k[4]); sw += words(k[4])
    if len(miss) == 1:      # a register parameter that the project renamed (`VehicleRecord *vehicle_record`, storage EAX): the first named parameter in front of the stack ones
        for j, k in enumerate(kept):
            if k[2] == "stack" and not re.fullmatch(r"param_\d+", k[1]):
                kept[j] = (k[0], k[1], "reg", miss[0], k[4]); miss = []
                sw = 0
                for q in kept:
                    if q[2] == "stack": kept[kept.index(q)] = (q[0], q[1], "stack", sw, q[4]); sw += words(q[4])
                break
            if k[2] == "stack": break
    ins, subst = [], []
    if miss and LOSTREG:      # Ghidra read the register as a pseudo variable (`unaff_ESI`, `in_ECX`): it becomes a parameter, the callers get a placeholder local for argsite
        after = body[m.end():]
        for r in list(miss):
            pn = ("unaff_" if r in ("ebx", "esi", "edi", "ebp") else "in_") + r.upper()
            if not re.search(r"\b%s\b" % pn, after): continue      # the register is not read as a variable: not recoverable here
            dm = re.search(r"^[ \t]*([A-Za-z_][\w ]*?[ \*]+)%s;[ \t]*\n" % pn, after, re.M)
            ty = dm.group(1).strip() if dm else "undefined4"
            pos = max([j for j, k in enumerate(kept) if k[2] == "reg"], default=-1) + 1
            kept.insert(pos, (ty + ("" if ty.endswith("*") else " ") + "in_" + r, "in_" + r, "reg", r, ty))
            ins.append((pos, "lost_" + r)); miss.remove(r); subst.append((pn, "in_" + r, dm.group(0) if dm else None))
    if miss: raise Fail("sig:lost-register", "Ghidra signature lacks register input(s) %s" % ",".join(miss))
    if not stk and sw and sw * 4 == int(RI.get(root, {}).get("ret_n", 0) or 0): stk = ["?"] * sw      # regsigs has no types for the stack slots; ret N and Ghidra's count agree
    if sw != len(stk): raise Fail("sig:stack-mismatch", "Ghidra has %d stack dwords (%s), regsigs.csv %d (%s)" % (sw, "; ".join(k[4] for k in kept if k[2] == "stack"), len(stk), ";".join(stk)))
    order = [k[3] if k[2] == "reg" else "s%d" % k[3] for k in kept]
    rest = body[:m.start()] + "%s %s(%s)%s{" % (re.sub(r"\s*__(?:cdecl|stdcall|fastcall|thiscall)\s*", " ", m.group("ret")).strip(), root, ", ".join(k[0] for k in kept) or "void", m.group("tail")) + body[m.end():]
    for k in kept:
        if k[2] == "reg": rest = re.sub(r"\breg_%s\b" % k[3], "in_" + k[3], rest)
    for pn, nn, decl in subst:
        if decl: rest = rest.replace(decl, "", 1)
        rest = re.sub(r"\b%s\b" % pn, nn, rest)
    return rest, kept, drop, re.sub(r"\s*__(?:cdecl|stdcall|fastcall|thiscall)\s*", " ", m.group("ret")).strip(), order, ins

# ------------------------------------------------------------------------------------------------------------- plan: texts of one cluster
def plan(root, members, funcs, protos, regin_all, have):
    """-> dict(texts {name: (va, body)} for autocompile, order, kept, ret, extra) or raises Fail"""
    if root not in funcs: raise Fail("plan:root-no-decomp", "no Ghidra C for the root")
    miss = [m for m in members if m not in funcs]
    if miss: raise Fail("plan:member-no-decomp", " ".join(miss)[:150])
    um = [m for m in members if m != root and regs_of(m)]
    if um: raise Fail("plan:member-usercall", "member(s) with register inputs: " + " ".join(um)[:150])
    if not regs_of(root) and not int(RI.get(root, {}).get("ret_n", 0) or 0): raise Fail("plan:root-not-usercall", "no register input / ret N")
    body, kept, drop, ret, order, ins = root_signature(root, funcs[root][1])
    nparam = len(kept); new = root + "__cl"
    texts = {}; shorts = []
    for m in members:
        b = body if m == root else funcs[m][1]
        if m == root:      # not the definition head itself
            hm0 = head_of(b, root); b0, sh = rewrite_calls(b[hm0.end():], root, new, drop, nparam, ins); b = b[:hm0.end()] + b0
        else: b, sh = rewrite_calls(b, root, new, drop, nparam, ins)
        if ins and m != root and new in b:      # placeholder locals (never assigned) at the top of the caller
            hm = head_of(b, m)
            if hm: b = b[:hm.end()] + "".join("\n  undefined4 %s;" % x for _, x in ins) + b[hm.end():]
        if sh: shorts.append("%s passes %s argument(s), the root takes %d" % (m, sorted(set(sh)), nparam))
        if m == root: b = re.sub(r"(?m)^(\S[^\n(;]*?\b)%s(\s*\()" % re.escape(root), r"\1%s\2" % new, b, count=1)
        else: b = re.sub(r"(?m)^(\S[^\n(;]*?\b)%s(\s*\()" % re.escape(m), r"\1%s\2" % m, b, count=1)
        texts[m] = b
    texts[root] = texts[root]
    # reg_ pseudo parameter names of the root (kept ones) are in_<reg> now; any other reg_e?? local of the root = a register the root reads that is not an input
    for m in members:
        sc = skip_member(m, root, texts[m], protos, regin_all, have)
        if sc: raise Fail(sc[0], m + ": " + sc[1])
    proto = "extern %s %s(%s);" % (ret, new, ", ".join(re.sub(r"\bin_e\w\w\b|\b(?<=\s)\w+$", lambda mm: "", k[0]).strip() if False else k[0] for k in kept) or "void")
    return dict(texts=texts, order=order, kept=kept, ret=ret, proto=proto + " /* CLX */", shorts=shorts, new=new)

def skip_member(m, root, body, protos, regin_all, have):
    pr = protos | ({"FUN_" + n[4:] for n in ADAPT_DESC if n.startswith("FUN_")} if "--root-only" in A else set())      # root-only: a register callee with a bridgegen adapter becomes a natural extern + stub option @regs
    return T2.skip_class(m, body, {}, have, pr)

# ------------------------------------------------------------------------------------------------------------- chunk -> member text
def natural_callees(text):
    """the generated chunk calls register callees through static naked F_r adapters (AC_PLAIN): remove the adapter, the call goes to the natural `extern` (stub option callee=@regs).  -> (text, {callee: option})"""
    opts = {}
    for m in list(re.finditer(r"^static ([^\n]*?)\b(\w+)_r\(([^\n]*)\);\n", text, re.M)):
        cn = m.group(2)
        if cn not in ADAPT_DESC: continue
        f = ADAPT_DESC[cn]; where = []
        for (t, n, k, w) in f["params"]: where.append(w if k == "reg" else "s%d" % ((w[0] - 4) // 4))
        opts[cn] = "@" + ",".join(where)
        a = "/*%08x*/ " % f["addr"] if f.get("addr") else ""
        text = text.replace(m.group(0), "%sextern %s %s(%s);\n" % (a, m.group(1).strip(), cn, m.group(3)))
        text = re.sub(r"^extern void %s\(void\);\n" % re.escape(cn), "", text, flags=re.M)
        text = re.sub(r"^__declspec\(naked\)[^\n]*\b%s_r\([^\n]*\n\{.*?\n\}\n" % re.escape(cn), "", text, flags=re.M | re.S)
        text = re.sub(r"\b%s_r\b" % re.escape(cn), cn, text)
        if re.match(r"(?:\w+ )*(?:%s)\b" % "|".join(FLOATRET), m.group(1).strip()): opts[cn + "=f"] = ""
    return text, opts

ADAPT_DESC = {}
def load_adapters():
    bl, sg, ri, man = bg.load(); ad = sorted(a for a, _ in bl.values()); nx = {a: ad[i + 1] if i + 1 < len(ad) else a + 0x400 for i, a in enumerate(ad)}
    for n in bg.adapter_ok(bl, sg, ri, man, nx): ADAPT_DESC[n] = bg.analyse(n, bl, sg, ri, man, nx)[0]
    ADAPT_DESC.update({n: f for n, f in bg.synth_adapters(bl, ri, {})[0].items() if n not in ADAPT_DESC})
    for n, f in list(ADAPT_DESC.items()):
        if f["ret"] == "void": ADAPT_DESC[n] = dict(f, ret="int")      # as autocompile AC_PLAIN

def split_chunk(t):
    i = re.search(r"^/\* (\S+) \*/$", t, re.M)
    return t[:i.start()], t[i.start():]

def merge(parts, root, entries):
    """parts: [(head, body)] -> one file: includes once, extern lines once per identifier (not for names the file defines), then the bodies; CLX lines dropped"""
    heads, seen, out = [], set(), []
    for h, b in parts:
        for l in h.split("\n"):
            if not l.strip(): continue
            m = re.match(r"(?:/\*[0-9a-f]{8}\*/ )?extern [^;(]*?\b([A-Za-z_]\w*)\s*(?:\(|\[|;)", l)
            key = ("x", m.group(1)) if m else ("l", l)
            if m and m.group(1) in entries: continue
            if key in seen: continue
            seen.add(key); heads.append(l)
    bodies = []
    for h, b in parts:
        b = re.sub(r"(?m)^[^\n]*/\* CLX \*/\n", "", b)
        bodies.append(b.strip("\n"))
    hdr = [l for l in heads if l.startswith("#include")]; rest = [l for l in heads if not l.startswith("#include")]
    return "\n".join(hdr + rest) + "\n" + "\n\n".join(bodies) + "\n"

# ------------------------------------------------------------------------------------------------------------- main
def run_cluster_diffemu(root, specfile, cases):
    """diffemu.cluster_main in process -> rows of the cluster (results.csv of the spec dir)"""
    old = signal.signal(signal.SIGALRM, lambda *a: (_ for _ in ()).throw(TimeoutError("cluster diffemu > %ds" % TMO)))
    signal.alarm(TMO)
    try:
        diffemu.cluster_main([root, "--clusters", specfile, "--cases", str(cases)])
    finally: signal.alarm(0); signal.signal(signal.SIGALRM, old)
    return list(csv.DictReader(open(os.path.join(os.path.dirname(specfile), "results.csv"))))
TMO = 900

def clusters_now(root_only=False):
    """output/callgraph_clusters.csv is stale (regsigs_old.csv, 266 usercall functions; reginputs2 made it 680): tools/callgraph.py recomputed in memory with the CURRENT regsigs.csv,
    without the entries of installed clusters (src/clusters/enabled.txt).  -> [dict(root, members, bytes)]"""
    import bisect
    O = P("output")
    items = [r for r in csv.DictReader(open(os.path.join(O, "delink_sdk", "ownership_items.csv"))) if r["kind"] == "code"]
    items.sort(key=lambda r: int(r["lo"], 16)); los = [int(r["lo"], 16) for r in items]; his = [int(r["hi"], 16) for r in items]
    def find(a):
        i = bisect.bisect_right(los, a) - 1
        return i if i >= 0 and a < his[i] else None
    ledger = {int(r["va"], 16): r["tier"] for r in csv.DictReader(open(os.path.join(O, "tier_ledger.csv")))}
    usercall = {int(r["addr"], 16) for r in RS.values()}
    inst = set()
    for n in open(P("src", "clusters", "enabled.txt")).read().split():
        inst.update(open(P("src", "clusters", "cl_%s.c" % n)).readline().split(":", 1)[1].split())
    C_TIERS = {"tier1", "hand", "agent", "tier2", "bridge", "tier"}
    game = {i: ("C" if ledger.get(los[i]) in C_TIERS or items[i]["name"] in inst else "orig") for i, r in enumerate(items) if r["owner"] == "game"}
    callers = collections.defaultdict(set)
    for ln in open(os.path.join(O, "delink", "ghidra_refs.csv")):
        f, t_, ty = ln.split(",")[:3]
        try: f = int(f, 16); t_ = int(t_, 16)
        except ValueError: continue
        j = find(t_)
        if j is None or t_ != los[j] or ty not in ("UNCONDITIONAL_CALL", "UNCONDITIONAL_JUMP"): continue
        i = find(f)
        if i is None or i == j or (ty == "UNCONDITIONAL_JUMP" and los[i] <= t_ < his[i]): continue
        callers[j].add(i)
    out = []
    for i, s in game.items():
        if s != "orig" or los[i] not in usercall: continue
        if root_only and los[i] >= 0x4B0000: continue      # --root-only: app region only
        mem = [i] + ([] if root_only else sorted(c for c in callers[i] if game.get(c) == "orig"))
        out.append(dict(root=items[i]["name"], members=" ".join(items[m]["name"] for m in mem), bytes=sum(his[m] - los[m] for m in mem)))
    return out

def select():
    rows = clusters_now("--root-only" in A)
    inst = set(open(P("src", "clusters", "enabled.txt")).read().split())
    pil = set()
    for f in glob.glob(P("output", "cluster_pilot*", "clusters.txt")):
        pil |= {l.split("|")[0].strip() for l in open(f) if l[0] != "#" and "|" in l}
    pos = [x for i, x in enumerate(A) if not x.startswith("--") and (i == 0 or A[i - 1] not in ("--run", "--max", "--maxbytes", "--maxmembers", "--cases", "--batch", "--dump-missing", "--text"))]
    if pos and os.path.exists(pos[0]): want = [l.split()[0] for l in open(pos[0]) if l.strip()]; rows = [r for r in rows if r["root"] in want]
    elif pos: rows = [r for r in rows if r["root"] in pos]
    elif "--calib" in A:
        allr = [dict(root=r["root"], members=r["members"], bytes=r["bytes"]) for r in csv.DictReader(open(P("output", "callgraph_clusters.csv")))]
        rows = [r for r in allr if r["root"] in inst][:int(opt("--max", 12))]
    elif "--root-only" in A: rows = [r for r in rows if r["root"] not in inst]      # every uninstalled app-region usercall function = its own single-entry cluster (callers stay original and call the entry thunk)
    else: rows = [r for r in rows if r["root"] not in inst and r["root"] not in pil]
    rows.sort(key=lambda r: int(r["bytes"]))
    mb, mm = int(opt("--maxbytes", 10 ** 9)), int(opt("--maxmembers", 10 ** 9))
    rows = [r for r in rows if int(r["bytes"]) <= mb and len(r["members"].split()) <= mm]
    if "--max" in A and "--calib" not in A: rows = rows[:int(opt("--max", 0))]
    return rows

def rows_names(rows): return sorted({m for r in rows for m in r['members'].split()} | {r['root'] for r in rows})

def main():
    rows = select(); funcs = T2.load_funcs(T2.NAMED); load_adapters()
    for x in [P("output", "t2_auto", "need_decomp.c")] + [A[i + 1] for i, v in enumerate(A) if v == "--text"]:      # extra Ghidra exports (DecompList.java) for the FUN_/gap_ functions the named file lacks
        if os.path.exists(x):
            for k, v in T2.load_funcs(x).items(): funcs.setdefault(k, v)
    for k in [k for k in rows_names(rows) if k not in funcs and k.startswith("gap_") and ("FUN_" + k[4:].zfill(8)) in funcs]: funcs[k] = (funcs["FUN_" + k[4:].zfill(8)][0], re.sub(r"\bFUN_%s\b" % k[4:].zfill(8), k, funcs["FUN_" + k[4:].zfill(8)][1]))      # Ghidra names a created gap function FUN_<va>
    if "--dump-missing" in A:
        sy = core.T().sym; out = {}
        for k in rows_names(rows):
            if k in funcs: continue
            m = re.search(r"(?:^|[_@])([0-9a-f]{6,8})$", k) if k not in sy else None
            v = "%08x" % min(sy[k]) if k in sy else (m.group(1).zfill(8) if m else None)
            if v: out[v] = k
        open(opt("--dump-missing", ""), "w").write("".join("%s %s\n" % kv for kv in out.items())); return
    regin_all = {n: v["reg_inputs"].strip("|") for n, v in RI.items() if v["reg_inputs"].strip("|")}
    have = {l.split()[1] for l in open(P("src", "manifest.txt")) if len(l.split()) > 3 and l.split()[0] not in ("#", "(none)")}
    import callee_auto
    ap, WHY_ = callee_auto.build(funcs); callee_auto.save(ap); T2.WHY.update(WHY_)
    protos = {"FUN_" + v for v in callee_auto.protoed()}
    res = {}      # root -> dict
    def put(root, r, stage, verdict, cls, detail, **kw):
        res[root] = dict(root=root, size=len(r["members"].split()), bytes=r["bytes"], stage=stage, verdict=verdict, cls=cls, detail=" ".join(str(detail).split())[:300], **kw)
    plans = {}; claimed = {}
    for r in rows:
        root = r["root"]; ms = r["members"].split()
        if root not in ms: ms.insert(0, root)
        try: p = plan(root, ms, funcs, protos, regin_all, have)
        except Fail as e: put(root, r, "plan", "SKIP", e.cls, e.detail); continue
        clash = [m for m in ms if m in claimed]
        if clash: put(root, r, "plan", "SKIP", "plan:member-shared", "member(s) %s also in cluster %s" % (" ".join(clash)[:80], claimed[clash[0]])); continue
        for m in ms: claimed[m] = root
        p["members"] = ms; plans[root] = (r, p)
    print("clusters %d, planned %d, skipped %d" % (len(rows), len(plans), len(res)), flush=True)
    if "--plan" in A: write_out(res, rows); return
    # --- shared preamble of t2_auto.main (struct specs newer than the exported header, array overrides)
    subprocess.run([sys.executable, P("tools", "spec_header.py"), P("src", "auto", "ghidra_types.h"), T2.HDR], capture_output=True)
    for h in ("ghidra_types.h",): shutil.copy(T2.HDR, os.path.join(CDIR, h))
    shutil.copy(P("src", "auto", "prelude.h"), os.path.join(CDIR, "prelude.h"))
    dec = open(P("output", "decls.h"), errors="replace").read(); fx = os.path.join(T2.W, "overlay", "MFL_extern_fix.txt"); ov = []; nm_at = collections.defaultdict(list)
    for l_ in open(P("src", "symbols.txt")):
        q = l_.split()
        if len(q) == 2: nm_at[int(q[1], 16)].append(q[0])
    for f_ in glob.glob(P("names", "MFL_structs*.txt")):
        for l_ in open(f_):
            q = l_.strip().split(",")
            if q[0] == "apply" and len(q) >= 4 and int(q[3]) > 1:
                ov += ["extern struct %s %s[%s];" % (q[2], nm, q[3]) for nm in nm_at[int(q[1], 16)] if re.search(r"extern (?:undefined4|undefined|int|uint) %s;" % nm, dec)]
    if ov: open(fx, "a").write("".join(l_ + "\n" for l_ in ov))
    specs = []; bsz = int(opt("--batch", 12)); cases = int(opt("--cases", 200)); order = list(plans)
    for bi in range(0, len(order), bsz):
        batch = order[bi:bi + bsz]; gf = {}; names = []
        for root in batch:
            r, p = plans[root]
            for m in p["members"]:
                body = p["texts"][m]
                if m != root and re.search(r"\b%s\(" % re.escape(p["new"]), body): body = p["proto"] + "\n" + body
                gname = p["new"] if m == root else m
                if m == root: body = p["proto"] + "\n" + body       # the root's own definition: nothing; recursion needs the extern line, harmless
                gf[gname] = ("00000001" if m == root else funcs[m][0], body); names.append(gname)
        try: st = T2.generate(names, gf, T2.NAMED)
        except Exception as e:
            for root in batch: put(root, plans[root][0], "gen", "ERROR", "gen:exception", repr(e));
            continue
        for root in batch:
            r, p = plans[root]
            try: finish(root, r, p, st, put, specs, cases, gf)
            except Fail as e: put(root, r, e.stage if hasattr(e, "stage") else "gen", "FAIL", e.cls, e.detail)
            except Exception as e:
                import traceback; put(root, r, "exception", "ERROR", "tool:exception", repr(e) + traceback.format_exc()[-200:])
            print("%-45s %-8s %s %s" % (root, res[root]["verdict"], res[root]["cls"], res[root]["detail"][:100]), flush=True)
        write_out(res, rows)
    write_out(res, rows)

def finish(root, r, p, st, put, specs, cases, gf):
    mem = p["members"]; parts = []; opts = {}; files = {}
    kept = p["kept"]; rfirst = [k[2] for k in kept] == sorted((k[2] for k in kept), key=lambda x: x != "reg")      # argsite wants register parameters first, then stack
    rproto = "static %s %s_r(%s); /* CLX */" % (p["ret"], root, ", ".join(("%s reg_%s" % (k[4], k[3])) if k[2] == "reg" else "%s stk%d" % (k[4], i) for i, k in enumerate(kept)) or "void")
    for m in mem:
        g = p["new"] if m == root else m
        ok, f, err, rnd = st[g]
        if not f or not ok:
            raise Fail(T2.cclass(err) if err else "gen:failed", "%s: %s" % (m, err))
        t = open(f, errors="replace").read()
        if m == root: t = re.sub(r"(?m)^[^\n]*/\* CLX \*/\n", "", t).replace(p["new"], root)
        elif rfirst and p["new"] in t:      # the calls of the root look like F_r calls for the argsite stage (original call sites give lost arguments)
            t = re.sub(r"(?m)^[^\n]*/\* CLX \*/$", lambda mm: rproto, t); t = t.replace(p["new"], root + "_r")
        fn = os.path.join(T2.FILES, m + ".c"); open(fn, "w").write(t)
        try: pk = t2_postfix.fix(fn)
        except Exception as ex: pk = []; print("postfix %s: %s" % (m, ex))
        t = open(fn, errors="replace").read()
        t = re.sub(r"(?m)^[^\n]*/\* CLX \*/\n", "", t).replace(root + "_r(", root + "(").replace(p["new"], root)
        t, o = natural_callees(t); opts.update(o)
        open(fn, "w").write(t)
        parts.append(split_chunk(t))
    text = merge(parts, root, set(mem))
    text = text.replace("\n/* %s */" % root, "\n/* %s */" % root)
    # a leftover naked adapter / F_r name = a callee we could not turn into a natural call
    if re.search(r"\b\w+_r\(", text) and re.search(r"__declspec\(naked\)", text): raise Fail("gen:adapter-left", "naked F_r adapter left in the source")
    cdir = CDIR; src = os.path.join(cdir, root + ".c"); open(src, "w").write(text)
    pk = t2_postfix.fix(src)
    o, k, e = T2.compile_one(src)
    if e: raise Fail(T2.cclass(e), e)
    o2 = " ".join(("%s=%s" % (kk, vv) if vv else kk) for kk, vv in sorted(opts.items()) if True)
    o2 = " ".join(sorted(("%s=%s" % (kk, vv)) if vv else kk for kk, vv in opts.items()))
    line = "%s | %s.c | %s | %s | %s" % (root, root, " ".join(mem), " ".join(p["order"]), o2)
    specf = os.path.join(cdir, "clusters.txt")
    # --- diffemu cluster mode (spec of this cluster alone, results copied)
    one = os.path.join(BASE, "one", root); os.makedirs(one, exist_ok=True)
    for h in ("prelude.h", "ghidra_types.h"): shutil.copy(os.path.join(cdir, h), one)
    shutil.copy(src, os.path.join(one, root + ".c")); sf = os.path.join(one, "clusters.txt"); open(sf, "w").write(line + "\n")
    t0 = time.time()
    try: rows = run_cluster_diffemu(root, sf, cases)
    except Exception as ex: raise StageFail("diffemu", "diffemu:exception", "%s %s" % (type(ex).__name__, str(ex)[:200]))
    vs = {x["name"]: x for x in rows}
    bad = [x for x in rows if x["verdict"] != "EQUAL"]
    for x in rows: x["cluster"] = root
    cl_rows.extend(rows)
    open(specf, "a").write(line + "\n")
    secs = "%.0f" % (time.time() - t0)
    if bad:
        d = ""
        dp = os.path.join(one, "diffs", bad[0]["name"] + ".txt")
        if os.path.exists(dp): d = open(dp).read()
        shutil.copy(dp, os.path.join(FAILS, root + "_" + bad[0]["name"] + ".txt")) if os.path.exists(dp) else None
        v = bad[0]["verdict"]
        raise StageFail("diffemu", ("diffemu:" + (T2.dgroup(d + bad[0]["note"]) if v == "DIFF" else v)), "%s %s: %s %s" % (bad[0]["name"], v, bad[0]["note"], d[:200]), rows=rows, secs=secs)
    low = [x for x in rows if float(x["coverage_orig"]) < 0.8 or float(x["coverage_ours"]) < 0.8 or int(x["cases"]) < 20]
    # --- ptrcast / rawaddr rewrite (raw data addresses -> symbols) and a second diffemu run on the rewritten text, as t2_auto
    orig_txt = open(src, errors="replace").read(); nt = ptrcast.rewrite(orig_txt)
    if nt != orig_txt: open(src, "w").write(nt)
    subprocess.run([sys.executable, P("tools", "rawaddr.py"), "--rewrite", src], capture_output=True)
    if open(src, errors="replace").read() != orig_txt:
        o_, k_, e_ = T2.compile_one(src); ok2 = not e_
        if ok2:
            shutil.copy(src, os.path.join(one, root + ".c"))
            try: rows2 = run_cluster_diffemu(root, sf, cases)
            except Exception: rows2 = []
            ok2 = bool(rows2) and all(x["verdict"] == "EQUAL" for x in rows2)
            if ok2: rows = rows2; cl_rows[-len(rows2):] = [dict(x, cluster=root) for x in rows2] if len(cl_rows) >= len(rows2) else cl_rows
        if not ok2:
            open(src, "w").write(orig_txt); shutil.copy(src, os.path.join(one, root + ".c")); rewrite_note = "ptrcast/rawaddr rewrite not EQUAL, reverted"
    # --- lint
    probs = T2.lint(src, root)
    def regcall_ok(x):      # defectlint class 7 "X takes its input in registers but is called as cdecl": the members and the root are natural C here, the `@` callees are adapted at install
        if re.search(r"class 4 .*pops its arguments", x) and int(RI[root]["ret_n"] or 0): return True      # the entry thunk of the root does the `ret N`
        m_ = re.search(r"class 7 .*?`(\w+)`", x)
        return bool(m_) and (m_.group(1) in mem or m_.group(1) in opts)
    probs = [x for x in probs if not regcall_ok(x)]
    rewrite_note = locals().get("rewrite_note", "")
    verdict = "EQUAL" if not low else "EQUAL-LOWCOV"
    names_equal.append(root)
    stage = "pass"
    if probs: verdict = "EQUAL-LINT"; stage = "lint"
    elif low: stage = "lowcov"
    else: names_pass.append(root)
    put(root, r, stage, verdict, "pass" if stage == "pass" else ("lint" if probs else "diffemu:lowcov"), "; ".join(probs)[:250] if probs else ("low coverage: " + ",".join(x["name"] for x in low) if low else ""), secs=secs, entries=len(mem))

class StageFail(Fail):
    def __init__(s, stage, cls, detail, rows=None, secs=""): super().__init__(cls, detail); s.stage = stage; s.rows = rows; s.secs = secs

cl_rows, names_equal, names_pass = [], [], []
def write_out(res, rows):
    cols = ["root", "size", "bytes", "stage", "verdict", "cls", "detail", "secs", "entries"]
    with open(os.path.join(BASE, "results.csv"), "w", newline="") as f:
        w = csv.DictWriter(f, cols, extrasaction="ignore"); w.writeheader(); w.writerows(sorted(res.values(), key=lambda x: int(x["bytes"])))
    if cl_rows:
        with open(os.path.join(CDIR, "results.csv"), "w", newline="") as f:
            w = csv.DictWriter(f, ["cluster"] + diffemu.COLS, extrasaction="ignore"); w.writeheader(); w.writerows(cl_rows)
    open(os.path.join(BASE, "names_equal.txt"), "w").write("".join(n + "\n" for n in names_equal))
    open(os.path.join(BASE, "names_pass.txt"), "w").write("".join(n + "\n" for n in names_pass))
    c = collections.Counter(x["cls"] for x in res.values()); b = collections.Counter()
    for x in res.values(): b[x["cls"]] += int(x["bytes"])
    L = ["cluster_auto: %d clusters, EQUAL %d (pass %d)" % (len(res), len(names_equal), len(names_pass))] + ["  %-34s %4d clusters %7d B" % (k, v, b[k]) for k, v in sorted(c.items(), key=lambda kv: -kv[1])]
    open(os.path.join(BASE, "summary.txt"), "w").write("\n".join(L) + "\n")

if __name__ == "__main__": main()
