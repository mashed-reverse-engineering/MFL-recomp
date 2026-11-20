#!/usr/bin/env python3
"""tailarg_scan.py: TAIL detector (W17 DET; productionised agents/pause_w16/tailarg.py).  The gap_5da500 class: an ORIGINAL function that
rewrites one of its own incoming stack arguments (`mov DWORD PTR [esp+N],reg|imm`) and then tail-jumps to another function, i.e.
`return T(new_arg, ...)`.  Ghidra shows `T();` with no arguments; a recomp body written that way calls T with the caller's ORIGINAL
argument (pre-pse1 gap_5da500 freed the wave instead of its sample block: 0xc0000374 on Windows).

  tailarg_scan.py [--src <recomp dir>] [--orig <MFL.exe>] [--overlay <file.c> ...] [--out <report>]
  tailarg_scan.py --selftest

For each such site: the recomp definition of the containing function (any name of its address in src/symbols.txt + aliases.txt), the
calls in its body to any name of the jump target, and the argument count of each call.  BAD = every call passes fewer arguments than the
rewritten slot needs (slot k = argument k+1) while the target's recomp definition / prototype takes at least k+1 parameters (or is
unprototyped).  NOCALL / NODEF rows are informational (the body calls another spelling, or the function is asm / not recomped).
--overlay: use this file instead of the recomp file with the same base name (planted-bug proofs).  Exit 1 = a BAD row, 2 = input error."""
import bisect, os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ret_scan as R

G = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


def names_of(src):
    """address -> set of names, from src/symbols.txt and aliases.txt; plus the sorted function starts"""
    names, starts = {}, set()
    for f in ("symbols.txt", "aliases.txt"):
        p = os.path.join(src, f)
        if not os.path.exists(p): continue
        for l in open(p, errors="replace"):
            q = l.split()
            if len(q) >= 2 and re.fullmatch(r"[0-9a-fA-F]{8}", q[1]):
                a = int(q[1], 16)
                names.setdefault(a, set()).add(q[0])
                if not q[0].startswith(("LAB_", "switchD", "caseD", "_L")) and q[0] != "default": starts.add(a)      # `default` = a switch label
    return names, sorted(starts)


def sites(ins, starts, names):
    """[(F, T, slot index k, text)] for every tail jmp to another function after an argument-slot store"""
    out = []
    for i, (a, mn, op) in enumerate(ins):
        if mn != "jmp": continue
        t = R.tgt(op)
        if t is None or t not in names: continue
        k = bisect.bisect_right(starts, a) - 1
        if k < 0: continue
        f = starts[k]
        nxt = starts[k + 1] if k + 1 < len(starts) else f + 0x10000
        if f <= t < nxt or not any(not n.startswith(("LAB_", "switchD", "caseD", "_L")) for n in names[t]): continue      # in-function jump
        pops, j = 0, i - 1
        while j >= 0 and j >= i - 6:
            b, m2, o2 = ins[j]
            if m2 in ("call", "ret") or m2.startswith("j") or b < f: break
            if m2 == "pop": pops += 4
            elif m2 == "add" and o2.startswith("esp,") and R.HEX.fullmatch(o2[4:]): pops += int(R.HEX.fullmatch(o2[4:]).group(1), 16)
            m = re.fullmatch(r"DWORD PTR \[esp\+0x([0-9a-f]+)\],(e\w\w|0x[0-9a-f]+)", o2) if m2 == "mov" else None
            if m:
                n = int(m.group(1), 16) - pops      # offset at entry (the jmp runs with the entry esp)
                if n >= 4 and n % 4 == 0: out.append((f, t, n // 4 - 1, "%x mov %s" % (b, o2)))
            j -= 1
    return out


DEF = re.compile(r"^((?:[A-Za-z_][\w\*]*[ \t\*\n]+)+?)(\w+)[ \t]*\(", re.M)


def balanced(s, k, o="(", c=")"):
    """index after the bracket that closes the one opened just before s[k]"""
    d = 1
    while k < len(s) and d:
        d += {o: 1, c: -1}.get(s[k], 0); k += 1
    return k


def nargs(text):
    t = re.sub(r"/\*.*?\*/", "", text, flags=re.S).strip()
    if t in ("", "void"): return 0
    d, n = 0, 1
    for ch in t:
        if ch in "([{": d += 1
        elif ch in ")]}": d -= 1
        elif ch == "," and d == 0: n += 1
    return n


def read_src(src, overlay):
    """name -> (file, params count, body) of each definition; name -> max params of any prototype with a parameter list"""
    ov = {os.path.basename(p): p for p in overlay}
    defs, protos = {}, {}
    for d, _, files in os.walk(src):
        for fn in files:
            if not fn.endswith((".c", ".h")): continue
            s = re.sub(r"__declspec\s*\(\s*naked\s*\)", "__declspec_naked", open(ov.get(fn, os.path.join(d, fn)), errors="replace").read())
            for m in DEF.finditer(s):
                if R.NOTTYPE & set(re.findall(r"\w+", m.group(1))): continue
                e = balanced(s, m.end())
                params = s[m.end():e - 1]
                rest = re.sub(r"/\*.*?\*/|//[^\n]*", "", s[e:e + 200], flags=re.S).lstrip()
                if rest[:1] == "{" and fn.endswith(".c"):
                    b = s.find("{", e)
                    defs.setdefault(m.group(2), (os.path.join(d, fn), nargs(params), s[b:balanced(s, b + 1, "{", "}")]))
            for m in re.finditer(r"\b(\w+)\s*\(([^;{}()]*(?:\([^()]*\)[^;{}()]*)*)\)\s*;", s):      # prototypes (and calls; only counts > 0 matter)
                if "extern" in s[max(0, m.start() - 80):m.start()].split("\n")[-1] and m.group(2).strip():
                    protos[m.group(1)] = max(protos.get(m.group(1), 0), nargs(m.group(2)))
    return defs, protos


def calls(body, name):
    """argument counts of the calls `name(...)` in body"""
    out = []
    for m in re.finditer(r"\b%s\s*\(" % re.escape(name), body):
        e = balanced(body, m.end())
        out.append(nargs(body[m.end():e - 1]))
    return out


def scan(ins, src, overlay=()):
    names, starts = names_of(src)
    defs, protos = read_src(src, overlay)
    rows, bad = [], 0
    for f, t, k, txt in sites(ins, starts, names):
        fn = sorted(n for n in names.get(f, ()) if n in defs)
        tn = sorted(names.get(t, ()))
        if not fn:
            rows.append("NODEF %08x %s jmp %08x %s | slot %d | %s" % (f, "/".join(sorted(names.get(f, ["?"]))), t, "/".join(tn), k, txt)); continue
        path, _, body = defs[fn[0]]
        need = max([defs[n][1] for n in tn if n in defs] + [protos.get(n, 0) for n in tn] + [0])
        cs = [c for n in tn for c in calls(body, n)]
        if re.search(r"\b_?_asm\b", body): kind = "asm"      # the asm body is the original instruction sequence
        elif not cs: kind = "NOCALL"
        elif max(cs) < k + 1 and (need == 0 or need >= k + 1): kind = "BAD"; bad += 1      # need 0 = unprototyped / unknown: the original passes k+1
        else: kind = "ok"
        rows.append("%s %08x %s jmp %08x %s | slot %d, target takes %s, calls pass %s | %s | %s" % (
            kind, f, fn[0], t, "/".join(tn), k, need or "?", cs or "-", txt, os.path.relpath(path, src)))
    return rows, bad


SELF_ASM = """
 5da500:	mov    eax,DWORD PTR [esp+0x4]
 5da504:	mov    DWORD PTR [eax+0x50],0x0
 5da50b:	mov    eax,DWORD PTR [eax+0x4c]
 5da50e:	mov    DWORD PTR [esp+0x4],eax
 5da512:	jmp    0x5bf120
 5da520:	ret
"""


def selftest():
    import tempfile
    ins = R.parse(SELF_ASM.splitlines())
    with tempfile.TemporaryDirectory() as d:
        open(d + "/symbols.txt", "w").write("gap_5da500 005da500\nFUN_005bf120 005bf120\nnext_5da520 005da520\n")
        open(d + "/aliases.txt", "w").write("rwa_free_005bf120 005bf120\n")
        open(d + "/free.c", "w").write("void rwa_free_005bf120(void *block)\n{\n  RwFree(block);\n}\n")
        res = {}
        for tag, body in (("bad", "  FUN_005bf120();\n"), ("good", "  FUN_005bf120(*(void **)(wave + 0x4c));\n")):
            open(d + "/an_hk_1336.c", "w").write("extern int FUN_005bf120();\nvoid gap_5da500(char *wave)\n{\n  *(int *)(wave + 0x50) = 0;\n%s}\n" % body)
            rows, bad = scan(ins, d)
            res[tag] = (bad, rows)
    assert res["bad"][0] == 1 and res["bad"][1][0].startswith("BAD "), res["bad"]
    assert res["good"][0] == 0 and res["good"][1][0].startswith("ok "), res["good"]
    print("tailarg_scan selftest: OK (planted pre-pse1 gap_5da500 flagged, the fixed body passes)")
    return 0


if __name__ == "__main__":
    a = sys.argv[1:]
    if a == ["--selftest"]: sys.exit(selftest())
    opt, ovl, k = {"--src": G + "/src", "--orig": R.ORIG, "--out": None}, [], 0
    while k < len(a):
        if a[k] == "--overlay" and k + 1 < len(a): ovl.append(a[k + 1])
        elif a[k] in opt and k + 1 < len(a): opt[a[k]] = a[k + 1]
        else: sys.exit("unknown or incomplete option %r\n%s" % (a[k], __doc__))
        k += 2
    for p in [opt["--src"], opt["--orig"]] + ovl:
        if not os.path.exists(p): print("missing input " + p); sys.exit(2)
    rows, bad = scan(R.objdump(opt["--orig"]), opt["--src"], ovl)
    rows.sort(key=lambda x: ({"BAD": 0, "NOCALL": 1, "NODEF": 2, "asm": 3, "ok": 4}[x.split()[0]], x))
    summ = "tailarg_scan: %d tail calls after an argument-slot store: %d BAD, %s -> %s" % (len(rows), bad, ", ".join(
        "%d %s" % (sum(r.startswith(k + " ") for r in rows), k) for k in ("NOCALL", "NODEF", "asm", "ok")), "FAIL" if bad else "PASS")
    if opt["--out"]: open(opt["--out"], "w").write("\n".join(rows + [summ]) + "\n")
    print("\n".join([r for r in rows if r.startswith("BAD")][:40] + [summ]))
    sys.exit(1 if bad else 0)
