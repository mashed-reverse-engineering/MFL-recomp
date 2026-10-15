#!/usr/bin/env python3
"""cheatgrep.py [files...]: flags constructs that make a 'match' meaningless in agent / tier 1 / tier 2 sources: __asm / _emit, any #pragma, volatile in a function body,
raw `*(T*)0x......` pointers, raw image data address literals (rawaddr.py), byte/dword arrays (>= 16 constants) that occur in the original MFL.exe.
Scope (default): src/{automatch_agent,automatch_near,tier1,automatch}/*.c (not hand code, bridges, keep_*).  Allowlist: src/cheat_allow.txt, `token reason` per line,
token = file basename or function name (a reason is mandatory).  Exit 1 when anything unallowed is found."""
import glob, os, re, sys
H = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(H, ".."))
sys.path.insert(0, H); import rawaddr
SCOPE = ("automatch_agent", "automatch_near", "tier1", "automatch")
PAT = [("asm", re.compile(r"\b__asm\b|\b_asm\b|\b_{1,2}emit\b")), ("pragma", re.compile(r"^[ \t]*#[ \t]*pragma\b", re.M)),
       ("rawptr", re.compile(r"\*\s*\(\s*[A-Za-z_][\w \*]*\*\s*\)\s*\(?\s*0[xX][0-9a-fA-F]{6}\b"))]
ARR = re.compile(r"\{\s*((?:0[xX][0-9a-fA-F]+\s*,\s*){15,}0[xX][0-9a-fA-F]+)\s*,?\s*\}")
_exe = None
def exe():
    global _exe
    if _exe is None: _exe = open(rawaddr.EXE, "rb").read()
    return _exe
def allow():
    a = {}
    p = os.path.join(G, "src", "cheat_allow.txt")
    if os.path.exists(p):
        for l in open(p):
            q = l.split(None, 1)
            if q and not q[0].startswith("#"):
                if len(q) < 2: sys.exit("cheat_allow.txt: '%s' has no reason" % q[0])
                a[q[0]] = q[1].strip()
    return a
def scan(path):
    t = open(path, errors="surrogateescape").read(); m = rawaddr.mask(t); fs = rawaddr.functions(m); out = []
    def fn(pos): return next((f[0] for f in fs if f[1] <= pos < f[2]), "")
    for k, rx in PAT:
        for x in rx.finditer(m): out.append((k, fn(x.start()), m[:x.start()].count("\n") + 1))
    for x in re.finditer(r"\bvolatile\b", m):
        if fn(x.start()): out.append(("volatile", fn(x.start()), m[:x.start()].count("\n") + 1))
    for x in ARR.finditer(m):
        v = [int(s, 16) for s in re.findall(r"0[xX][0-9a-fA-F]+", x.group(1))]
        for w in (1, 4):
            if max(v) < (1 << 8 * w) and (w == 1 or all(True for _ in v)):
                if b"".join(i.to_bytes(w, "little") for i in v) in exe(): out.append(("codebytes", fn(x.start()), m[:x.start()].count("\n") + 1)); break
    try: out += [("rawdata", h[3], t[:h[0]].count("\n") + 1) for h in rawaddr.scan(path)[1]]
    except Exception as e: out.append(("scanerror %s" % e, "", 0))
    return out
def main(argv):
    fl = [os.path.abspath(a) for a in argv] or sorted(sum((glob.glob(os.path.join(G, "src", d, "*.c")) for d in SCOPE), []))
    al = allow(); bad = 0
    for p in fl:
        for k, f, ln in scan(p):
            if os.path.basename(p) in al or any(x in al for x in f.split(",")): continue
            bad += 1; print("FAIL:cheat %s:%d %s in %s" % (os.path.relpath(p, G), ln, k, f or "-"))
    print("cheatgrep: %d file(s), %d hit(s)" % (len(fl), bad)); return 1 if bad else 0
if __name__ == "__main__": sys.exit(main(sys.argv[1:]))
