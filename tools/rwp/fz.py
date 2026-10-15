#!/usr/bin/env python3
"""rw_fuzzy.py <out.csv> <lib>...: relaxed library matching for the exe functions the exact byte match (libmatch_lib.py) did not find.  For every unnamed exe function whose named neighbours on both sides are the same
class (output/rw_libmatch.csv = RenderWare), compares it with all library functions of the same length (relocation fields wildcarded) and reports the best candidate with the number of differing bytes.
Small difference counts mean the same source compiled with other data addresses / flags; the csv: VA,size,best candidate,library:member,differing bytes,second best.  Usage: rw_fuzzy.py out.csv lib..."""
import csv, os, re, struct, sys, tempfile, collections
HERE = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
src = open(os.path.join(HERE, "libmatch_lib.py")).read().replace("\nmain()\n", "\n")
ns = {"__file__": os.path.join(HERE, "libmatch_lib.py"), "__name__": "libmatch_lib"}
exec(compile(src, "libmatch_lib", "exec"), ns)
text, base = ns["text_of_exe"]()
members, obj_functions = ns["members"], ns["obj_functions"]
out, libs = sys.argv[1], sys.argv[2:]
bysize = collections.defaultdict(list)
tmp = tempfile.mkdtemp(prefix="rwfuzzy_")
for lib in libs:
    for mem, body in members(lib):
        f = os.path.join(tmp, "m.obj"); open(f, "wb").write(body)
        try: fns = list(obj_functions(f))
        except Exception: continue
        for name, code, masked in fns:
            if len(code) >= 6 and not name.startswith(("$", "??")): bysize[len(code)].append((name, code, masked, "%s:%s" % (os.path.basename(lib), mem)))
fn = []
for l in open(os.path.join(HERE, "..", "output", "fnsizes.txt")):
    a, n, s = l.split(); fn.append((int(a, 16), n, int(s)))
fn.sort()
rw = {int(r[0], 16) for r in csv.reader(open(os.path.join(HERE, "..", "output", "rw_libmatch.csv"))) if len(r) >= 4}
def is_rw(a, n): return a in rw or n.startswith(("Rw", "rw", "_rw", "Rp", "rp", "Rt", "rt", "_rp", "_rt", "Rx", "rx", "_rx"))
rows=[]
for a,n,sz in fn:
    if not (0x569b30<=a<0x58c8a0) or sz<6: continue
    code=text[a-base:a-base+sz]; best=[]
    for name,lc,masked,where in bysize.get(sz,[]):
        d=sum(1 for k in range(sz) if k not in masked and lc[k]!=code[k]); best.append((d,name,where))
    best.sort()
    if best: rows.append((a,sz,best[0][1],best[0][2],best[0][0],best[1][0] if len(best)>1 else -1))
with open(out, "w") as f:
    for r in rows: f.write("%08x,%d,%s,%s,%d,%d\n" % r)
c = collections.Counter(min(r[4], 20) for r in rows)
print(len(rows), "functions compared; differing bytes of the best same length candidate:", sorted(c.items()))
print("sizes with diff<=4:", sum(r[1] for r in rows if r[4] <= 4), "bytes of", sum(r[1] for r in rows))
