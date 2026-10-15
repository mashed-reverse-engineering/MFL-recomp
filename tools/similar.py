#!/usr/bin/env python3
"""Similar matched functions as a hint (Lewis' / Mizuchi's idea): for a function that does not match yet, list the functions that DO match (manifest status MATCH) whose original
assembly is most similar, with the source file, so their C shape can be reused (permuter seeds, manual rewrite).  Uses the ORIGINAL bytes at the manifest addresses only.
Usage: similar.py <va hex | symbol> [n=5]        (cache: ~/.cache/mfl_similar.pkl)"""
import difflib, os, pickle, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import asmdiff, cmpfn
MAN = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "src", "manifest.txt")
CACHE = os.path.expanduser("~/.cache/mfl_similar.pkl")

def orig_ops(va):
    ins = asmdiff.disasm(cmpfn.exe_bytes(va, 1500), va)
    out = []
    for _, _, t in ins:
        mn, ops = asmdiff.normalise(t)
        out.append(mn + " " + re.sub(r"\b(eax|ecx|edx|ebx|esi|edi|ebp|ax|cx|dx|bx|si|di|bp|al|cl|dl|bl)\b", "R", ops))   # registers do not matter for the shape
        if mn.startswith("ret"):
            break
    return out

def main():
    ents = []
    for l in open(MAN):
        p = l.split()
        if len(p) >= 3 and not l.startswith("#") and p[0] != "(none)":
            ents.append((p[1], int(p[2], 16), p[0], "DIFFERENT" in p))
    db = pickle.load(open(CACHE, "rb")) if os.path.exists(CACHE) else {}
    for s, va, f, near in ents:
        if va not in db:
            db[va] = orig_ops(va)
    os.makedirs(os.path.dirname(CACHE), exist_ok=True); pickle.dump(db, open(CACHE, "wb"))
    key = sys.argv[1]
    n = int(sys.argv[2]) if len(sys.argv) > 2 else 5
    tv = next((va for s, va, f, nr in ents if s == key), None) or int(key, 16)
    target = db.get(tv) or orig_ops(tv)
    rows = []
    for s, va, f, near in ents:
        if va == tv or near: continue          # only functions that match
        rows.append((difflib.SequenceMatcher(None, target, db[va], autojunk=False).ratio(), s, f, va, len(db[va])))
    print("target %08x: %d instructions" % (tv, len(target)))
    for r, s, f, va, ln in sorted(rows, reverse=True)[:n]:
        print("  %.2f  %s (%s, %08x, %d instr)" % (r, s, f, va, ln))

main()
