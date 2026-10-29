#!/usr/bin/env python3
"""bytediff_causes.py <out.csv> [--flags "/O2"] [--work DIR] [--pkl scan.pkl]: compile every tier-2 chunk (src/automatch_near/an_*.c) with the ledger flags,
compare each function with the original bytes (cmpfn.check: relocation fields masked, RELCHECK target rules) and classify the first-difference cause.
out.csv: name, size, bytes_diff_count, first_diff_offset, category.  A summary table (count and count with <= 8 differing bytes per category) is printed.
Categories: MATCH, register_allocation, instruction_order, instruction_order_regs, x87_form_or_spill, switch_jumptable, compare_or_branch_polarity,
stack_frame_local_layout, operand_form_constant, control_flow_shape, prologue_epilogue_saves, strength_reduction_or_lea, missing_extra_mov, small_structure_other,
large_structure, reloc_target, compile_error.
The scan result (name -> ok, detail, code, masked) is pickled to --pkl so that tools/bytematch_rewrite.py and re-classification do not recompile."""
import collections, csv, difflib, os, pickle, re, shutil, sys
from concurrent.futures import ThreadPoolExecutor
HERE = os.path.dirname(os.path.abspath(__file__)); G = os.path.join(HERE, ".."); R = os.path.join(G, "src")
sys.path.insert(0, HERE)
import cmpfn, asmdiff

REG = re.compile(r"\b(eax|ecx|edx|ebx|esi|edi|ebp|ax|cx|dx|bx|si|di|bp|al|cl|dl|bl|ah|ch|dh|bh)\b")
JCC = {"je", "jne", "jl", "jge", "jg", "jle", "jb", "jae", "ja", "jbe", "js", "jns", "jp", "jnp"}


def names():
    """name -> (va, chunk file)"""
    out = {}
    for l in open(os.path.join(R, "automatch_near_names.txt")):
        p = l.split()
        if len(p) >= 3: out[p[1]] = (int(p[0], 16), "an_" + p[2])
    return out


def scan(src_dir, flags, jobs=2, only=None):
    """compile all chunks in src_dir (a copy of src/automatch_near + the headers); -> {name: (ok, detail, (code, masked) or None)}"""
    groups = collections.defaultdict(list)
    for n, (va, ch) in names().items():
        if only is None or n in only: groups[ch].append((n, va))

    def run(k):
        try: r = cmpfn.check(os.path.join(src_dir, k), groups[k], flags)
        except Exception as e: return {s: (False, "compile error EXC %s" % e, None) for s, _ in groups[k]}
        return {s: (r[s][0], r[s][1], cmpfn.LAST.get(s)) for s, _ in groups[k]}
    res = {}
    with ThreadPoolExecutor(jobs) as ex:
        for o in ex.map(run, sorted(groups)): res.update(o)
    return res


def fill(code, masked, ob):
    cm = bytearray(code)
    for o in masked:
        if o < len(cm) and o < len(ob): cm[o] = ob[o]      # relocated fields: show the original's bytes so only real differences remain
    return bytes(cm)


def norm(ins): return [re.sub(r"0x[0-9a-f]+", "N", t) if t.startswith(("call", "jmp", "j")) else t for _, _, t in ins]


def classify(code, masked, va):
    size = len(code)
    gs = cmpfn.FNSZ.get(va)
    ob = cmpfn.exe_bytes(va, gs or size)
    nd = sum(1 for i, (a, b) in enumerate(zip(code, ob)) if a != b and i not in masked) + abs(len(code) - len(ob))
    fd = next((i for i, (a, b) in enumerate(zip(code, ob)) if a != b and i not in masked), min(len(code), len(ob)))
    ours = asmdiff.disasm(fill(code, masked, ob)); org = asmdiff.disasm(ob)
    a, b = norm(ours), norm(org)
    ar, br = [REG.sub("R", x) for x in a], [REG.sub("R", x) for x in b]
    sm = difflib.SequenceMatcher(None, a, b, autojunk=False)
    ops = [o for o in sm.get_opcodes() if o[0] != "equal"]
    da = [x for o in ops for x in a[o[1]:o[2]]]; db = [x for o in ops for x in b[o[3]:o[4]]]
    allt = " ".join(da + db)
    mn = lambda L: [x.split()[0] for x in L]
    if len(a) == len(b) and ar == br: cat = "register_allocation"
    elif len(a) == len(b) and sorted(a) == sorted(b): cat = "instruction_order"
    elif sorted(ar) == sorted(br): cat = "instruction_order_regs"
    elif any("*4" in x for x in a + b) and "jmp" in allt and "*4" in allt: cat = "switch_jumptable"
    elif re.search(r"\b(fld|fst|fstp|fild|fistp|fmul|fadd|fsub|fdiv|fcom|fcomp|fxch|fnstsw|fchs)\b", allt) and len(a) == len(b) and mn(a) != mn(b): cat = "x87_form_or_spill"
    elif any(x.split()[0] in ("fst", "fstp", "fld") and "esp" in x for x in da + db) and len(da) != len(db): cat = "x87_form_or_spill"
    elif len(a) == len(b) and mn(a) == mn(b):
        dif = [(x, y) for x, y in zip(a, b) if x != y]
        if any(x.split()[0] in ("cmp", "test") or x.split()[0] in JCC for x, _ in dif): cat = "compare_or_branch_polarity"
        elif any("esp" in x or "ebp" in x for x, _ in dif): cat = "stack_frame_local_layout"
        else: cat = "operand_form_constant"
    elif any(x.split()[0] in JCC for x in da + db) and len(a) != len(b): cat = "control_flow_shape"
    elif any(x.startswith(("sub esp", "add esp", "push ebp", "mov ebp", "pop ebp", "push esi", "push edi", "push ebx", "pop esi", "pop edi", "pop ebx", "pop ecx")) for x in da + db) and abs(len(a) - len(b)) <= 3: cat = "prologue_epilogue_saves"
    elif any(x.startswith(("lea", "imul", "shl", "sar", "shr", "add", "sub", "xor")) for x in da + db) and abs(len(a) - len(b)) <= 2: cat = "strength_reduction_or_lea"
    elif any(x.startswith("mov") for x in da + db) and abs(len(a) - len(b)) <= 2: cat = "missing_extra_mov"
    elif abs(len(a) - len(b)) <= 4: cat = "small_structure_other"
    else: cat = "large_structure"
    return size, nd, fd, cat


def main():
    args = sys.argv[1:]
    def opt(k, d):
        if k in args: i = args.index(k); v = args[i + 1]; del args[i:i + 2]; return v
        return d
    flags = opt("--flags", "/O2").split(); work = opt("--work", None); pkl = opt("--pkl", None)
    out = args[0]
    work = work or os.path.join(os.environ.get("TMPDIR", "/tmp"), "bytediff_src")
    os.makedirs(work, exist_ok=True)
    for h in os.listdir(R):
        if h.endswith(".h"): shutil.copy(os.path.join(R, h), work)
    nd_ = os.path.join(R, "automatch_near")
    for f in os.listdir(nd_):
        if f.endswith((".c", ".h")) and not os.path.islink(os.path.join(nd_, f)): shutil.copy(os.path.join(nd_, f), work)
    res = scan(work, flags)
    if pkl: pickle.dump(res, open(pkl, "wb"))
    nm = names(); rows = []
    for n, (ok, det, last) in sorted(res.items()):
        va = nm[n][0]
        if ok: rows.append((n, len(last[0]) if last else 0, 0, "", "MATCH"))
        elif last is None: rows.append((n, 0, "", "", "compile_error"))
        elif det.startswith("RELOC"): rows.append((n, len(last[0]), "", "", "reloc_target"))
        else: rows.append((n,) + classify(last[0], last[1], va))
    w = csv.writer(open(out, "w")); w.writerow("name size bytes_diff_count first_diff_offset category".split()); w.writerows(rows)
    c = collections.Counter(r[4] for r in rows)
    print("%-30s %5s %s" % ("category", "count", "with<=8B diff"))
    for k, v in c.most_common():
        print("%-30s %5d %d" % (k, v, sum(1 for r in rows if r[4] == k and r[2] not in ("", 0) and r[2] <= 8)))
    print("total", len(rows))


if __name__ == "__main__":
    main()
