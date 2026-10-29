#!/usr/bin/env python3
"""rw_queue.py: output/rw_rewrite_queue.csv = every RenderWare function of the app image (owner rw / rw? / rw~ in
output/delink_sdk/ownership_items.csv, plus game-owned rw*/rp*/rx* names), with size, state (orig / tier1 / hand / agent /
tier2 / bridge / cluster), call-graph height (0 = leaf; cycles cut), static reach from game code (call / fptr / no),
executed in courses (output/fn_courses.csv), ranked easiest-first (not yet C, reached first, small, shallow)."""
import csv, bisect, re, collections, sys
from pathlib import Path
O = Path(__file__).resolve().parent.parent / 'output'; R = O.parent / 'recomp'
items = [r for r in csv.DictReader(open(O/'delink_sdk/ownership_items.csv')) if r['kind'] == 'code']
items.sort(key=lambda r: int(r['lo'], 16))
los = [int(r['lo'], 16) for r in items]; his = [int(r['hi'], 16) for r in items]
def find(a):
    i = bisect.bisect_right(los, a) - 1
    return i if i >= 0 and a < his[i] else None
ledger = {int(r['va'], 16): r['tier'] for r in csv.DictReader(open(O/'tier_ledger.csv')) if re.fullmatch(r'[0-9a-f]{8}', r['va'])}
clus = {l.strip() for l in open(R/'clusters/enabled.txt') if l.strip() and l[0] != '#'}
t2 = {l.split()[0] for l in open(R/'automatch_near_names.txt') if l.strip() and l[0] != '#'}
courses = {r['name']: r['courses'] for r in csv.DictReader(open(O/'fn_courses.csv'))}
RWN = re.compile(r'^(_?[Rr][wpx][A-Z_a-z]|RWP|rwp_)')
# rpworld.lib:obj\\i386\\* = statically linked D3DX / CRT-like members (d3dxmath, cnode, cpreprocessor, ...): not RenderWare
isrw = lambda r: not r['member'].startswith('rpworld.lib:obj\\i386') and r['owner'].startswith('rw') or (r['owner'] == 'game' and RWN.match(r['name']))
callees = collections.defaultdict(set); callers = collections.defaultdict(set); fptr = set()
for ln in open(O/'delink/ghidra_refs.csv'):
    f, t, ty = ln.split(',')[:3]
    try: f = int(f, 16); t = int(t, 16)
    except ValueError: continue
    j = find(t)
    if j is None or t != los[j]: continue
    i = find(f)
    if ty in ('UNCONDITIONAL_CALL', 'UNCONDITIONAL_JUMP'):
        if i is None or i == j: continue
        callees[i].add(j); callers[j].add(i)
    elif ty == 'DATA':
        if i is None: fptr.add(j)            # address in a table (vtable, callback list)
        elif i != j: callees[i].add(j); callers[j].add(i)
# reach from game code
reach = {}; q = collections.deque()
for i, r in enumerate(items):
    if r['owner'] == 'game' and not RWN.match(r['name']): reach[i] = 'call'; q.append(i)
while q:
    i = q.popleft()
    for j in callees[i]:
        if j not in reach: reach[j] = 'call'; q.append(j)
# tables: functions only in a data table, then what they reach
for j in sorted(fptr):
    if j not in reach: reach[j] = 'fptr'; q.append(j)
while q:
    i = q.popleft()
    for j in callees[i]:
        if j not in reach: reach[j] = 'fptr'; q.append(j)
sys.setrecursionlimit(100000); h = {}; onstack = set()
def height(i):
    if i in h: return h[i]
    if i in onstack: return 0
    onstack.add(i); v = 0
    for j in callees[i]:
        if j != i: v = max(v, 1 + height(j))
    onstack.discard(i); h[i] = v; return v
rows = []
for i, r in enumerate(items):
    if not isrw(r): continue
    va = los[i]; n = r['name']; sz = his[i] - va
    st = ledger.get(va) or ('tier2' if n in t2 else 'cluster' if n in clus else 'orig')
    if st == 'tier' : st = 'tier1'
    rows.append(dict(name=n, va='%08x' % va, size=sz, state=st, owner=r['owner'], member=r['member'].split('/')[-1] if r['member'] else '',
        depth=height(i), callees=len(callees[i]), callers=len(callers[i]), reach=reach.get(i, 'no'), courses=courses.get(n, '')))
done = {'tier1', 'hand', 'agent', 'bridge'}
def key(x): return (x['state'] in done, x['state'] == 'orig' and 0 or 1, x['reach'] == 'no', x['reach'] == 'fptr', x['depth'], x['size'])
rows.sort(key=key)
for k, x in enumerate(rows, 1): x['rank'] = k
with open(O/'rw_rewrite_queue.csv', 'w', newline='') as f:
    w = csv.DictWriter(f, ['rank', 'name', 'va', 'size', 'state', 'depth', 'callees', 'callers', 'reach', 'courses', 'owner', 'member']); w.writeheader()
    for x in rows: w.writerow({k: x[k] for k in w.fieldnames})
c = collections.Counter(x['state'] for x in rows); print(len(rows), dict(c))
print('orig reached:', sum(1 for x in rows if x['state'] == 'orig' and x['reach'] != 'no'), 'bytes', sum(x['size'] for x in rows if x['state'] == 'orig'))
