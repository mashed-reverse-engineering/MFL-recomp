#!/usr/bin/env python3
"""gen_funcs.py [MFL.exe]: write funcs.txt = the RW entry points the tap hooks.
Line: name va len nargs spec...   nargs = stack dwords logged (default 8 when unknown); len = bytes of whole instructions (>= 5) displaced by the hook; spec = what to dump behind pointer args:
  pK:N  = N bytes at arg K;  pK:nM*S = (arg M) * S bytes at arg K.   (arg K = dword at esp+4+4K on entry, cdecl)
The prologue must be position independent (no call / jmp / jcc inside the displaced bytes), else the function is listed as '#skip'."""
import os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pe import PE
import capstone
R = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
p = PE(sys.argv[1] if len(sys.argv) > 1 else R + '/../MashedGame/FullyLoaded/installed/App_Executables/MFL.exe')
syms = {}
for l in open(R + '/src/symbols.txt'):
    w = l.split()
    if len(w) == 2: syms[w[0]] = int(w[1], 16)
na = {}
for f_ in ('cdecl_args.txt', 'call_pushmin.txt'):     # stack argument counts (output/, tools/origargs.py + call-site scan); cdecl_args first
    for l in open(R + '/output/' + f_):
        w = l.split()
        if len(w) >= 2 and int(w[0], 16) not in na: na[int(w[0], 16)] = int(w[1])
na.update({0x4eb6d0: 4, 0x4eb960: 5, 0x4ebdb0: 5})      # Im2D Line / Triangle / IndexedPrimitive: not in the tables
# device table at 0x62e7a8 (RwEngineInstance + 0x20 is dOpenDevice.fpRenderStateSet = [0x62e7c8]): not in symbols.txt
dev = {'RwRenderStateSet': 0x4e5be0, 'RwRenderStateGet': 0x4e5070, 'RwIm2DRenderLine': 0x4eb6d0, 'RwIm2DRenderTriangle': 0x4eb960,
       'RwIm2DRenderPrimitive': 0x4ebb80, 'RwIm2DRenderIndexedPrimitive': 0x4ebdb0}
V28 = 28   # RwIm2DVertex
spec = {'RwIm2DRenderPrimitive': 'p1:n2*28', 'RwIm2DRenderIndexedPrimitive': 'p1:n2*28 p3:n4*2', 'RwIm2DRenderLine': 'p0:56', 'RwIm2DRenderTriangle': 'p0:84',
        'RwIm3DTransform': 'p0:n1*36 p2:64', 'RwIm3DRenderIndexedPrimitive': 'p1:n2*2', 'RwD3D9SetTransform': 'p1:64', 'RwD3D9SetTransformWorld': 'p0:64',
        'RwD3D9SetLight': 'p1:104', 'RwCameraSetViewWindow': 'p1:8', 'RwCameraSetViewOffset': 'p1:8', 'RwCameraBeginUpdate': 'p0:256', 'RwCameraEndUpdate': 'p0:256'}
want = re.compile(r'^(RwD3D9(Set|Get|Enable|Camera|Dynamic|IndexBuffer|Create|Delete)|_rwD3D9(SetStreams|SetVertexDeclaration|BeginScene|RenderStateFlushCache)|rwD3D9RasterRender|'
                  r'RwCamera(BeginUpdate|EndUpdate|Clear|ShowRaster|SetProjection|SetViewWindow|SetViewOffset|SetNearClipPlane|SetFarClipPlane)$|RwIm3D(Transform|End|Render)|RwRasterShowRaster)')
funcs = dict(dev)
for n, v in syms.items():
    if want.match(n): funcs[n] = v
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = True
out = []
for n, v in sorted(funcs.items(), key=lambda kv: kv[1]):
    n_ = 0; bad = None
    for x in md.disasm(p.read(v, 24), v):
        if x.group(capstone.CS_GRP_JUMP) or x.group(capstone.CS_GRP_CALL) or x.group(capstone.CS_GRP_RET) or x.group(capstone.CS_GRP_BRANCH_RELATIVE) or 'eip' in x.op_str: bad = x.mnemonic; break
        n_ += x.size
        if n_ >= 5: break
    out.append(f'{n} {v:x} {n_} {min(na.get(v, 8), 8)} {spec.get(n, "")}'.rstrip() if bad is None and n_ >= 5 else f'#skip {n} {v:x} ({bad or "short"})')
open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'funcs.txt'), 'w').write('\n'.join(out) + '\n')
print(sum(not l.startswith('#') for l in out), 'hooks,', sum(l.startswith('#') for l in out), 'skipped')
