#!/usr/bin/env python3
"""rwcheck.py <rwtap.log> [<other rwtap.log>] [--states out.csv] [--max N]
one log : per-function call counts, calls per draw, number of D3D draws the RW stream accounts for.
two logs: compares the RW-level streams call by call (name, args, dumped data); the first --max differences are listed with the argument that differs.
          Two recordings of the ORIGINAL must be identical here (the self-consistency acceptance); an RW reimplementation replayed from the same stream must
          reproduce the same stream of device-level calls.
--states f.csv : RW render-state table (RwRenderStateSet state,value) as in effect at every Im2D / Im3D draw call (draw index d=, call, state=value ...),
          the reference for the ZWRITE/ZTEST coupling etc. of the RwRenderStateSet reimplementation."""
import sys, collections
PTR = {'RwD3D9DynamicVertexBufferUnlock', 'RwD3D9SetStreamSource'}   # args are heap handles of the dynamic vertex buffer ring (differ between runs, like `#dyn` in normalise.py): args not compared
def parse(path):
    hooks, calls = {}, []
    for ln in open(path, errors='replace'):
        w = ln.split()
        if not w: continue
        if w[0] == 'H': hooks[w[2]] = int(w[3], 16)
        elif w[0] == 'C':
            kv = {t.split('=', 1)[0]: t.split('=', 1)[1] for t in w[3:] if '=' in t}
            calls.append((int(w[1]), w[2], int(kv['d']), int(kv['f']), (kv['a'].split(',') if kv.get('a') else []), {k: v for k, v in kv.items() if k[0] == 'p'}))
    return hooks, calls
def main():
    av = sys.argv[1:]; a = [x for i, x in enumerate(av) if not x.startswith('--') and not (i and av[i - 1] in ('--states', '--max'))]; mx = int(sys.argv[sys.argv.index('--max') + 1]) if '--max' in sys.argv else 10
    h1, c1 = parse(a[0])
    if '--states' in sys.argv:
        out = sys.argv[sys.argv.index('--states') + 1]; st = {}; f = open(out, 'w')
        for s, n, d, fr, ar, dd in c1:
            if n == 'RwRenderStateSet': st[int(ar[0], 16)] = ar[1]
            elif n.startswith('RwIm2DRender') or n.startswith('RwIm3DRender') or n == 'rwD3D9RasterRenderQuad' or n == 'rwD3D9RasterRenderGeneric':
                f.write(f'{d},{fr},{n},' + ' '.join(f'{k}={v}' for k, v in sorted(st.items())) + '\n')
        f.close()
    if len(a) == 1:
        cnt = collections.Counter(n for _, n, *_ in c1)
        print(f'{len(h1)} hooks, {len(c1)} calls, frames {c1[-1][3] + 1 if c1 else 0}, last draw index {c1[-1][2] if c1 else 0}')
        for n, k in cnt.most_common(): print(f'{k:8d} {n}')
        return 0
    h2, c2 = parse(a[1]); nd = 0
    for i, (x, y) in enumerate(zip(c1, c2)):
        if x[1] != y[1] or (x[4] != y[4] and x[1] not in PTR) or x[5] != y[5]:
            nd += 1
            if nd <= mx:
                why = 'name' if x[1] != y[1] else 'args ' + ','.join(str(j) for j, (p, q) in enumerate(zip(x[4], y[4])) if p != q) if x[4] != y[4] else 'dump ' + ','.join(k for k in x[5] if x[5][k] != y[5].get(k))
                print(f'#{i} {x[1]} vs {y[1]} d={x[2]}/{y[2]}: {why}')
    print(f'{len(c1)} vs {len(c2)} calls, {nd} differ' + ('' if len(c1) == len(c2) else ' (LENGTH DIFFERS)'))
    return 0 if nd == 0 and len(c1) == len(c2) else 1
sys.exit(main())
