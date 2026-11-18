#!/usr/bin/env python3
"""classsweep.py: report-only class sweep of recompilation defects, ours (linked exe, functions by map) vs the original (MFL.exe, by
output/fnsizes.txt).  W14 (M).  Three classes, one candidate row per function and class:
  div      signed vs unsigned integer arithmetic (W13 b8: `g_tick_number % 3`, original cdq + idiv, ours unsigned): the count of signed ops
           (idiv, cdq, sar, one-operand imul) falls while the count of unsigned ops (div, shr, one-operand mul) rises, or the other way round
  fpucopy  a dword copied through the x87 (fld m32 directly followed by fstp m32) more often in ours than in the original (W13 b9: the
           original copies with mov; fld quiets a signalling-NaN bit pattern)
  x87      x87 operation order (W12 b1 / b5 / b6, W13 b12, W14 b13 / b14): tools/delink/x87ord.py events, a row = an event on one side only
Usage (capstone: run.py venv):
  classsweep.py run <link dir> <out.csv> [cov.csv]   sweep; cov.csv (default agents/cov_w11/cov.csv) marks race-reached functions (status hit);
                                                    prints one summary line `classsweep: ...` (for gate.log).  Exit 0 (report-only).
  classsweep.py selftest                            fixtures built at run time per class (bad pair = row, equal pair = no row)
  classsweep.py known [file]                         tools/delink/classsweep_known.txt: every known defect gives more rows on its pre-fix link
                                                    than on its post-fix link (x87: events on one side only; div / fpucopy: the row itself);
                                                    prints `known: n of n`; exit 1 otherwise
ponytail: per-function counts / linear walks, no CFG; a row is a candidate for a compile proof, never a verdict (rule 53)."""
import collections, csv, os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import x87ord
from x87ord import md, x86

G = x87ord.G      # the tree whose game/MFL.exe, output/fnsizes.txt, cov and known list are read (x87ord's constant)
SIGNED, UNSIGNED = {'idiv', 'cdq', 'sar'}, {'div', 'shr'}


def div_counts(img, va, n):
    s = u = 0
    for i in md.disasm(img.rd(va, n), va):
        m = i.mnemonic
        if m in SIGNED or (m == 'imul' and len(i.operands) == 1): s += 1
        elif m in UNSIGNED or (m == 'mul' and len(i.operands) == 1): u += 1
    return s, u


def pure_copies(img, va, n):
    k, pend = 0, False
    for i in md.disasm(img.rd(va, n), va):
        if not i.mnemonic.startswith('f'): continue
        mem4 = len(i.operands) == 1 and i.operands[0].type == x86.X86_OP_MEM and i.operands[0].size == 4
        if i.mnemonic == 'fld' and mem4: pend = True
        elif i.mnemonic == 'fstp' and mem4 and pend: k += 1; pend = False
        else: pend = False
    return k


def classify(orig, ov, on, ours, mv, mn):
    """rows [(class, orig, ours, detail)] of one function"""
    rows = []
    (so, uo), (sm, um) = div_counts(orig, ov, on), div_counts(ours, mv, mn)
    if (sm < so and um > uo) or (sm > so and um < uo):
        rows.append(('div', 'signed %d unsigned %d' % (so, uo), 'signed %d unsigned %d' % (sm, um), ''))
    co, cm = pure_copies(orig, ov, on), pure_copies(ours, mv, mn)
    if cm > co:
        rows.append(('fpucopy', '%d' % co, '%d' % cm, 'fld m32 -> fstp m32 copies'))
    x, y = x87ord.diff(x87ord.events(orig, ov, on), x87ord.events(ours, mv, mn, True))
    if x or y:
        rows.append(('x87', '%d orig-only' % len(x), '%d ours-only' % len(y), (' | '.join(x[:2]) + ' || ' + ' | '.join(y[:2]))[:300]))
    return rows


def sweep(link):
    mp = os.path.join(link, 'ours.map')
    delta = next(int(l.split()[2], 16) - 0x752364 for l in open(mp) if ' _g_race_load_752364 ' in l)
    ours, orig = x87ord.Img(os.path.join(link, 'ours.exe'), delta), x87ord.Img(G + '/game/MFL.exe')
    ot, mt = x87ord.orig_table(), x87ord.ours_table(mp)
    out = []
    for nm, (va, n, obj) in sorted(mt.items()):
        if nm in ot:
            out += [(r[0], nm, obj) + r[1:] for r in classify(orig, *ot[nm], ours, va, n)]
    return out, sum(1 for nm in mt if nm in ot)


def run(link, outcsv, covf):
    import time; t0 = time.time()
    hit = {r['function'].lstrip('_') for r in csv.DictReader(open(covf)) if r['status'] == 'hit'} if os.path.exists(covf) else set()
    rows, nf = sweep(link)
    with open(outcsv, 'w', newline='') as f:
        w = csv.writer(f)
        w.writerow(('class', 'function', 'obj', 'race_reached', 'orig', 'ours', 'detail'))
        for c, nm, obj, o, m, d in rows: w.writerow((c, nm, obj, int(nm in hit), o, m, d))
    cnt = collections.Counter(r[0] for r in rows); rch = collections.Counter(r[0] for r in rows if r[1] in hit)
    print('classsweep: %d functions compared; candidates %s; csv %s (report-only)' % (nf, ', '.join('%s %d (%d race-reached)' % (c, cnt[c], rch[c]) for c in ('div', 'fpucopy', 'x87')), outcsv) + ', %d s' % (time.time() - t0))
    return 0


class Fake(x87ord.Img):
    def __init__(self, code): self.code, self.delta = code, 0
    def rd(self, va, n): return self.code if va == 0x1000 and n else b''
    def writable(self, va): return True


def selftest():
    H = bytes.fromhex
    cases = [   # class, original bytes, ours bytes, row expected
        ('div', H('99f7f9'), H('33d2f7f1'), True),                                   # cdq; idiv ecx  vs  xor edx,edx; div ecx
        ('div', H('99f7f9'), H('99f7f9'), False),
        ('fpucopy', H('a110000000a320000000'), H('d90510000000d91d20000000'), True),  # mov eax,[10]; mov [20],eax  vs  fld [10]; fstp [20]
        ('fpucopy', H('d90510000000d91d20000000'), H('d90510000000d91d20000000'), False),
        ('x87', H('d90510000000d80514000000d80518000000d91d20000000'), H('d90510000000d80518000000d80514000000d91d20000000'), True),   # (a+b)+c vs (a+c)+b
        ('x87', H('d90510000000d80514000000d80518000000d91d20000000'), H('d90514000000d80510000000d80518000000d91d20000000'), False),  # swap only
    ]
    bad = 0
    for c, o, m, want in cases:
        got = c in [r[0] for r in classify(Fake(o), 0x1000, len(o), Fake(m), 0x1000, len(m))]
        print('selftest %-8s %-6s %s' % (c, 'bad' if want else 'equal', 'ok' if got == want else 'WRONG (row %s)' % got)); bad += got != want
    print('selftest %s' % ('PASS' if not bad else 'FAIL'))
    return 1 if bad else 0


def known(path):
    rows = [l.split() for l in open(path) if l.strip() and l[0] != '#']
    cache, ok = {}, 0
    for c, fn, pre, post, tag in rows:
        res = []
        for lk in (pre, post):      # score: x87 = events on one side only (a function keeps its other x87ord rows after a fix), else 1 per row
            if lk not in cache: cache[lk] = {(r[0], r[1]): (int(r[3].split()[0]) + int(r[4].split()[0]) if r[0] == 'x87' else 1) for r in sweep(os.path.join(G, lk))[0]}
            res.append(cache[lk].get((c, fn), 0))
        good = res[0] > res[1]; ok += good      # the defect is rows that the fix removes
        print('known %-8s %-7s %-40s pre-fix %d, post-fix %d  %s' % (tag, c, fn, res[0], res[1], 'ok' if good else 'MISSED'))
    print('known: %d of %d' % (ok, len(rows)))
    return 0 if ok == len(rows) else 1


if __name__ == '__main__':
    a = sys.argv[1:]
    if a[:1] == ['selftest']: sys.exit(selftest())
    if a[:1] == ['known']: sys.exit(known(a[1] if len(a) > 1 else os.path.join(G, 'tools', 'delink', 'classsweep_known.txt')))
    if a[:1] == ['run'] and len(a) >= 3: sys.exit(run(a[1], a[2], a[3] if len(a) > 3 else os.path.join(G, 'agents', 'cov_w11', 'cov.csv')))
    print(__doc__); sys.exit(2)
