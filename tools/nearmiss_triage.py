#!/usr/bin/env python3
"""nearmiss_triage.py <autocompile dir (ac, with ok_*.c)> <automatch.csv> [out=output/nearmiss_triage.csv] [jobs=2]
Compiles every chunk with a non-MATCH function once (cmpfn.check), diffs ours vs original instruction by instruction and puts each function in ONE category with the cheapest tool
that might fix it.  Output csv: name,va,size,diff_lines,category,tool,chunk.  Prints counts per category and a histogram of diff sizes.
No LLM involved; the categories are the routing table for cheap models (README 'Matching toolbox')."""
import collections, csv, difflib, os, re, sys
from concurrent.futures import ThreadPoolExecutor
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import cmpfn, asmdiff

REG = re.compile(r"\b(e?[abcd]x|e?[sd]i|e?bp|[abcd][lh])\b")
JCC = re.compile(r"^j[a-z]+$")
TOOL = {
    "REGALLOC": "permute_one.sh / variants (locals_struct, declaration order)",
    "OPERAND_ORDER": "variants swap/flip (cheap)",
    "CONST": "variants cmpconst/sign/gsign (cheap)",
    "SCHED": "permute_one.sh; else Opus (statement order)",
    "SPILL_MOV": "haiku recipe: temp variable / local copy of param, declaration order",
    "JUMP_LAYOUT": "variants ne0/eq0/ret_ternary; loop form rewrite (sonnet)",
    "CALLCONV": "conv_audit / cdecl_scan / ret_widths (script), else prototype fix in Ghidra",
    "FRAME": "variants locals_struct/array, declaration order (haiku)",
    "INLINE": "Opus (callee inlined / not inlined: /Ob flags, static, split)",
    "SIZE": "Opus (algorithm differs) or wrong flavour flags",
    "INDEX_SCALE": "global_widths / array element type (shl 4 vs *4: the global is typed as a 16 byte struct or the index scales wrong)",
    "IMM_STORE": "variants imm_store (store of a constant instead of a register copy); else sonnet",
    "RMW": "variants rmw_compound (x = x | c  vs  x |= c through a pointer): cheap",
    "RET_N": "stdcall_scan / SetStdcall.java (callee pops arguments)",
    "OTHER": "sonnet with check_one.py",
}


def strip(ops):
    ops = re.sub(r"\b(?:[DWB]?[A-Z]*WORD|BYTE) PTR |\bds:|\bes:", "", ops)      # a1/a3 (eax moffs) vs 8b/89 forms print differently
    ops = re.sub(r"\[(ADDR)\]", r"\1", ops)
    return REG.sub("R", ops)


def classify(ours, orig):
    A = [asmdiff.normalise(t) for _, _, t in asmdiff.disasm(ours)]
    B = [asmdiff.normalise(t) for _, _, t in asmdiff.disasm(orig)]
    if not B: return 0, "OTHER"
    la, lb = len(A), len(B)
    sm = difflib.SequenceMatcher(None, A, B, autojunk=False)
    ops = [o for o in sm.get_opcodes() if o[0] != "equal"]
    nd = sum(max(i2 - i1, j2 - j1) for _, i1, i2, j1, j2 in ops)
    if abs(len(ours) - len(orig)) > 0.3 * len(orig): return nd, "SIZE"
    if not ops: return 0, "OTHER"
    ca = sum(m == "call" for m, _ in A); cb = sum(m == "call" for m, _ in B)
    mnA, mnB = [m for m, _ in A], [m for m, _ in B]
    changed = [x for _, i1, i2, j1, j2 in ops for x in A[i1:i2] + B[j1:j2]]
    ca_ = " ; ".join(m + " " + o for m, o in changed)
    if any(m == "ret" and o.startswith("0x") for m, o in changed): return nd, "RET_N"
    if re.search(r"\*4\+ADDR", ca_) and re.search(r"shl \w+,0x4", ca_): return nd, "INDEX_SCALE"
    if re.search(r"(or|and|add|sub|xor) (DWORD PTR )?\[\w+\+0x\w+\],", ca_) and la > lb and abs(la - lb) <= 4: return nd, "RMW"
    if ca == cb and re.search(r"mov DWORD PTR ds:ADDR,0x", ca_) and la <= lb + 2: return nd, "IMM_STORE"
    if ca != cb: return nd, "INLINE"
    if la == lb and mnA == mnB:
        # same instruction shape: only operands differ
        sa, sb = [strip(o) for _, o in A], [strip(o) for _, o in B]
        if sa == sb: return nd, "REGALLOC"
        if all(sorted(x.split(",")) == sorted(y.split(",")) for x, y in zip(sa, sb)): return nd, "OPERAND_ORDER"
        diffs = [(m, x, y) for (m, _), x, y in zip(A, sa, sb) if x != y]
        if any(m in ("sub", "add") and "esp" in x + y for m, x, y in diffs): return nd, "CALLCONV" if any(m == "add" for m, _, _ in diffs) else "FRAME"
        if all(re.sub(r"-?(0x)?[0-9a-f]+", "N", x) == re.sub(r"-?(0x)?[0-9a-f]+", "N", y) for _, x, y in diffs): return nd, "CONST"
        return nd, "OTHER"
    if sorted(A) == sorted(B): return nd, "SCHED"
    if sorted((m, strip(o)) for m, o in A) == sorted((m, strip(o)) for m, o in B): return nd, "SCHED"
    if any(m == "add" and "esp" in o for m, o in changed) and abs(la - lb) <= 2: return nd, "CALLCONV"
    if any(m == "sub" and "esp" in o for m, o in changed) and nd <= 3: return nd, "FRAME"
    # a branch / compare differs: loop form, jump layout, cmp vs test
    if any(JCC.match(m) or m in ("jmp", "cmp", "test", "loop") for m, _ in changed) and abs(la - lb) <= 3: return nd, "JUMP_LAYOUT"
    # only mov / lea / push / pop inserted or deleted
    ins = [x for tag, i1, i2, j1, j2 in ops if tag in ("insert", "delete") for x in A[i1:i2] + B[j1:j2]]
    rep = [o for o in ops if o[0] == "replace"]
    if ins and not rep and all(m in ("mov", "lea", "push", "pop", "xor", "movzx", "movsx") for m, _ in ins): return nd, "SPILL_MOV"
    if abs(la - lb) <= 2 and nd <= 4 and all(m in ("mov", "lea", "push", "pop", "xor", "movzx", "movsx", "add", "sub", "inc", "dec", "cmp", "test") for m, _ in changed): return nd, "SPILL_MOV"
    return nd, "OTHER"


def one(args):
    f, items = args
    r = cmpfn.check(f, items, os.environ.get("AM_FLAGS", "/O2").split())
    rows = []
    for n, va in items:
        ok, det = r.get(n, (False, "missing"))
        if ok or n not in cmpfn.LAST:
            rows.append((n, va, 0, 0, "COMPILE_FAIL" if not ok else "MATCH", "", os.path.basename(f))); continue
        code, masked = cmpfn.LAST[n]
        orig = cmpfn.exe_bytes(va, cmpfn.FNSZ.get(va) or len(code) + 16)
        cm = bytearray(code)
        ms = sorted(masked)
        for g in range(0, len(ms) - 3, 4):      # relocated 4 byte fields -> placeholder address in the exe range (as cmpfn.check)
            for k, v in enumerate((0, 0, 0x50, 0)): cm[ms[g + k]] = v
        try: nd, cat = classify(bytes(cm), orig)
        except Exception as e: nd, cat = 0, "OTHER"
        rows.append((n, va, cmpfn.FNSZ.get(va, len(orig)), nd, cat, TOOL.get(cat, ""), os.path.basename(f)))
    return rows


def main():
    cdir, csvp = sys.argv[1], sys.argv[2]
    out = sys.argv[3] if len(sys.argv) > 3 else os.path.join(HERE, "..", "output", "nearmiss_triage.csv")
    jobs = int(sys.argv[4]) if len(sys.argv) > 4 else 2
    want = collections.defaultdict(list)
    for r in csv.reader(open(csvp)):
        if len(r) >= 4 and r[3] in ("STRUCTURE", "OPERANDS", "REGISTERS"): want[r[0]].append((r[1], int(r[2], 16)))
    fp = os.path.join(cdir, "..", "var", "found.txt")      # functions variants.py already made identical in this round are no near misses any more
    if os.path.exists(fp):
        won = {l.split()[1] for l in open(fp) if len(l.split()) >= 2}
        want = {c: [x for x in it if x[0] not in won] for c, it in want.items()}
    with ThreadPoolExecutor(jobs) as ex:
        rows = [x for rs in ex.map(one, [(os.path.join(cdir, c), it) for c, it in sorted(want.items())]) for x in rs]
    rows = list({r[0]: r for r in rows}.values())      # a function can be in two chunks
    rows.sort(key=lambda r: (r[4], r[3]))
    with open(out, "w") as o:
        o.write("name,va,size,diff_lines,category,tool,chunk,srcdir\n")
        for n, va, sz, nd, cat, tool, ch in rows: o.write("%s,%08x,%d,%d,%s,%s,%s,%s\n" % (n, va, sz, nd, cat, tool.replace(",", ";"), ch, os.path.abspath(cdir)))
    c = collections.Counter(r[4] for r in rows)
    print("%d near misses" % len(rows))
    for k, v in c.most_common(): print("  %-14s %4d  %s" % (k, v, TOOL.get(k, "")))
    print("diff size histogram (differing instructions):")
    h = collections.Counter(min(r[3], 21) for r in rows)
    for k in sorted(h): print("  %s%2s %s" % ("" if k < 21 else ">", k if k < 21 else 20, "#" * h[k]))


if __name__ == "__main__":
    main()
