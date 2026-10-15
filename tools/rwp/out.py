import sys;sys.path.insert(0,__import__('os').environ.get('TMPDIR','/tmp/claude-%d'%__import__('os').getuid())+'/s')
from solve import *
def short(i): return R[i][1][3:].lower()   # RwpGjk -> gjk
# per-file stats
files=collections.OrderedDict()
direct=collections.defaultdict(set)
for k in live:
    for f in anc.get(k,{}):
        pass
# anchored f per function = argmax of anc
for k,a in anc.items():
    direct[k]=max(a,key=a.get)
anch_by_file=collections.defaultdict(list)
for k,f in direct.items():
    if f==fileof[k]: anch_by_file[f].append(k)
def conf(k):
    f=fileof[k]; A=anch_by_file.get(f,[])
    if k in A: return 'H'
    if A and min(A)<=k<=max(A): return 'M'
    return 'L'
vt=collections.defaultdict(list)
for r in csv.reader(open('output/delink/ghidra_refs.csv')):
    if r[2]!='DATA': continue
    try: s=int(r[0],16);t=int(r[1],16)
    except: continue
    if 0x5f6335<=s<0x5f8f00 and fn_of(t) is not None and t in [it[0] for it in items]:
        k=fn_of(t); 
        if k in fileof: vt[fileof[k]].append(s)
order=sorted(set(fileof.values()))
elig_all=elig
with open('output/rwp_files.csv','w') as o:
    o.write('source_file,rcs_rev,code_start,code_end,n_functions,bytes,n_anchored,confidence,vtable_slots,unresolved_files_after\n')
    for idx,f in enumerate(order):
        ks=[k for k in live if fileof[k]==f]
        lo=items[ks[0]][0]; hi=items[ks[-1]][1]
        nxt=order[idx+1] if idx+1<len(order) else None
        miss=[short(e) for e in elig if f<e and (nxt is None or e<nxt)] if nxt is not None else []
        na=len(anch_by_file.get(f,[]))
        cf='high' if na>=3 and not miss else ('medium' if na>=1 else 'low')
        slots=sorted(set(vt.get(f,[])))
        o.write('%s,%s,%08x,%08x,%d,%d,%d,%s,%s,%s\n'%(R[f][2].split('/src/')[-1].split('Physics/')[-1] if False else R[f][1],R[f][3],lo,hi,len(ks),sum(items[k][1]-items[k][0] for k in ks),na,cf,' '.join('%x'%s for s in slots),'+'.join(miss)))
n=0
with open('output/rwp_names.csv','w') as o:
    o.write('# Rwp 3.7 function names (PROPOSAL, not applied): address,name,comment. See output/rwp_files.csv and ROADMAP.md "Rwp block naming".\n')
    for k in live:
        it=items[k]
        if not (it[4].startswith('FUN_') or it[4].startswith('gap_')): continue
        f=fileof[k]
        o.write('%08x,rwp_%s_%08x,%s.c conf=%s (was %s)\n'%(it[0],short(f),it[0],R[f][1],conf(k),it[4])); n+=1
print(n)
