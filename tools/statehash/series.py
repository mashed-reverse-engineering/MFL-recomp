#!/usr/bin/env python3
"""series.py A.bin B.bin frames[:step] va[:n] ...: per frame, the words at original VAs in A (ctrl) and B (ours; same page offsets); x or x/y when different."""
import os, struct, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import statecmp as S

fa, fb = sys.argv[1], sys.argv[2]
nfr, _, step = sys.argv[3].partition(':')
nfr, step = int(nfr), int(step or 1)
want = []
for x in sys.argv[4:]:
    va, _, n = x.partition(':')
    want.append((int(va, 16), int(n or 1)))
ga, gb = S.records(fa), S.records(fb)
mem = [None, None]
base = None
for fr in range(nfr):
    try:
        (ha, _, pa), (hb, _, pb) = next(ga), next(gb)
    except StopIteration:
        break
    if mem[0] is None:
        mem = [bytearray(S.PAGE * len(pa)), bytearray(S.PAGE * len(pb))]
        base = ha['base']
    for m, pg in ((mem[0], pa), (mem[1], pb)):
        for i, d in pg.items():
            m[i * S.PAGE:(i + 1) * S.PAGE] = d
    if fr % step:
        continue
    out = []
    for va, n in want:
        o = va - base
        a = struct.unpack_from('<%dI' % n, mem[0], o)
        b = struct.unpack_from('<%dI' % n, mem[1], o)
        out.append(' '.join(('%08x' % x) if x == y else ('%08x/%08x' % (x, y)) for x, y in zip(a, b)))
    print('%3d ph%d rf%-4d %s' % (fr, ha['phase'], ha['racef'], ' | '.join(out)))
