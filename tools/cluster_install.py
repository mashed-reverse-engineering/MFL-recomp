#!/usr/bin/env python3
"""cluster_install.py CLUSTER... [--dir output/cluster_pilotN] [--force]   |   --list   |   --entries CLUSTER...   |   --remove CLUSTER...
Installs verified clusters (output/cluster_pilot*/, `diffemu --cluster`, all entries EQUAL) as build units of src/clusters/ (README "Cluster install").
For every CLUSTER (= root name of a clusters.txt row) it writes src/clusters/cl_<root>.c and appends <root> to src/clusters/enabled.txt:
  - the natural C of the pilot file (root + callers, members keep their names: build_overlay.py --clusters patches `jmp` to them at their original addresses);
  - root with register inputs / `ret N` (output/reginputs.csv): the root's natural C is renamed ROOT_n and a naked ENTRY THUNK ROOT (bridgegen.gen_stub, same code as the 92 bridges) is
    generated at the end: loads the register inputs / copies the stack arguments in the clusters.txt order, calls ROOT_n, keeps the caller saved registers the original never wrote,
    `ret N`.  A plain root (no register input, ret 0) needs no thunk;
  - stub option `callee=@esi,s1` (callee outside the cluster that the original calls with registers): the natural call in the C becomes a call of a generated naked adapter
    callee_r (bridgegen.gen_adapter) that sets the registers and calls the original;
  - first line `// cluster-entries: a b c` (read by build_overlay.py --clusters: those definitions are removed from every other source of the build).
Refuses: an entry whose original is < 5 bytes (no room for the jmp), a verdict other than EQUAL, a root that is an ENABLED bridge (two implementations)."""
import csv, glob, os, re, sys
HERE = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HERE)
G = os.path.join(HERE, "..")
CL = os.path.join(G, "src", "clusters")
EN = os.path.join(CL, "enabled.txt")


def specs(only_dir=None):
    out = {}
    for f in sorted(glob.glob(os.path.join(G, "output", "cluster_pilot*", "clusters.txt"))):
        d = os.path.normpath(os.path.dirname(f))      # G contains ".." (HERE/..): compare normalised paths
        if only_dir and os.path.abspath(only_dir) != d: continue
        for l in open(f):
            if not l.strip() or l.startswith("#"): continue
            p = [x.strip() for x in l.split("|")]
            out[p[0]] = dict(root=p[0], dir=d, file=os.path.join(d, p[1]), entries=p[2].split(), order=p[3].split(), opts=re.split(r"[;\s]+", p[4]) if len(p) > 4 and p[4] else [])
    return out


def verdicts(d):
    v = {}
    p = os.path.join(d, "results.csv")
    if os.path.exists(p):
        for r in csv.DictReader(open(p)): v[r["name"]] = r["verdict"]
    return v


def enabled():
    return [l.strip() for l in open(EN) if l.strip() and not l.startswith("#")] if os.path.exists(EN) else []


def entries_of(name):
    """entries (functions patched / removed from other sources) of an installed cluster, read from its file"""
    t = open(os.path.join(CL, "cl_%s.c" % name)).readline()
    return t.split(":", 1)[1].split()


def noncomment(text, fn):
    """apply fn to the text outside /* */ and // comments"""
    return re.sub(r"/\*.*?\*/|//[^\n]*|[^/]+|/", lambda m: m.group(0) if m.group(0)[:2] in ("/*", "//") else fn(m.group(0)), text, flags=re.S)


def install(c, force):
    import bridgegen as bg, clobber_util
    ri = {r["name"]: r for r in csv.DictReader(open(os.path.join(G, "output", "reginputs.csv")))}
    fs = {}
    for l in open(os.path.join(G, "output", "fnsizes.txt")):
        q = l.split()
        if len(q) == 3: fs[q[1]] = (int(q[0], 16), int(q[2]))
    try:      # <cluster dir>/fnsizes_extra.txt (`va name size`, true function size): entries fnsizes.txt does not list (gap_ items: output/callgraph_waves.csv bytes are the size of the whole gap item)
        for l in open(os.path.join(c["dir"], "fnsizes_extra.txt")):
            q = l.split()
            if len(q) == 3: fs[q[1]] = (int(q[0], 16), int(q[2]))
    except OSError: pass
    v = verdicts(c["dir"])
    bad = [e for e in c["entries"] if v.get(e) != "EQUAL"]
    if bad and not force: sys.exit("%s: not EQUAL in %s/results.csv: %s" % (c["root"], c["dir"], " ".join(bad)))
    for e in c["entries"]:
        if e not in fs or fs[e][1] < 5: sys.exit("%s: entry %s has no known size >= 5 bytes (fnsizes.txt): no room for the jmp" % (c["root"], e))
        if e not in ri: sys.exit("%s: entry %s not in reginputs.csv" % (c["root"], e))
    root = c["root"]
    if root in open(os.path.join(G, "src", "bridges", "enabled.txt")).read().split():
        sys.exit("%s is an ENABLED bridge: remove it from src/bridges/enabled.txt first (two implementations of one function)" % root)
    text = open(c["file"]).read()
    r = ri[root]
    regs = [x for x in r["reg_inputs"].split("|") if x]
    ret_n = int(r["ret_n"])
    gen, adapters, protos = [], [], []
    # ---- stub option callee=@regs: adapter for callees outside the cluster that take registers in the original
    for o in c["opts"]:
        m = re.match(r"(\w+)=@(.+)", o)
        if not m: continue
        cn, where = m.group(1), m.group(2).split(",")
        cm = re.search(r"^(?:/\*[0-9a-f]{8}\*/ )?extern\s+(.*?)\b%s\s*\((.*?)\)\s*;[^\n]*\n" % cn, text, re.M)
        if not cm: sys.exit("%s: no extern line for the @ callee %s" % (root, cn))
        ret = re.sub(r"\b__(?:cdecl|stdcall)\b|\bextern\b", "", cm.group(1)).strip()
        cparams = re.sub(r"/\*.*?\*/", "", cm.group(2))      # comments inside the parameter list (`int slot /* eax */`) would end up in the generated adapter prototype
        pt = [re.sub(r"\b__(?:cdecl|stdcall)\b", "", a).strip() for a in cparams.split(",")] if cparams.strip() not in ("", "void") else []
        if len(pt) != len(where): sys.exit("%s: callee %s has %d parameters, option lists %d" % (root, cn, len(pt), len(where)))
        params = []
        for i, (t, w) in enumerate(zip(pt, where)):
            t = re.sub(r"\s*\b\w+$", "", t) if re.search(r"[\w\*]\s+\**\w+$", t) else t           # drop the parameter name
            params.append((t, "p%d" % i, "reg", w) if not re.fullmatch(r"s\d+", w) else (t, "p%d" % i, "stack", (4 + 4 * int(w[1:]), 4)))
        f = dict(name=cn, ret=ret, params=params, purge=int(ri.get(cn, {"ret_n": 0})["ret_n"]))
        text = text.replace(cm.group(0), "")
        text = noncomment(text, lambda s, cn=cn: re.sub(r"\b%s\b(?=\s*\()" % cn, cn + "_r", s))
        protos += ["static %s;" % bg.proto_of(f, cn + "_r"), "extern void %s(void);" % cn]
        adapters.append("/* %s_r: C -> original %s (registers %s) */\n%s" % (cn, cn, ",".join(where), bg.gen_adapter(f)))
    # ---- entry thunk of the root
    if regs or ret_n:
        dm = re.search(r"^(?!extern|static|typedef)([A-Za-z_][\w \*]*?)\b%s\s*\(" % root, text, re.M)
        if not dm: sys.exit("%s: definition of the root not found" % root)
        rtype = re.sub(r"\b__(?:cdecl|stdcall)\b", "", dm.group(1)).strip()
        text = noncomment(text, lambda s: re.sub(r"\b%s\b" % root, root + "_n", s))
        params = []
        for w in c["order"]:
            params.append(("int", w, "reg", w) if not re.fullmatch(r"s\d+", w) else ("int", w, "stack", (4 + 4 * int(w[1:]), 4)))
        if {p[3] for p in params if p[2] == "reg"} != set(regs): sys.exit("%s: clusters.txt order %s != register inputs %s of reginputs.csv" % (root, c["order"], regs))
        a, size = fs[root]
        written = clobber_util.written(bg.func_bytes(a, a + size))
        f = dict(name=root, ret=rtype, params=params, purge=ret_n)
        gen.append("/* entry thunk at the original address of %s: register inputs (%s) -> natural C %s_n; ret %d */\n%s" % (root, ",".join(c["order"]), root, ret_n, bg.gen_stub(f, written, root + "_n")))
    if protos:      # before the first function definition (and its comment): the adapter prototypes use the types the file defines
        dm = re.search(r"^(?!extern|typedef|#|/\*|//|\s)[A-Za-z_][^;{}\n]*\)\s*\n\s*\{", text, re.M)
        if not dm: sys.exit("%s: no function definition found" % root)
        k = dm.start()
        while True:
            pl = text.rfind("\n", 0, k - 1) + 1
            if text[pl:k].lstrip().startswith(("/*", "//")) and text[pl:k].strip(): k = pl
            else: break
        text = text[:k] + "\n".join(protos) + "\n" + text[k:]
    head = "// cluster-entries: %s\n/* Installed cluster %s (tools/cluster_install.py from %s); entries are patched at their original addresses by build_overlay.py --clusters. */\n" % (" ".join(c["entries"]), root, os.path.relpath(c["file"], G))
    os.makedirs(CL, exist_ok=True)
    open(os.path.join(CL, "cl_%s.c" % root), "w").write(head + text.rstrip("\n") + "\n\n" + "\n".join(adapters + gen))
    if root not in enabled(): open(EN, "a").write(root + "\n")
    print("installed %s: %d entries, %d bytes of original code%s" % (root, len(c["entries"]), sum(fs[e][1] for e in c["entries"]), ", thunk" if gen else ", no thunk (plain root)"))


def main():
    a = sys.argv[1:]
    d = a[a.index("--dir") + 1] if "--dir" in a else None
    sp = specs(d)
    if "--list" in a:
        for n, c in sp.items():
            v = verdicts(c["dir"]); ok = all(v.get(e) == "EQUAL" for e in c["entries"])
            print("%-36s %-8s %s %s" % (n, "EQUAL" if ok else "not-EQUAL", "installed" if n in enabled() else "", os.path.basename(c["dir"])))
        return
    if "--entries" in a:
        print(" ".join(e for n in a[a.index("--entries") + 1:] for e in entries_of(n))); return
    if "--remove" in a:
        rm = set(a[a.index("--remove") + 1:])
        keep_ = [n for n in enabled() if n not in rm]      # read BEFORE the file is truncated (open(EN, "w") first emptied the list)
        open(EN, "w").write("".join(n + "\n" for n in keep_))
        for n in rm:
            if os.path.exists(os.path.join(CL, "cl_%s.c" % n)): os.unlink(os.path.join(CL, "cl_%s.c" % n))
        return
    names = [x for i, x in enumerate(a) if not x.startswith("--") and (i == 0 or a[i - 1] != "--dir")]
    if not names: sys.exit(__doc__)
    for n in names:
        if n not in sp: sys.exit("unknown cluster %s (--list)" % n)
        install(sp[n], "--force" in a)


main()
