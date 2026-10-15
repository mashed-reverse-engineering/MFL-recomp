import csv,bisect,collections
R=[(int(a,16),p.split('/')[-1][:-2],p,r) for a,p,r in (l.split() for l in open(__import__('os').environ.get('TMPDIR','/tmp/claude-%d'%__import__('os').getuid())+'/s/rcs.txt'))]
starts=[x[0] for x in R]
def fidx(a):
    if a<starts[0] or a>=0x5f9000: return None
    return bisect.bisect_right(starts,a)-1
items=[]
for r in csv.reader(open('output/delink_sdk/ownership_items.csv')):
    if r[0]=='lo':continue
    lo=int(r[0],16);hi=int(r[1],16)
    if 0x569b30<=lo<0x58c8a0: items.append((lo,hi,r[2],r[3],r[5]))
los=[i[0] for i in items]
def fn_of(a):
    k=bisect.bisect_right(los,a)-1
    return k if k>=0 and a<items[k][1] else None
votes=collections.defaultdict(list)
for r in csv.reader(open('output/delink/ghidra_refs.csv')):
    if r[2]!='DATA':continue
    s=int(r[0],16);t=int(r[1],16)
    # code -> data static
    k=fn_of(s)
    if k is not None and fidx(t) is not None: votes[k].append(('d',fidx(t),t))
    # vtable slot -> code
    k=fn_of(t)
    if k is not None and fidx(s) is not None and not (0x569b30<=s<0x58c8a0): votes[k].append(('v',fidx(s),s))
if __name__=='__main__':
    for k,it in enumerate(items):
        v=votes.get(k,[])
        c=collections.Counter(x[1] for x in v)
        print('%08x %05x %-5s %s %s'%(it[0],it[1]-it[0],it[3],it[4][:14],' '.join('%s%d:%s'%(('V' if 'v' in [x[0] for x in v if x[1]==f] else 'd'),f,R[f][1][3:9]) for f in sorted(c))))
