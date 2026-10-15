#!/usr/bin/env python3
"""Instruction level comparison of two x86 code blobs (idea from Echo / reccmp: compare normalised assembly, not bytes).
Uses `objdump` (binutils) for disassembly; everything else is stdlib.
classify(ours, orig, masked) -> (kind, score, detail)   kind: IDENTICAL | REGISTERS (same code modulo a consistent register renaming)
| OPERANDS (same mnemonics, operands differ: constants, operand/memory order, SIB form) | STRUCTURE (instruction sequence differs).
score = 1 - normalised instruction edit distance (1.0 = identical)."""
import difflib, re, subprocess, tempfile, os

REGS32 = ["eax", "ecx", "edx", "ebx", "esi", "edi", "ebp"]
SUB = {"ax": "eax", "cx": "ecx", "dx": "edx", "bx": "ebx", "si": "esi", "di": "edi", "bp": "ebp",
       "al": "eax", "cl": "ecx", "dl": "edx", "bl": "ebx", "ah": "eax", "ch": "ecx", "dh": "edx", "bh": "ebx"}

def disasm(code, va=0x1000):
    with tempfile.NamedTemporaryFile(suffix=".bin", delete=False) as f:
        f.write(code)
        path = f.name
    try:
        out = subprocess.run(["objdump", "-D", "-b", "binary", "-m", "i386", "-M", "intel", "--adjust-vma=0x%x" % va, path],
                             capture_output=True, text=True).stdout
    finally:
        os.unlink(path)
    ins = []
    for l in out.split("\n"):
        m = re.match(r"\s*([0-9a-f]+):\t([0-9a-f ]+)\t(.*)$", l)
        if m:
            text = re.sub(r"\s+", " ", m.group(3).split("#")[0]).strip()
            ins.append((int(m.group(1), 16), len(m.group(2).split()), text))
    return ins

def normalise(text, va_range=(0x400000, 0x900000)):
    """mnemonic + operands with addresses/targets replaced (relocated fields differ between object and exe)"""
    parts = text.split(" ", 1)
    mn = parts[0]
    ops = parts[1] if len(parts) > 1 else ""
    if mn in ("call", "jmp") and re.fullmatch(r"[0-9a-fx]+", ops):
        ops = "TARGET"
    elif mn.startswith("j") and re.fullmatch(r"[0-9a-fx]+", ops):
        ops = "LABEL"
    ops = re.sub(r"0x0*500000\b", "ADDR", ops)
    ops = re.sub(r"0x([0-9a-f]{6,8})\b", lambda m: "ADDR" if va_range[0] <= int(m.group(1), 16) <= va_range[1] or int(m.group(1), 16) < 0x1000000 and int(m.group(1), 16) > 0x3fffff else m.group(0), ops)
    return mn, ops

def reg_pattern(ops):
    """operands with registers replaced by r0,r1,... in order of appearance (per instruction list, filled by caller)"""
    return ops

JCC_REVERSE = {"jl": "jg", "jg": "jl", "jle": "jge", "jge": "jle", "jb": "ja", "ja": "jb", "jbe": "jae", "jae": "jbe",
               "je": "je", "jne": "jne", "jz": "jz", "jnz": "jnz", "js": "js", "jns": "jns"}

def swap_operands(ops):
    p = ops.split(",")
    return ",".join(reversed(p)) if len(p) == 2 else ops

def apply_swaps(a, b):
    """reccmp's operand swap rules ('effective match'): test x,y == test y,x; cmp x,y + jcc == cmp y,x + reversed jcc.  a, b: lists of (mnemonic, operands)
    of the same length.  Returns (a with the swaps applied, number of swaps)."""
    a, n = list(a), 0
    for i in range(len(a)):
        if a[i] == b[i]:
            continue
        mn, ops = a[i]
        if mn != b[i][0] or mn not in ("cmp", "test") or swap_operands(ops) != b[i][1]:
            continue
        if mn == "test":
            a[i] = b[i]; n += 1
        elif i + 1 < len(a) and JCC_REVERSE.get(a[i + 1][0]) == b[i + 1][0] and a[i + 1][1] == b[i + 1][1]:
            a[i], a[i + 1] = b[i], b[i + 1]; n += 1
    return a, n

def classify(ours, orig, ours_va=0x1000, orig_va=0x1000):
    ia, ib = disasm(ours, ours_va), disasm(orig, orig_va)
    rets = sum(1 for _, _, t in ia if t.startswith("ret"))
    cut, seen = len(ib), 0
    for k, (_, _, t) in enumerate(ib):          # the original blob is longer than the function: stop after the same number of rets
        if t.startswith("ret"):
            seen += 1
            if seen >= rets:
                cut = k + 1
                break
    a = [normalise(t) for _, _, t in ia]
    b = [normalise(t) for _, _, t in ib[:cut]]
    swaps = 0
    if len(a) == len(b):
        a, swaps = apply_swaps(a, b)
    if a == b:
        return ("REGISTERS", 0.999, "%d operand swap(s) (cmp/test with reversed jump)" % swaps) if swaps else ("IDENTICAL", 1.0, "")
    sm = difflib.SequenceMatcher(None, a, b, autojunk=False)
    score = sm.ratio()
    if len(a) == len(b) and all(x[0] == y[0] for x, y in zip(a, b)):
        regre = re.compile(r"\b(eax|ecx|edx|ebx|esi|edi|ebp|ax|cx|dx|bx|si|di|bp|al|cl|dl|bl|ah|ch|dh|bh)\b")
        if all(regre.sub("R", x[1]) == regre.sub("R", y[1]) for x, y in zip(a, b)):
            # only register names differ; a single global renaming is not required (live ranges may be renamed independently)
            mapping = {}
            for x, y in zip(a, b):
                for p, q in zip(regre.findall(x[1]), regre.findall(y[1])):
                    if p != q:
                        mapping.setdefault((SUB.get(p, p), SUB.get(q, q)), 0)
                        mapping[(SUB.get(p, p), SUB.get(q, q))] += 1
            exact = sum(1 for x, y in zip(a, b) if x == y) / float(len(a))
            score = 0.9 + 0.099 * exact          # rank: identical > register-only > operand-only > structure
            return "REGISTERS", score, ("%d operand swap(s), " % swaps if swaps else "") + "register differences " + ", ".join("%s->%s x%d" % (p, q, n) for (p, q), n in sorted(mapping.items()))
        return "OPERANDS", 0.7 + 0.19 * score, first_diff(a, b)
    return "STRUCTURE", 0.7 * score, first_diff(a, b)

def first_diff(a, b):
    sm = difflib.SequenceMatcher(None, a, b, autojunk=False)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag != "equal":
            return "ours[%d:%d]=%s | orig[%d:%d]=%s" % (i1, i2, [" ".join(x).strip() for x in a[i1:i2]][:3], j1, j2, [" ".join(x).strip() for x in b[j1:j2]][:3])
    return ""

if __name__ == "__main__":
    import sys
    print(classify(bytes.fromhex(sys.argv[1]), bytes.fromhex(sys.argv[2])))
