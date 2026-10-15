import sys,struct;sys.path.insert(0,__import__('os').environ.get('TMPDIR','/tmp/claude-%d'%__import__('os').getuid())+'/s')
import os;os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)),'..','..'))
from map import *
d=open(os.environ.get('MFL_EXE','game/MFL.exe'),'rb').read()
def b(va,n): o=va-0x401000+0x400; return d[o:o+n]  # check text raw offset
# text raw offset
pe=struct.unpack_from('<I',d,0x3c)[0]
ro=struct.unpack_from('<I',d,pe+24+struct.unpack_from('<H',d,pe+20)[0]+20)[0]
def b(va,n): o=va-0x401000+ro; return d[o:o+n]
res={}
for k,it in enumerate(items):
    code=b(it[0],it[1]-it[0]); hits=[]
    for i in range(len(code)-3):
        v=struct.unpack_from('<I',code,i)[0]
        if 0x5f6335<=v<0x5f8f00: hits.append(v)
    res[k]=hits
if __name__=='__main__':
  for k,it in enumerate(items):
    f=collections.Counter(fidx(v) for v in res[k])
    print('%08x %5x %s'%(it[0],it[1]-it[0],' '.join('%d:%s*%d'%(i,R[i][1][3:8],n) for i,n in sorted(f.items()))))
