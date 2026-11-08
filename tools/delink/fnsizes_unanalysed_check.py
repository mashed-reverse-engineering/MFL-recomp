#!/usr/bin/env python3
"""fnsizes_unanalysed_check.py [--exe MFL.exe]: lesson check of 2026-11-06 (_rwResHeapFree crash).
mkobj --drop-app ignores E8/E9/0F8x byte-scan branches inside a sized function unless Ghidra has the same flow ref, or the row is listed in
fnsizes_unanalysed.txt. A row of output/fnsizes.txt with NO Ghidra flow ref inside but a byte-scan branch into code is a row Ghidra never
analysed (e.g. from lib item extents): it must be listed. Exit 1 and print the rows that are missing; also prints listed rows not in fnsizes.
Rows in fnsizes_unanalysed_known.txt predate the rule and are only counted (--write-known rewrites that file from the current misses)."""
import bisect, csv, os, struct, sys
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
EXE = sys.argv[sys.argv.index("--exe") + 1] if "--exe" in sys.argv else os.path.join(G, "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")
FLOW = ("UNCONDITIONAL_CALL", "UNCONDITIONAL_JUMP", "CONDITIONAL_JUMP", "CONDITIONAL_CALL", "COMPUTED_JUMP", "COMPUTED_CALL")

def code_sections(path):
    d = open(path, "rb").read(); pe = struct.unpack_from("<I", d, 0x3c)[0]
    n, opt = struct.unpack_from("<HxxxxxxxxxxxxH", d, pe + 6)
    base = struct.unpack_from("<I", d, pe + 24 + 28)[0]; out = []
    for i in range(n):
        o = pe + 24 + opt + 40 * i
        vs, va, rs, rp = struct.unpack_from("<IIII", d, o + 8); fl = struct.unpack_from("<I", d, o + 36)[0]
        if fl & 0x20: out.append((base + va, d[rp:rp + min(vs, rs)]))
    return out

def main():
    rows = [(int(l.split()[0], 16), l.split()[1], int(l.split()[2])) for l in open(os.path.join(G, "output", "fnsizes.txt")) if len(l.split()) >= 3]
    rows.sort(); starts = [r[0] for r in rows]
    unan = {int(l.split()[0], 16) for l in open(os.path.join(G, "tools", "delink", "fnsizes_unanalysed.txt")) if l.strip() and not l.startswith("#")}
    def row_of(a):
        i = bisect.bisect_right(starts, a) - 1
        return rows[i] if i >= 0 and a < rows[i][0] + rows[i][2] else None
    has_flow = set()
    for r in csv.reader(open(os.path.join(G, "output", "delink", "ghidra_refs.csv"))):
        if len(r) >= 3 and r[2] in FLOW:
            try: x = row_of(int(r[0], 16))
            except ValueError: continue
            if x: has_flow.add(x[0])
    secs = code_sections(EXE); lo = min(v for v, _ in secs); hi = max(v + len(b) for v, b in secs)
    def scan_branch(va, size):           # same pattern as mkobj's byte scan, target inside the code sections
        for v, b in secs:
            if not v <= va < v + len(b): continue
            for i in range(va - v, min(va - v + size, len(b) - 6)):
                k = 1 if b[i] in (0xE8, 0xE9) else 2 if b[i] == 0x0F and 0x80 <= b[i + 1] <= 0x8F else 0
                if k and lo <= (v + i + k + 4 + struct.unpack_from("<i", b, i + k)[0]) & 0xFFFFFFFF < hi: return True
        return False
    if "--selftest" in sys.argv:          # the crash row of the lesson must be caught when the list is ignored
        assert 0x4ea960 not in has_flow and scan_branch(0x4ea960, next(r[2] for r in rows if r[0] == 0x4ea960)), "rule misses RwResourcesAllocateResEntry"
        print("selftest ok"); return 0
    missing = [r for r in rows if r[0] not in has_flow and r[0] not in unan and scan_branch(r[0], r[2])]
    kf = os.path.join(G, "tools", "delink", "fnsizes_unanalysed_known.txt")      # rows that predate the rule (2026-11-07); listing them in fnsizes_unanalysed.txt changes the link, so that is a gated decision
    if "--write-known" in sys.argv:
        open(kf, "w").write("# fnsizes rows without Ghidra flow refs that predate fnsizes_unanalysed_check.py (2026-11-07); not new, not yet decided\n" + "".join("%08x %s\n" % (va, nm) for va, nm, _ in missing))
    known = {int(l.split()[0], 16) for l in open(kf) if l.strip() and not l.startswith("#")} if os.path.exists(kf) else set()
    nk = sum(1 for r in missing if r[0] in known); missing = [r for r in missing if r[0] not in known]
    stale = sorted(unan - set(starts))
    for va, nm, sz in missing: print("MISSING %08x %s %d (no Ghidra flow ref inside, byte-scan branch present)" % (va, nm, sz))
    for va in stale: print("STALE %08x (listed, no fnsizes row)" % va)
    print("fnsizes_unanalysed_check: %d rows, %d listed, %d known (pre-rule), %d missing, %d stale" % (len(rows), len(unan), nk, len(missing), len(stale)))
    return 1 if missing else 0

if __name__ == "__main__": sys.exit(main())
