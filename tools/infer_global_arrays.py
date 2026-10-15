#!/usr/bin/env python3
"""infer_global_arrays.py [--report] [out.txt]: find arrays of structs hidden in globals.
Looks in output/named/MFL_app_named.c for `(&g_X)[idx * N]` and `(int)&g_X + idx * N` / `&g_X + idx * N`
(stride K bytes), clusters the globals of one stride whose addresses lie within K of each other
(= fields of one element), adds sibling globals inside the window, infers field types from the
declared type (output/decls.h) and usage, and writes ApplyStructs.java specs
(struct/field/apply) to names/MFL_structs_arrays.txt.  Only clusters in CURATED (count evidence) are emitted;
--report lists every candidate with its use count and the chain extent so new ones can be curated."""
import re, sys, collections, os
R = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
SZ = {"undefined": 1, "undefined1": 1, "byte": 1, "char": 1, "uchar": 1, "undefined2": 2, "short": 2, "ushort": 2, "undefined4": 4,
      "int": 4, "uint": 4, "float": 4, "long": 4, "ulong": 4, "double": 8, "undefined8": 8}
NUM = r"(0x[0-9a-f]+|\d+)"
# curated: first global of the array -> (struct name, element count, evidence); counts must be proven by hand
CURATED = {  # first global -> (struct, count, evidence, {word index: field name})
    "g_voices": ("VoiceSlot", 210, "sound voice table: init loop `uVar1 += 8 while < 0x690`; +0 voice object (flags +0x34, audio +0x30), +4 volume",
                 {0: "voice", 1: "volume"}),
    "g_car_7e8b64": ("EventLogArgs", 256, "ring buffer of 256: g_event_log_head wraps at 0xff, parallel to g_event_log_type/time/param",
                     {0: "arg0", 1: "arg1", 2: "arg2"}),
    "g_car_7eab64": ("EventLogData", 256, "same 256 ring buffer (index g_event_log_head-1), 0x40 byte copy source (0xc dwords copied)", {}),
    "g_trail_653040": ("TrailSlot", 6, "trail_update loop `iVar7 < 6`, 6*0x2c ends at g_trail_653148", {1: "active", 9: "object_a", 10: "object_b"}),
    "g_spring_grid_88a860": ("SpringNode", 20 * 23, "spring grid init: 23 columns (`< 0x17`) x 20 rows (`0x13 < row` exit), index row*0x17+col",
                             {}),
    "g_fx_651960": ("FxSlot", 10, "fx_spawn ring: g_fx_spawn_651aac wraps at 10, init loop ends at 0x651aa0 (10*0x20)",
                    {0: "clump", 1: "life:f32", 2: "phase", 3: "active", 4: "fx_system", 5: "pos_x:f32", 6: "pos_y:f32", 7: "pos_z:f32"}),
}

def load_decls():
    d = {}
    for m in re.finditer(r"/\*([0-9a-f]{8})\*/ extern (\S+(?: \S+)*?) \**(\w+)(?:\[\d+\])?;", open(R + "/output/decls.h", errors="replace").read()):
        d[m.group(3)] = (int(m.group(1), 16), m.group(2))
    return d

def bracket(t, i):  # t[i]=='[' -> index of matching ']'
    n = 0
    for j in range(i, len(t)):
        n += (t[j] == "[") - (t[j] == "]")
        if n == 0: return j
    return -1

def top_mul(ix):
    """'expr * N' or 'expr * N + c' with a simple expr -> (expr, N, c) else None"""
    m = re.fullmatch(r"(.+?) \* " + NUM + r"(?: \+ " + NUM + ")?", ix)
    if not m or re.search(r" [-+] ", re.sub(r"\([^()]*\)", "", m.group(1))): return None
    return m.group(1), int(m.group(2), 0), int(m.group(3) or "0", 0)

def scan(text, decl):
    """-> uses[name][stride] = count; ptr[name] = count of `(&g)[..] + off` deref (pointer element)"""
    uses = collections.defaultdict(collections.Counter); ptr = collections.Counter()
    for m in re.finditer(r"\(&(g_\w+)\)\[", text):
        g = m.group(1)
        if g not in decl: continue
        e = bracket(text, m.end() - 1)
        r = top_mul(text[m.end():e])
        if not r: continue
        s = SZ.get(decl[g][1].split()[0], 4)
        uses[g][r[1] * s] += 1
        if re.match(r"\s*\+ (0x)?[0-9a-f]+\)", text[e + 1:e + 12]): ptr[g] += 1
    for m in re.finditer(r"(\(int\))?&(g_\w+) \+ (\w+(?:\.\w+)?|\([^()]*\)) \* " + NUM + r"(?![\w.])", text):
        g = m.group(2)
        if g not in decl: continue
        s = 1 if m.group(1) else SZ.get(decl[g][1].split()[0], 4)
        uses[g][int(m.group(4), 0) * s] += 1
    return uses, ptr

def isfloat(text, g):
    return bool(re.search(r"\((?:float|RwReal)\)\(&%s\)\[|\(&%s\)\[[^\]]*\] = (?:fVar|k_f_|_float)|_%s\b" % (g, g, g), text))

def ftype(t, isptr):
    t = t.split()[0]
    if t == "float": return "f32", "float"
    if t in ("short", "ushort", "undefined2"): return ("i16" if t == "short" else "u16"), ""
    if t in ("byte", "char", "undefined1", "uchar"): return "u8", ""
    if t == "int": return "i32", ""
    return "u32", "pointer to a record (+off accessed)" if isptr else ""

def clusters(uses, decl):
    byaddr = sorted((a, g) for g, (a, _) in decl.items())
    out = []
    for K in sorted({k for u in uses.values() for k in u}):
        if K < 8 or K > 0x1000: continue
        gs = sorted((decl[g][0], g) for g, u in uses.items() if u and u.most_common(1)[0][0] == K)
        cur = []
        for a, g in gs:
            if cur and a - cur[0][0] >= K: out.append((K, cur)); cur = []
            cur.append((a, g))
        if cur: out.append((K, cur))
    res = []
    for K, c in out:
        base = c[0][0]
        mem = [(a, g) for a, g in byaddr if base <= a < base + K]
        n = sum(sum(uses[g].values()) for _, g in c)
        chain = base
        for a, g in byaddr:
            if a >= base and a - chain < K: chain = a
            elif a > chain: break
        res.append(dict(K=K, base=base, first=mem[0][1], members=mem, uses=n, nev=len(c), chain=(chain + 4 - base + K - 1) // K))
    return res

def existing_ranges():
    """address ranges applied by the other struct specs (so arrays never overlap them)"""
    import glob
    out = []
    for f in glob.glob(R + "/names/MFL_structs*.txt"):
        if f.endswith("_arrays.txt"): continue
        size = {}; cur = None
        L = open(f).read().splitlines()
        for l in L:
            p = l.strip().split(",")
            if p[0] == "struct": size[p[1]] = int(p[2], 0)
        for l in L:
            p = l.strip().split(",")
            if p[0] in ("apply", "applyptr") and p[2] in size:
                a = int(p[1], 16); n = int(p[3]) if len(p) > 3 and p[0] == "apply" else 1
                out.append((a, a + size[p[2]] * n, "%s %s" % (p[2], p[1])))
    return out

def main():
    a = [x for x in sys.argv[1:] if not x.startswith("--")]
    decl = load_decls()
    text = open(R + "/output/named/MFL_app_named.c", errors="replace").read()
    uses, ptr = scan(text, decl)
    cl = clusters(uses, decl)
    if "--report" in sys.argv:
        for c in sorted(cl, key=lambda c: -c["uses"]):
            if c["uses"] >= 5 and (c["nev"] >= 2 or c["uses"] >= 20):
                print("K=0x%x base=%08x %-34s uses=%4d evid=%d fields=%d minCount=%d%s" % (c["K"], c["base"], c["first"], c["uses"], c["nev"], len(c["members"]), c["chain"], "  CURATED" if c["first"] in CURATED else ""))
        return
    taken = existing_ranges()
    o = ["# generated by tools/infer_global_arrays.py: arrays of structs behind `(&g_X)[i*N]` globals (apply after the other struct specs, before the globals CSVs)"]
    seen = set()
    for c in cl:
        if c["first"] not in CURATED: continue
        name, cnt, why, names = CURATED[c["first"]]
        o += ["", "# %s: %s" % (name, why), "struct,%s,0x%x" % (name, c["K"])]
        for i, (adr, g) in enumerate(c["members"]):
            off = adr - c["base"]
            nxt = c["members"][i + 1][0] - c["base"] if i + 1 < len(c["members"]) else c["K"]
            t, why2 = ftype(decl[g][1], ptr[g] > 0)
            if t == "u32" and isfloat(text, g): t, why2 = "f32", "float by usage"
            if SZ.get(decl[g][1].split()[0], 4) > nxt - off: t = "u8"
            fname, _, ft = names.get(off // 4, "w%02x" % off).partition(":")
            t = ft or t
            o.append("field,0x%02x,%s,%s%s" % (off, t, fname, ("," + why2) if why2 and not ft else ""))
        o.append("apply,0x%08x,%s,%d" % (c["base"], name, cnt))
        for lo, hi, what in taken:
            assert hi <= c["base"] or lo >= c["base"] + c["K"] * cnt, "%s overlaps %s" % (name, what)
    open(a[0] if a else R + "/names/MFL_structs_arrays.txt", "w").write("\n".join(o) + "\n")

if __name__ == "__main__": main()
