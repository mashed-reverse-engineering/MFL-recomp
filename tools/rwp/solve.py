import sys,struct;sys.path.insert(0,__import__('os').environ.get('TMPDIR','/tmp/claude-%d'%__import__('os').getuid())+'/s')
import os;os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)),'..','..'))
from scan import *
n=len(items)
elig=[i for i,r in enumerate(R) if '/dev/' not in r[2] and i<=35]
# padding items
pad=set()
for k,it in enumerate(items):
    code=b(it[0],it[1]-it[0])
    if all(c in (0xcc,0x90,0) for c in code): pad.add(k)
live=[k for k in range(n) if k not in pad]
# anchors
anc={}
for k in live:
    c=collections.Counter()
    for t,f,a in votes.get(k,[]): c[f]+=2 if t=='v' else 1
    for v in res[k]: c[fidx(v)]+=1
    c2=collections.Counter()
    for f,w in c.items():
        if f in (24,27,28,29,32): c2[19]+=w
        elif f in elig: c2[f]+=w
    c=dict(c2)
    if c: anc[k]=c
# edges between live items
pos={k:i for i,k in enumerate(live)}
cross=[0]*(len(live)+1)
edges=[]
for r in csv.reader(open('output/delink/ghidra_refs.csv')):
    if r[2] in('DATA',) or 'FLOW' in r[2] and 'JUMP' not in r[2]:continue
    try: s=int(r[0],16);t=int(r[1],16)
    except ValueError: continue
    a=fn_of(s);bb=fn_of(t)
    if a is None or bb is None or a==bb or a not in pos or bb not in pos: continue
    u,v=sorted((pos[a],pos[bb])); edges.append((u,v))
for u,v in set(edges):
    for p in range(u+1,v+1): cross[p]+=1
m=len(live); F=len(elig); INF=1e18
import math
sz=[0]
for k in live: sz.append(sz[-1]+items[k][1]-items[k][0])
pa=[[0]*(m+1) for _ in range(F)]
for fi in range(F):
    f=elig[fi]
    for i in range(m):
        a=anc.get(live[i]); c=0
        if a and a.get(f,0)!=max(a.values()): c=6*min(max(a.values()),4)
        pa[fi][i+1]=pa[fi][i]+c
EMPTY=8
dp=[[INF]*F for _ in range(m+1)]; bp=[[None]*F for _ in range(m+1)]
def seg(fi,j,i):
    L=sz[i]-sz[j]
    return 3*abs(math.log(L/3500))+pa[fi][i]-pa[fi][j]+(0.15*cross[j] if j>0 else 0)
for i in range(1,m+1):
    for fi in range(F):
        for j in range(0,i):
            if j==0: prev=EMPTY*fi;arg=None
            else:
                prev=INF
                for fj in range(fi):
                    c=dp[j][fj]+EMPTY*(fi-fj-1)
                    if c<prev: prev=c;arg=fj
            if prev>=INF: continue
            c=prev+seg(fi,j,i)
            if c<dp[i][fi]: dp[i][fi]=c;bp[i][fi]=(j,arg)
fi=min(range(F),key=lambda f:dp[m][f]+EMPTY*(F-1-f)); asg=[0]*m; i=m
while i>0:
    j,arg=bp[i][fi]
    for t in range(j,i): asg[t]=fi
    i=j; fi=arg
fileof={live[i]:elig[asg[i]] for i in range(m)}
if __name__=='__main__':
    for i in range(m):
        k=live[i]
        print('%08x %5x f%-2d %-12s %s'%(items[k][0],items[k][1]-items[k][0],fileof[k],R[fileof[k]][1][3:-0 or None][:12],'A'+str(sorted(anc[k])) if k in anc else ''))
