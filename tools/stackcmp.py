#!/usr/bin/env python3
"""Stack layout comparison of our function and the original (idea of reccmp's stackcmp.py): prologue (pushes, `sub esp`), frame size, and the stack slots the code touches
([esp+X] / [ebp-X], X made relative to the frame so pushes do not matter).  Usage: stackcmp.py <recomp .c file> <symbol> <va hex> [cl flags...]
Prints one summary line per side and the slots that exist in only one of them.  Meant for NEAR functions (a different local variable layout is the usual reason for a mismatch)."""
import os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import cmpfn, asmdiff

def layout(code):
    pushes, sub, depth, slots = 0, 0, 0, set()
    ins = asmdiff.disasm(code)
    for _, _, t in ins:
        if t.startswith("push"):
            depth += 4
            if not sub: pushes += 1
        elif t.startswith("pop"):
            depth -= 4
        m = re.match(r"sub esp,(0x[0-9a-f]+)", t)
        if m and not sub:
            sub = int(m.group(1), 16); depth += sub
        for x in re.findall(r"\[esp([+-]0x[0-9a-f]+)?\]", t):
            off = int(x, 16) if x else 0
            slots.add(off + depth)            # offset from the entry esp: comparable between the two versions
        if t.startswith("ret"):
            break
    return pushes, sub, sorted(slots)

def main():
    src, sym, va = sys.argv[1], sys.argv[2], int(sys.argv[3], 16)
    flags = sys.argv[4:] or ["/O2"]
    ok, det = cmpfn.check(src, [(sym, va)], flags)[sym]
    if ok: print(sym, "byte identical"); return
    m = re.search(r"ours ([0-9a-f]+) orig ([0-9a-f]+)", det)
    if not m: print(sym, det[:200]); return
    a, b = layout(bytes.fromhex(m.group(1))), layout(bytes.fromhex(m.group(2)))
    print("%s ours: %d pushes, sub esp %#x, %d stack slots | orig: %d pushes, sub esp %#x, %d stack slots" % (sym, a[0], a[1], len(a[2]), b[0], b[1], len(b[2])))
    only_a, only_b = sorted(set(a[2]) - set(b[2])), sorted(set(b[2]) - set(a[2]))
    if only_a or only_b:
        print("  slots only in ours: %s   only in original: %s   (offsets from the entry esp)" % (only_a, only_b))
    else:
        print("  same stack slots (the difference is not the stack layout)")

main()
