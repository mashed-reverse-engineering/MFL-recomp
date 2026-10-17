#!/usr/bin/env python3
"""t2_round.py: kind `round` (REAL, not a harness limit).  The original rounds an x87 temporary to float at a spill point (`fstp dword [esp+N]` ... `fld dword [esp+N]`), our compile keeps the 80 bit value in the register:
1 ulp differences in the stored floats.  Evidence from the ORIGINAL asm: more float spills (fst / fstp dword to the stack frame) in the original function than in our object.  Then the float product / quotient terms
of `lv = a * b * c + d;` statements (and whole right sides) are tried one at a time as `volatile float rfN` temporaries (`rfN = a * b * c; lv = rfN + d;`), each verified by diffemu (t2_auto stage `round`).
The volatile temporaries are in the cheatgrep allowlist (src/cheat_allow.txt) for the functions that pass.
candidates(text, name) -> [(label, new text)]   deficit(name, objpath) -> original spills - our spills   (None when unknown)"""
import os, re, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(HERE, ".."))
SPILL = re.compile(r"\bfstp?\s+DWORD PTR \[(?:esp|ebp)[+-]")
def _count(o): return len(SPILL.findall(o))
def deficit(name, obj):
    try:
        import t2_postfix as P, reginputs2 as R
        P.orig_const_args(name, set()); lo, sz = P._FN[name]
        o = subprocess.run(["objdump", "-w", "-M", "intel", "-D", "--no-show-raw-insn", "--start-address=0x%x" % lo, "--stop-address=0x%x" % (lo + sz), R.EXE], capture_output=True, text=True).stdout
        ours = subprocess.run(["objdump", "-d", "-M", "intel", "--no-show-raw-insn", obj], capture_output=True, text=True).stdout
        return _count(o) - _count(ours)
    except Exception as ex: print("t2_round.deficit %s: %s" % (name, ex), flush=True); return None
def oneulp(txt):
    """True when the first diffemu difference looks like a rounding difference: a frame word `orig [x] ours [y]` with |x - y| <= 2, or memory bytes at a word's LOW byte that differ by <= 3 (no call trace / register difference is checked here)"""
    for m in re.finditer(r"orig \[([-\d, ]+)\] ours \[([-\d, ]+)\]", txt):
        for a, b in zip(m.group(1).split(","), m.group(2).split(",")):
            try:
                if a.strip() != b.strip() and abs(int(a) - int(b)) <= 2: return True
            except ValueError: pass
    mm = re.search(r"memory: \d+ bytes differ: ([^|\n]*)", txt)
    if mm:
        bs = re.findall(r"([0-9a-f]{8}) ([0-9a-f]{2})/([0-9a-f]{2})", mm.group(1))
        return bool(bs) and all(int(a, 16) % 4 == 0 and abs(int(x, 16) - int(y, 16)) <= 3 for a, x, y in bs)
    return False
def _terms(e):
    """top level +/- terms of the expression e -> [(start, end)] (offsets into e)"""
    out, d, st = [], 0, 0
    for i, c in enumerate(e):
        if c in "([": d += 1
        elif c in ")]": d -= 1
        elif d == 0 and c in "+-" and i > 0 and e[:i].strip() and e[:i].rstrip()[-1] not in "*/+-(<>=!&|,%" and not (c == "-" and e[i + 1:i + 2] == ">"):
            out.append((st, i)); st = i + 1
    out.append((st, len(e))); return out
def _top_ops(s, ops="*/"):
    d = 0
    for i, c in enumerate(s):
        if c in "([": d += 1
        elif c in ")]": d -= 1
        elif d == 0 and c in ops and not (c == "*" and (i == 0 or s[:i].strip()[-1:] in "(*/+-,=") ) and not (c == "/" and s[i:i + 2] in ("/*", "//")): return True
    return False
def candidates(t, nm, maxn=14, k0=0):
    m = re.search(r"^[A-Za-z_][^\n;]*\b%s\([^;{]*\)\s*\n\{[ \t]*\n" % re.escape(nm), t, re.M)
    if not m: return []
    top = m.end(); out = []; k = k0
    for sm in re.finditer(r"^([ \t]+)([A-Za-z_*][\w\[\]\.>\-\*]*(?:\[[^\]\n]*\])?(?:\.\w+)*) = ([^;\n]*(?:\*|/)[^;\n]*);[ \t]*$", t[top:], re.M):
        ind, lv, rhs = sm.group(1), sm.group(2), sm.group(3); a = top + sm.start()
        if re.search(r"\?|\bsin\(|\bcos\(|\bsqrt\(|\bfabs\(|\b_ftol|[A-Za-z_]\w*\(\s*[^()]*\)\s*[*/+-]\s*[A-Za-z_]\w*\(", rhs): pass
        pl = t[:a].rstrip().split("\n")[-1].rstrip()
        if pl.endswith((")", "else")): continue
        if re.search(r"(?<![\w)\]])\(\s*(?:int|uint|short|char)\s*\)|\(float\)\s*\(?\s*\w+\s*\(", rhs) and not re.search(r"\(float\)\s*\(int\)", rhs): continue
        ts = _terms(rhs); cands = []
        if len(ts) > 1:
            for s_, e_ in ts:
                x = rhs[s_:e_].strip()
                if _top_ops(x) and not x.startswith("*(") and "(" not in x.replace("(float)", "").replace("(int)", "").replace("(RwReal)", "") or (_top_ops(x) and len(ts) > 1 and x.count("(") == x.count(")")): cands.append((rhs[s_:e_], x))
        elif _top_ops(rhs) and rhs.count("*") + rhs.count("/") >= 2: cands.append((rhs, rhs.strip()))
        for full, x in cands:
            if len(x) < 6 or re.search(r"\b(float10|double)\b", x): continue
            k += 1; v = "rf%d" % k
            line = "%s%s = %s;" % (ind, lv, rhs.replace(full.strip(), v, 1))
            prev = t[:a].rstrip("\n").rsplit("\n", 1)      # placement 0: right before the statement; 1: before the previous simple assignment (the original computes the product first)
            pm = re.match(r"^[ \t]+([A-Za-z_*][\w\[\]\.>\-\*]*) = [^;{}]*;[ \t]*$", prev[-1]) if len(prev) > 1 else None
            for pl_ in (0, 1):
                if pl_ == 1:
                    if not pm or re.search(r"\b%s\b" % re.escape(re.sub(r"\[.*", "", pm.group(1)).lstrip("*")), x): continue
                    at = a - len(prev[-1]) - 1
                else: at = a
                n = t[:at] + "%s%s = %s;\n" % (ind, v, x) + t[at:a] + line + t[a + len(sm.group(0)):]
                n = n[:top] + "  volatile float %s;\n" % v + n[top:]
                out.append(("%s := %s (placement %d)" % (v, x, pl_), n))
            if len(out) >= maxn: return out
    return out
if __name__ == "__main__":
    for f in sys.argv[1:]:
        t = open(f).read(); nm = os.path.basename(f)[:-2]
        for lab, n in candidates(t, nm): print(f, lab)
