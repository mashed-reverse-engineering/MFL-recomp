import re,struct,os
d=open(os.environ.get('MFL_EXE',os.path.join(os.path.dirname(os.path.abspath(__file__)),'..','..','game','MFL.exe')),'rb').read()
pe=struct.unpack_from('<I',d,0x3c)[0]
ns=struct.unpack_from('<H',d,pe+6)[0]; so=struct.unpack_from('<H',d,pe+20)[0]
secs=[]
for i in range(ns):
    o=pe+24+so+40*i
    vs,va,rs,ro=struct.unpack_from('<IIII',d,o+8)
    secs.append((va+0x400000,vs,ro,rs))
def f2va(f):
    for va,vs,ro,rs in secs:
        if ro<=f<ro+rs: return va+f-ro
out=[]
for m in re.finditer(rb'\$Id: (//[^\s$]+?)#(\d+)',d):
    out.append((f2va(m.start()),m.group(1).decode(),m.group(2).decode()))
for a,p,r in out: print('%08x %s %s'%(a,p,r))
print([ (hex(va),hex(vs)) for va,vs,ro,rs in secs])
