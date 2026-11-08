#!/usr/bin/env python3
"""p2_frozen_fnsizes.py [--csv out.csv] [--dropapp <link>/mfl.obj.dropapp.csv]: window-2 P2 (plan_engine_switch_window2_2026-11-08_v1 section 2). Run with the venv (capstone):
  python3 tools/run.py venv delink/p2_frozen_fnsizes.py
For each row of tools/delink/fnsizes_unanalysed_known.txt: the fnsizes size vs the size a recursive-descent disassembly from the entry reaches
(capstone, flow kept inside the row + 64 B slack), and every E8/E9/0F8x byte-scan branch inside the fnsizes extent (the pattern mkobj --drop-app
counts for a row listed in fnsizes_unanalysed.txt). A scan hit is `real` when it is an instruction the disassembly reached.
Verdict: nochange = no scan target lies inside another body (every target is a fnsizes row start or outside the code) -> listing the row cannot
add an interior reference, safe to move to fnsizes_unanalysed.txt; change = some target is inside a fnsizes body / gap -> listing can keep a tail
(link changes; window-3 code group). real_interior = such a target from a reached instruction (the crash class of 2026-11-06). Reads only."""
import bisect, csv, os, struct, sys
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from fnsizes_unanalysed_check import code_sections, EXE
import capstone

def main():
    rows = sorted((int(l.split()[0], 16), l.split()[1], int(l.split()[2])) for l in open(os.path.join(G, "output", "fnsizes.txt")) if len(l.split()) >= 3)
    starts = [r[0] for r in rows]; size = {r[0]: r[2] for r in rows}
    known = [int(l.split()[0], 16) for l in open(os.path.join(G, "tools", "delink", "fnsizes_unanalysed_known.txt")) if l.strip() and not l.startswith("#")]
    secs = code_sections(EXE); lo = min(v for v, _ in secs); hi = max(v + len(b) for v, b in secs)
    def byte(va):
        for v, b in secs:
            if v <= va < v + len(b): return b, va - v
        return None, 0
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    def descend(a, sz):
        b, o = byte(a); seen, todo, end = set(), [a], a
        while todo:
            p = todo.pop()
            while p not in seen and a <= p < a + sz + 64:
                ins = next(md.disasm(bytes(b[p - a + o:p - a + o + 16]), p, 1), None)
                if ins is None: break
                seen.add(p); end = max(end, p + ins.size); m = ins.mnemonic
                if m.startswith("j") or m == "call":
                    try: t = int(ins.op_str, 16)
                    except ValueError: t = None
                    if m != "call" and t is not None: todo.append(t)
                    if m == "jmp": break
                if m in ("ret", "retf", "int3", "hlt") or m == "jmp": break
                p += ins.size
        return seen, end
    def inside(t):           # start of the fnsizes body that contains t strictly (not at its entry), or "gap" outside any body
        i = bisect.bisect_right(starts, t) - 1
        if i >= 0 and starts[i] == t: return None
        if i >= 0 and t < starts[i] + rows[i][2]: return "%08x" % starts[i]
        return "gap"
    names = {r[0]: r[1] for r in rows}; out = []
    dfile = sys.argv[sys.argv.index("--dropapp") + 1] if "--dropapp" in sys.argv else None      # a link's mfl.obj.dropapp.csv: a dropped row's sites are dead there
    drop = {int(r["va"], 16): r["status"] for r in csv.DictReader(open(dfile))} if dfile else {}
    for a in known:
        sz = size.get(a)
        if sz is None: out.append({"va": "%08x" % a, "name": "?", "fnsize": "", "dis_size": "", "scan": 0, "interior": "", "real_interior": "", "verdict": "stale", "dropapp": ""}); continue
        seen, end = descend(a, sz); b, o = byte(a); ints, real = [], []
        for i in range(sz):
            k = 1 if b[o + i] in (0xE8, 0xE9) else 2 if b[o + i] == 0x0F and 0x80 <= b[o + i + 1] <= 0x8F else 0
            if not k: continue
            t = (a + i + k + 4 + struct.unpack_from("<i", b, o + i + k)[0]) & 0xFFFFFFFF
            if not lo <= t < hi: continue
            w = inside(t)
            if w is None or w == "%08x" % a: continue          # entry of a function, or a branch inside the row itself
            ints.append("%08x->%08x(%s)" % (a + i, t, w))
            if a + i in seen: real.append(ints[-1])
        out.append({"va": "%08x" % a, "name": names[a], "fnsize": sz, "dis_size": end - a, "scan": len(ints), "interior": " ".join(ints),
                    "real_interior": " ".join(real), "verdict": "change" if ints else "nochange", "dropapp": drop.get(a, "")})
    f = sys.argv[sys.argv.index("--csv") + 1] if "--csv" in sys.argv else None
    if f:
        with open(f, "w", newline="") as h:
            w = csv.DictWriter(h, fieldnames=list(out[0])); w.writeheader(); w.writerows(out)
    for r in out:
        if r["verdict"] != "nochange" or r["fnsize"] != r["dis_size"]:
            print(r["va"], r["name"], r["fnsize"], r["dis_size"], r["verdict"], r["dropapp"] or "-", r["real_interior"] or r["interior"][:120])
    c = {v: sum(1 for r in out if r["verdict"] == v) for v in ("nochange", "change", "stale")}
    print("rows %d: nochange %d, change %d (real interior %d), stale %d; size differs %d" % (len(out), c["nochange"], c["change"],
          sum(1 for r in out if r["real_interior"]), c["stale"], sum(1 for r in out if r["fnsize"] != r["dis_size"])))

if __name__ == "__main__": main()
