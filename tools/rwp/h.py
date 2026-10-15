import sys;sys.path.insert(0,__import__('os').environ.get('TMPDIR','/tmp/claude-%d'%__import__('os').getuid())+'/s')
import os;os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)),'..','..'))
from map import *
t=collections.defaultdict(list)
for r in csv.reader(open('output/delink/ghidra_refs.csv')):
    if r[2]!='DATA':continue
    s=int(r[0],16);d=int(r[1],16)
    k=fn_of(s)
    if k is not None and not(0x56a000<=d<0x58c8a0) and d>=0x5c0000: t[k].append(d)
for k,it in enumerate(items):
    v=sorted(t.get(k,[]))
    print('%08x %5x %s'%(it[0],it[1]-it[0],' '.join('%x'%x for x in v[:6])))
