"""A8: classify layer `other` (code regions by call-site scan, library data runs by majority referrer layer). Usage: other_layer_classify.py <link work dir with gen/globals_layout.csv> > output/other_layer_<date>.csv
layers.py reads that csv. Layers: code regions below were classified by hand from callers/callees/strings (see evidence column)."""
import os,csv,sys,collections,bisect
sys.path.insert(0,os.path.dirname(os.path.abspath(__file__)))
import layers; layers.OTHER=[]
from layers import *
W=sys.argv[1]
def lay(a):
    it=item_at(a); return layer(a,it[3],it[4]) if it else None
runs=[]
for r in csv.DictReader(open(W+'/gen/globals_layout.csv')):
    if r['kind']!='lib': continue
    va,sz=H(r['va']),int(r['size']); v=va
    while v<va+sz:
        it=item_at(v); e=min(it[1],va+sz) if it else va+sz
        if not(0x63a610<=v<0x649138) and not(it and it[3] in('crt','rw','crt?','rw?')): runs.append([v,e,r['section'],None,collections.Counter()])
        v=e
runs.sort(); rs=[r[0] for r in runs]
def run_at(a):
    i=bisect.bisect_right(rs,a)-1
    return runs[i] if i>=0 and a<runs[i][1] else None
rel=[(H(r['target']),H(r['site'])) for r in csv.DictReader(open('output/delink_relocs.csv'))]
rel.sort(); tk=[t for t,s in rel]
for r in runs:
    i=bisect.bisect_left(tk,r[0])
    while i<len(rel) and rel[i][0]<r[1]:
        r[4].append(rel[i][1]) if False else None; i+=1
refs={id(r):[] for r in runs}
for r in runs:
    i=bisect.bisect_left(tk,r[0])
    while i<len(rel) and rel[i][0]<r[1]: refs[id(r)].append(rel[i][1]); i+=1
KEEP=('app','CRT','RenderWare','rwp','Lua','qhull','zlib/PNG/TIFF','D3DX','DirectShow')
for it in range(6):
    for r in runs:
        c=collections.Counter()
        for s in refs[id(r)]:
            if r[0]<=s<r[1]: continue
            if 0x401000<=s<0x5dc451: L=lay(s) or 'app'
            else:
                o=run_at(s); L=o[3] if o else None
                if L is None and not o: L=lay(s)
            if L in KEEP: c[L]+=1
        r[4]=c; r[3]=c.most_common(1)[0][0] if c else None

last=None
for r in runs:
    if r[3] is None: r[3]=last
    last=r[3]
CODE=[("4ab230","4ac4f0","D3DX","DirectInput c_rgodf* DIOBJECTDATAFORMAT tables (dinput8.lib data in .text): DirectX SDK lib, folded into the D3DX layer; no callers, 2 CRT callees only","n"),
 ("4b0b70","4b2a60","DirectShow","strmbase/movie filter islands: 6-21 external callers all DirectShow items, callees CRT/DirectShow, strings AMUnblock LoadRegTypeLib LoadTypeLib","n"),
 ("4c3b70","4c7150","app","stream_read/write_file + RwStream wrappers (104-190 app callers, RenderWare callees), PIZ archive fs piz_* (rb fopen, 85 app callers), warzone cb helpers: game code, needs C","y"),
 ("4c9ae0","4c9c30","Lua","3 fns between Lua items: 5 Lua callers, 2 Lua callees","n"),
 ("56e7dc","56e890","app","gaps inside the RW tail: callers are app only (6-7), no callees","y"),
 ("5d2ada","5d2ba0","D3DX","tail of GetMaxNumAnimationSets (d3dx9 anim.obj) plus a function with D3DX callers/callees","n"),
 ("5d2ba0","5d2d00","CRT","___mbtowc_mt/_mbtowc (CRT, symbol names) with D3DX/CRT callers","n"),
 ("5d58e7","5d5950","CRT","105 bytes with CRT callees x3 and CRT/RenderWare callers, after the D3DX block","n"),
 ("5d6420","5d7c80","RenderWare","no strings; callees rwp x13 (RW wrapper code between D3DX and D3DX objects); lower confidence","n"),
 ("5d7c80","5dc000","D3DX","obj_get/set_offXX COM accessor methods and effect/animation code: D3DX callees (17-52 per block), strings tx_/dx_/$y_ (d3dx9 identifiers); previously counted as DirectShow, A2 narrowing exposed it","n"),
 ("5dc000","5dc451","RenderWare","RWC section (.rwcode, 1,105 B): RenderWare code section, no refs","n")
]
w=csv.writer(sys.stdout); w.writerow(["kind","lo","hi","bytes","section","layer","needs_C","evidence"])
for lo,hi,L,ev,nc in CODE: w.writerow(["code",lo,hi,H(hi)-H(lo),"text",L,nc,ev])
for r in runs:
    c=", ".join("%s %d"%kv for kv in r[4].most_common(3)) or "no refs (padding between lib rows): previous run's layer"
    w.writerow(["data","%08x"%r[0],"%08x"%r[1],r[1]-r[0],r[2],r[3],"n","referrer layers: "+c])
