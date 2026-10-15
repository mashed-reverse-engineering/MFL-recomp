#!/usr/bin/env python3
"""Call graph waves + usercall clusters of the game code. Writes output/callgraph_waves.csv,
output/callgraph_clusters.csv, prints a summary (--roadmap appends it to ROADMAP.md,
--census adds a 'wave' column to tier2_census.csv)."""
import csv, bisect, sys, collections, statistics
from pathlib import Path
O = Path(__file__).resolve().parent.parent / 'output'
C_TIERS = {'tier1', 'hand', 'agent', 'tier2', 'bridge', 'tier'}  # replaced by C (bridge = installed C with adapter)

items = [r for r in csv.DictReader(open(O/'delink_sdk/ownership_items.csv')) if r['kind'] == 'code']
items.sort(key=lambda r: int(r['lo'], 16))
los = [int(r['lo'], 16) for r in items]
his = [int(r['hi'], 16) for r in items]
def find(a):
    i = bisect.bisect_right(los, a) - 1
    return i if i >= 0 and a < his[i] else None
ledger = {int(r['va'], 16): r['tier'] for r in csv.DictReader(open(O/'tier_ledger.csv'))}
usercall = {int(r['addr'], 16) for r in csv.DictReader(open(O/'regsigs.csv'))}
cen = {int(r['va'], 16): r['blocker'] for r in csv.DictReader(open(O/'tier2_census.csv'))}

game = {}  # index -> state
for i, r in enumerate(items):
    if r['owner'] == 'game':
        game[i] = 'C:' + ledger[los[i]] if ledger.get(los[i]) in C_TIERS else 'orig'
callees = collections.defaultdict(set); callers = collections.defaultdict(set); fptr = set()
for ln in open(O/'delink/ghidra_refs.csv'):
    f, t, ty = ln.split(',')[:3]
    try: f = int(f, 16); t = int(t, 16)
    except ValueError: continue
    j = find(t)
    if j is None or t != los[j]: continue
    if ty in ('UNCONDITIONAL_CALL', 'UNCONDITIONAL_JUMP'):
        i = find(f)
        if i is None or i == j: continue
        if ty == 'UNCONDITIONAL_JUMP' and los[i] <= t < his[i]: continue
        callees[i].add(j); callers[j].add(i)
    elif ty in ('DATA', 'READ'):
        i = find(f)
        if i is None or i != j: fptr.add(j)
# waves over remaining (orig) game functions; non-game callees = library, C game = done
rem = [i for i, s in game.items() if s == 'orig']
wave = {}
def w(i, stack=()):
    if i in wave: return wave[i]
    if i in stack: return 0  # ponytail: recursion ignored (roadmap: no SCCs > 1)
    m = -1
    for c in callees[i]:
        if game.get(c) == 'orig': m = max(m, w(c, stack + (i,)))
    wave[i] = m + 1; return wave[i]
sys.setrecursionlimit(10000)
for i in rem: w(i)
rows = []
for i in sorted(rem, key=lambda i: (wave[i], los[i])):
    n = items[i]['name']
    rows.append(dict(name=n, va='%08x' % los[i], bytes=his[i]-los[i], state='orig', wave=wave[i],
        n_callers=len(callers[i]), n_callees=len(callees[i]), is_usercall=int(los[i] in usercall),
        fptr=int(i in fptr), blocker=cen.get(los[i], '')))
with open(O/'callgraph_waves.csv', 'w', newline='') as fh:
    wr = csv.DictWriter(fh, rows[0].keys()); wr.writeheader(); wr.writerows(rows)
# clusters: usercall fn + callers still original
cl = []
for i in rem:
    if los[i] not in usercall: continue
    mem = {i} | {c for c in callers[i] if game.get(c) == 'orig'}
    bc = sorted({items[c]['name'] for m in mem for c in callers[m] if c not in mem})
    be = sorted({items[c]['name'] for m in mem for c in callees[m] if c not in mem})
    cl.append(dict(root=items[i]['name'], size=len(mem), bytes=sum(his[m]-los[m] for m in mem),
        members=' '.join(items[m]['name'] for m in sorted(mem)), boundary_callers=' '.join(bc), boundary_callees=' '.join(be)))
cl.sort(key=lambda r: -r['bytes'])
with open(O/'callgraph_clusters.csv', 'w', newline='') as fh:
    wr = csv.DictWriter(fh, cl[0].keys()); wr.writeheader(); wr.writerows(cl)
# summary
S = ['### Call graph waves (`tools/callgraph.py`, %d remaining original game functions, %d bytes)' % (len(rows), sum(r['bytes'] for r in rows)),
     '', 'wave: functions / bytes']
bw = collections.defaultdict(lambda: [0, 0])
for r in rows: bw[r['wave']][0] += 1; bw[r['wave']][1] += r['bytes']
ng=[r for r in rows if not r['name'].startswith('gap_')]
S.append('Without the 610 unnamed gap_ items: %d functions / %d bytes, ready %d / %d bytes (roadmap numbers).' % (len(ng), sum(r['bytes'] for r in ng), sum(r['wave']==0 for r in ng), sum(r['bytes'] for r in ng if r['wave']==0)))
S.append('; '.join('w%d %d/%d' % (k, *bw[k]) for k in sorted(bw)))
ready = [r for r in rows if r['wave'] == 0]
S.append('')
S.append('Ready (wave 0): %d functions, %d bytes; of these usercall %d, fn-pointer %d. Top 20 by bytes: %s' % (
    len(ready), sum(r['bytes'] for r in ready), sum(r['is_usercall'] for r in ready), sum(r['fptr'] for r in ready),
    ', '.join('%s %d%s' % (r['name'], r['bytes'], '' if not r['blocker'] else '[' + r['blocker'] + ']') for r in sorted(ready, key=lambda r: -r['bytes'])[:20])))
S.append('')
sz = collections.Counter(min(r['size'], 10) for r in cl)
S.append('Clusters (usercall fn + original callers): %d, %d bytes total; size distribution (10 = 10+): %s; biggest: %s' % (
    len(cl), sum(r['bytes'] for r in cl), ', '.join('%d:%d' % kv for kv in sorted(sz.items())),
    ', '.join('%s(%d fn, %d B)' % (r['root'], r['size'], r['bytes']) for r in cl[:5])))
print('\n'.join(S))
if '--roadmap' in sys.argv:
    open(O.parent/'ROADMAP.md', 'a').write('\n' + '\n'.join(S) + '\n')
if '--census' in sys.argv:
    p = O/'tier2_census.csv'; rd = list(csv.DictReader(open(p))); fn = [f for f in rd[0] if f != 'wave'] + ['wave']
    wv = {los[i]: wave[i] for i in rem}
    for r in rd: r['wave'] = wv.get(int(r['va'], 16), '')
    with open(p, 'w', newline='') as fh:
        wr = csv.DictWriter(fh, fn); wr.writeheader(); wr.writerows(rd)
