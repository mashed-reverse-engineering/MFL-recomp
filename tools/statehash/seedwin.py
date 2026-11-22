#!/usr/bin/env python3
"""seedwin.py <growth.py --every 1 output> K [from]: mode B (sh_seed) window table.  B was re-seeded from A after frames from, from+K, ...;
B's record of a seed frame is taken before the write, so window w = frames from+w*K+1 .. from+(w+1)*K and its last frame holds the drift of
the whole window.  Per window and class (cars / vehicles / aibots): max float words differing, max ulp, max relative error, max discrete
words, and the first frame with a discrete word.  A window with no line in the growth output is bit-exact in every class ("exact").
Input lines: `<frame> ph<p> rf<r> | cars nf maxulp maxrel nd | vehicles .. | aibots .. | other ..` (tools/statehash/growth.py)."""
import re, sys

K = int(sys.argv[2]); F0 = int(sys.argv[3]) if len(sys.argv) > 3 else 0
CL = ("cars", "vehicles", "aibots")
win, last = {}, 0
for l in open(sys.argv[1]):
    m = re.match(r"\s*(\d+) ph\d+ rf\d+\s+\|(.*)", l)
    if not m:
        if l.startswith("SUMMARY"): last = int(l.split()[2])
        continue
    fr = int(m.group(1))
    if fr <= F0: continue
    w = win.setdefault((fr - F0 - 1) // K, {})
    for part in m.group(2).split("|"):
        p = part.split()
        if p[0] not in CL: continue
        nf, mu, mr, nd = int(p[1]), int(p[2]), float(p[3]), int(p[4])
        e = w.setdefault(p[0], [0, 0, 0.0, 0, None])
        e[0] = max(e[0], nf); e[1] = max(e[1], mu); e[2] = max(e[2], mr); e[3] = max(e[3], nd)
        if nd and e[4] is None: e[4] = fr
print("# window frames | " + " | ".join("%s nf maxulp maxrel nd first-discrete" % c for c in CL))
nexact = 0
for i in range((max(last - 1 - F0, 0) + K - 1) // K if last else (max(win) + 1 if win else 0)):
    w = win.get(i, {})
    cells = []
    for c in CL:
        e = w.get(c)
        cells.append("exact" if not e or not (e[0] or e[3]) else "%d %d %.3g %d %s" % (e[0], e[1], e[2], e[3], e[4] if e[4] is not None else "-"))
    nexact += all(x == "exact" for x in cells)
    print("%3d %4d-%-4d | %s" % (i, F0 + i * K + 1, F0 + (i + 1) * K, " | ".join(cells)))
print("WINDOWS %d, bit-exact in cars+vehicles+aibots: %d" % (len(range((max(last - 1 - F0, 0) + K - 1) // K)) if last else len(win), nexact))
