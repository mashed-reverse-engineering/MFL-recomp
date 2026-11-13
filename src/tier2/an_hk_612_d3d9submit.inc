/* mflrw.dll d3d9submit binding (librw-mfl/capi/d3d9submit.cpp; LIB_RW group d3d9submit, engine switch window 6 lane K1): the Im3D submit node (rxD3D9SubmitNode),
   _rwD3D9Im3DRenderOpen / Close and D3D9ImmInstanceNode work on the exe's device pointer, device caps, Im3D globals (0x7d522c..0x7d524b), "last used" caches,
   primitive type table (0x5ea2a8 = g_rw_5ea28c + 7 words) and these exe callees (table order = d3d9submit.cpp enum E, 27 slots).  No Win32 import slot, no trap row
   slot, no slot filled at run time: the table is const.
   Slot 2 is the current Im3D stash pointer 0x7d522c, the word in front of CurrentBaseIndex3D (it has no symbol of its own).
   Include after an_hk_612_noded3d9i.inc.  Declared earlier and not declared here: g_d3d9_device, _RwD3D9DeviceCaps, g_rw_5ea28c, RwD3D9SetRenderState,
   RwD3D9SetStreamSource, RwD3D9SetTransformWorld, the five _rwD3D9Last*Used words, RwD3D9CreateVertexDeclaration (an_hk_612_noded3d9i.inc).
   Compiled in the lane against a scratch copy of an_hk_612.c (agents/k1_w6/hk_scratch_compile.py). */
#ifndef MFL_DIFFEMU
extern int CurrentBaseIndex3D, CurrentVertexBuffer3D, IB3DOffset, IndexBuffer3D, VertexDeclIm3DOld, VertexDeclIm3D, VertexDeclIm3DNoTex;
extern void __cdecl _rwD3D9RenderStateVertexAlphaEnable(void), _rwD3D9RWGetRasterStage(void), RwD3D9SetTextureStageState(void), RwD3D9DynamicVertexBufferLock(void);
extern void __cdecl RwD3D9DynamicVertexBufferUnlock(void), _rwD3D9RenderStateFlushCache(void), RwD3D9DeleteVertexDeclaration(void);
typedef void (__cdecl *MflrwD3d9submitBind)(void *const *table);
static void hk_bind_d3d9submit(int mflrw)
{
  static void *const d3d9submitTable[27] = {
    &g_d3d9_device, _RwD3D9DeviceCaps, (char *)&CurrentBaseIndex3D - 4, &CurrentBaseIndex3D, &CurrentVertexBuffer3D, &IB3DOffset, &IndexBuffer3D,
    &VertexDeclIm3DOld, &VertexDeclIm3D, &VertexDeclIm3DNoTex,
    /* 10.. */ &_rwD3D9LastFVFUsed, &_rwD3D9LastVertexDeclarationUsed, &_rwD3D9LastVertexShaderUsed, &_rwD3D9LastPixelShaderUsed, &_rwD3D9LastIndexBufferUsed,
    g_rw_5ea28c + 7,
    /* 16.. */ (void *)_rwD3D9RenderStateVertexAlphaEnable, (void *)RwD3D9SetTransformWorld, (void *)RwD3D9SetRenderState, (void *)_rwD3D9RWGetRasterStage,
    (void *)RwD3D9SetTextureStageState, (void *)RwD3D9DynamicVertexBufferLock, (void *)RwD3D9DynamicVertexBufferUnlock, (void *)RwD3D9SetStreamSource,
    (void *)_rwD3D9RenderStateFlushCache, (void *)RwD3D9CreateVertexDeclaration, (void *)RwD3D9DeleteVertexDeclaration};
  MflrwD3d9submitBind b = mflrw ? (MflrwD3d9submitBind)GetProcAddress(mflrw, "mflrw_d3d9submit_bind") : 0;
  if (b != 0) b(d3d9submitTable);      /* the DLL copies the table */
}
#endif
