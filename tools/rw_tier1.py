#!/usr/bin/env python3
"""rw_tier1.py [--flags "/O2 /G5"] [--dry] <draft.c> <spec>: split a draft file of RenderWare library functions into one tier-1 file each.
spec: lines `name@va  one line description` (# comments).  For each function: cut it out of the draft with the extern / #define lines it uses
(extern addresses from src/symbols.txt), verify THAT standalone file with cmpfn.check (flags default `/O2 /G5`: the RenderWare SDK objects were built by
cl 12 and match cl 13 only with /G5, see public/docs/BYTEMATCH_PATTERNS.md "RenderWare library"), and only then write src/tier1/<name>.c with the
`/* compile: /G5 */` marker that tools/build_overlay.py and tools/diffemu/diffemu.py read.  Anything that does not verify is printed as SKIP and not written.
After the run: tools/check_all.sh, `tools/gate.sh tier1` (under the gate lock)."""
import os, re, shutil, sys, tempfile
HERE = os.path.dirname(os.path.abspath(__file__)); R = os.path.join(HERE, "..", "src")
sys.path.insert(0, HERE)
import cmpfn
a = sys.argv[1:]; flags = ["/O2", "/G5"]; dry = "--dry" in a
if dry: a.remove("--dry")
if "--flags" in a: i = a.index("--flags"); flags = a[i + 1].split(); del a[i:i + 2]
draft, spec = a
sym = {}
for f in ("symbols.txt", "aliases.txt"):
    for l in open(os.path.join(R, f)):
        p = l.split()
        if len(p) >= 2 and re.fullmatch(r"[0-9a-f]{6,8}", p[1]): sym.setdefault(p[0], p[1])
rwp = {}      # proposed Rwp names (output/rwp_names.csv, not applied): quoted in the description
try:
    for l in open(os.path.join(HERE, "..", "output", "rwp_names.csv")):
        q = l.rstrip("\n").split(",", 2)
        if len(q) == 3 and re.fullmatch(r"[0-9a-f]{8}", q[0]): rwp[int(q[0], 16)] = q[1]
except OSError: pass
text = open(draft).read(); lines = text.split("\n")
DEF = re.compile(r"^[A-Za-z_][\w \*]*?\b(\w+)\([^;]*\)\s*\{")
funcs = {}; first = None; i = 0; prel = []
while i < len(lines):
    m = DEF.match(lines[i])
    if not m: prel.append(lines[i]); i += 1; continue
    first = i if first is None else first
    j = i; d = 0; seen = False
    while True:
        d += lines[j].count("{") - lines[j].count("}"); seen = seen or "{" in lines[j]
        if seen and d == 0: break
        j += 1
    funcs[m.group(1)] = "\n".join(lines[i:j + 1]); i = j + 1
pre = "\n".join(prel)      # everything outside the function bodies
defs = [l for l in pre.split("\n") if l.startswith("#define")]
decls = []      # (name, text)
for st in re.findall(r"typedef struct[^{;]*\{[^}]*\}\s*\w+;|extern [^;]*;|typedef [^;]*;", re.sub(r"#[^\n]*", "", pre)):
    st = st.strip()
    if st.startswith("typedef"):
        m = re.search(r"\(\s*(?:__cdecl\s*)?\*\s*(\w+)\s*\)", st) or re.search(r"(\w+)\s*(\[[^\]]*\])?\s*;$", st)
        if m: decls.append((m.group(1), st))
        continue
    body = st[len("extern "):-1]
    m = re.match(r"[^()]*?\b(\w+)\s*\(", body)
    if m: decls.append((m.group(1), st)); continue      # function prototype
    parts = [x.strip() for x in body.split(",")]      # variables: `extern T a, b[], c;`
    m = re.match(r"^(.*?)(\w+)\s*(\[[^\]]*\])?$", parts[0])
    if not m: continue
    ty = m.group(1)
    decls.append((m.group(2), "extern %s;" % parts[0]))
    for x in parts[1:]:
        m = re.match(r"^\**\s*(\w+)\s*(\[[^\]]*\])?$", x)
        if m: decls.append((m.group(1), "extern %s%s;" % (ty, x)))

def allman(body):
    """one-line draft `sig { a; b; }` -> signature line, `{`, one statement per line, `}` (tools/tier_ledger.py finds definitions by the brace on its own line)"""
    if "\n" in body.strip(): return body
    i = body.index("{"); sig = body[:i].strip(); inner = body[i + 1:body.rindex("}")].strip()
    st = []; d = 0; cur = ""
    for k, ch in enumerate(inner):
        cur += ch
        if ch in "({": d += 1
        elif ch in ")}": d -= 1
        if d == 0 and (ch == ";" or (ch == "}" and not re.match(r"\s*(else|while)\b", inner[k + 1:]))):
            st.append(cur.strip()); cur = ""
    if cur.strip(): st.append(cur.strip())
    return sig + "\n{\n" + "\n".join("  " + x for x in st) + "\n}"
tmp = tempfile.mkdtemp(prefix="rwt1_", dir=os.environ.get("TMPDIR", "/tmp"))
for h in os.listdir(R):
    if h.endswith(".h"): shutil.copy(os.path.join(R, h), tmp)
ok = bad = 0; newnames = []
try:
    for ln in open(spec):
        ln = ln.strip()
        if not ln or ln.startswith("#"): continue
        head, _, desc = ln.partition(" ")
        n, va = head.split("@"); va = int(va, 16)
        if n not in funcs: print("SKIP %s: not in the draft" % n); bad += 1; continue
        if sym.get(n, "%08x" % va) != "%08x" % va: print("SKIP %s: the name is bound to %s in the symbol tables, not %08x" % (n, sym[n], va)); bad += 1; continue
        body = funcs[n]; used = set(re.findall(r"[A-Za-z_]\w*", body))
        if re.search(r"\bRw(Free|Malloc)\b|\brwstr\w+|\brwsprintf|RW_GLOBAL", body): used.add("RwEngineInstance")      # the engine macros of mfl_tier2.h read it
        out = ["/* compile: %s */" % " ".join(x for x in flags if x not in ("/O2",)) if len(flags) > 1 else "", '#include "mfl_tier2.h"']
        for dl in defs:      # a used #define pulls in what its text names (ENG -> RwEngineInstance)
            if re.match(r"#define (\w+)", dl).group(1) in used: used |= set(re.findall(r"[A-Za-z_]\w*", dl))
        for dn, dt in decls:      # a declaration pulls in the typedefs it names (typedefs come first in the draft)
            if dn in used: used |= set(re.findall(r"[A-Za-z_]\w*", dt))
        for dn, dt in decls:
            if dn in used:
                ad = sym.get(dn)
                out.append(("/*%s*/ " % ad if ad else "") + dt)
        for dl in defs:
            if re.match(r"#define (\w+)", dl).group(1) in used: out.append(dl)
        if va in rwp: desc = (desc.strip() + " (Rwp library, proposed name %s)" % rwp[va]).strip()
        if desc: out.append("/* %s */" % desc.strip())
        src = "\n".join(x for x in out if x) + "\n" + allman(body) + "\n"
        p = os.path.join(tmp, n + ".c"); open(p, "w").write(src)
        r = cmpfn.check(p, [(n, va)], flags)[n]
        if not r[0]: print("SKIP %s: %s" % (n, r[1][:160].replace("\n", " "))); bad += 1; continue
        if not dry: open(os.path.join(R, "tier1", n + ".c"), "w").write(src)
        ok += 1; newnames.append((n, va))
finally:
    shutil.rmtree(tmp, ignore_errors=True)
if newnames and not dry:      # names that only the library ownership table knows (no Ghidra symbol): alias at the original address, so build_overlay can patch them
    al = os.path.join(R, "aliases.txt"); have = {l.split()[0] for l in open(al) if l.strip() and not l.startswith("#")}
    qn = {}      # callee names of the extern prototypes that only the ownership table knows (output/rw_rewrite_queue.csv): alias them too
    try:
        import csv
        for r in csv.DictReader(open(os.path.join(HERE, "..", "output", "rw_rewrite_queue.csv"))): qn.setdefault(r["name"], []).append(r["va"])
    except OSError: pass
    for n, v in list(newnames):
        for dn, dt in decls:
            if dn in funcs[n] and "(" in dt and dn not in sym and len(qn.get(dn, [])) == 1: newnames.append((dn, int(qn[dn][0], 16)))
    add = sorted({"%s %08x" % (n, v) for n, v in newnames if n not in sym and n not in have})
    if add: open(al, "a").write("".join(x + "\n" for x in add))
    print("%d aliases appended to src/aliases.txt" % len(add))
print("%d verified%s, %d skipped" % (ok, " (dry)" if dry else " and written", bad))
