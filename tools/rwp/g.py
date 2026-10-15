import sys;sys.path.insert(0,__import__('os').environ.get('TMPDIR','/tmp/claude-%d'%__import__('os').getuid())+'/s')
import os;os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)),'..','..'))
from map import *
import collections
names={}
for r in csv.reader(open('output/delink_sdk/ownership_items.csv')):
    if r[0]!='lo': names[int(r[0],16)]=(r[3],r[5])
out=collections.defaultdict(set); inn=collections.defaultdict(set)
for r in csv.reader(open('output/delink/ghidra_refs.csv')):
    if 'CALL' not in r[2]: continue
    s=int(r[0],16);t=int(r[1],16)
    a=fn_of(s);b=fn_of(t)
    if b is not None and a!=b: inn[b].add(a if a is not None else s)
    if a is not None and a!=b: out[a].add(b if b is not None else t)
for k,it in enumerate(items):
    if it[3]!='game':continue
    ext=[x for x in inn[k] if not isinstance(x,int) or x>0x58c8a0 or x<0x56a000]
    print('%08x %5x callers_out=%s callers_in=%d callees_in=%d'%(it[0],it[1]-it[0],[ '%x'%x for x in ext][:3],len([x for x in inn[k] if x<len(items)]), len([x for x in out[k] if x is not None and x<len(items)])))
