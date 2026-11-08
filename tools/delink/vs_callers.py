#!/usr/bin/env python3
"""vs_callers.py [va ...]: window-2 P3. Every reference (output/delink/ghidra_refs.csv) into the d3d9vertexshader functions (or the given VAs), with
the fnsizes row that holds the referencing site, then the reverse closure (who reaches those rows) up to depth 4. Reads only."""
import bisect, collections, csv, os, sys
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
rows = sorted((int(l.split()[0], 16), l.split()[1], int(l.split()[2])) for l in open(os.path.join(G, "output", "fnsizes.txt")) if len(l.split()) >= 3)
st = [r[0] for r in rows]
def fn(a):
    i = bisect.bisect_right(st, a) - 1
    return rows[i] if i >= 0 and a < rows[i][0] + rows[i][2] else (a, "?%08x" % a, 0)
tg = [int(x, 16) for x in sys.argv[1:]] or [r[0] for r in rows if "VertexShader" in r[1]]
into = collections.defaultdict(list)            # target fn start -> [(site, type, holder name)]
for r in csv.reader(open(os.path.join(G, "output", "delink", "ghidra_refs.csv"))):
    try: s, t = int(r[0], 16), int(r[1], 16)
    except (ValueError, IndexError): continue
    f = fn(t)
    if f[2] and t == f[0]: into[t].append((s, r[2], fn(s)))
name = {r[0]: r[1] for r in rows}
for t in tg:
    print("%08x %s <- %s" % (t, name.get(t, "?"), "; ".join("%08x %s %s" % (s, ty[:6], h[1]) for s, ty, h in into.get(t, [])) or "none"))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))      # raw cross-check for the targets Ghidra has no ref to: E8/E9 rel32 and plain dwords anywhere in the exe
import struct
from fnsizes_unanalysed_check import code_sections, EXE
img = open(EXE, "rb").read(); secs = code_sections(EXE)
for t in (t for t in tg if not into.get(t)):
    w = struct.pack("<I", t); raw = [i for i in range(len(img) - 3) if img[i:i + 4] == w]
    rel = ["%08x" % (v + i) for v, b in secs for i in range(len(b) - 5) if b[i] in (0xE8, 0xE9) and (v + i + 5 + struct.unpack_from("<i", b, i + 1)[0]) & 0xFFFFFFFF == t]
    print("raw %08x %s: dword at file offsets %s, rel32 from %s" % (t, name.get(t, "?"), [hex(x) for x in raw] or "none", rel or "none"))
seen, front = set(tg), set(tg)
for d in range(1, 5):
    nxt = {h[0] for t in front for s, ty, h in into.get(t, []) if h[2] and h[0] not in seen}
    if not nxt: break
    print("depth %d: %s" % (d, ", ".join(name.get(a, "%08x" % a) for a in sorted(nxt))))
    seen |= nxt; front = nxt
