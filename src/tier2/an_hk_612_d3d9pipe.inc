/* mflrw.dll d3d9pipe binding (librw-mfl/capi/d3d9pipe.cpp, LIB_RW group d3d9pipe, engine switch window 3 D5): the D3D9 world pipeline (instance / render callbacks,
   resource entry destroy) works on the exe's device, engine (current camera, RwMalloc / RwFree), mesh module, primitive table 0x5ea2f4, the cache words
   0x62e760..0x62e770 (declared with d3dxform) and these exe callees (table order = d3d9pipe.cpp enum B; qsort, RwD3D9SetRenderState, RwD3D9SetTextureStageState,
   RwD3D9DeleteVertexDeclaration and _rwD3D9RenderStateFlushCache declared above).  A missing or refused bind is fatal when the group is linked, optional when it is not. */
#ifndef MFL_DIFFEMU
/*004faa20*/ extern unsigned char rxD3D9Instance[];      /* a jmp [__imp_] thunk (ff 25) when LIB_RW lists d3d9pipe */
/*005ea2f4*/ extern int g_rw_5ea2f4[];
extern void __cdecl RwResourcesAllocateResEntry(void), _rwD3D9ConvertToTriList(void), _rwD3D9ResourceEntryInstanceDataDestroy(void), _rpD3D9RemoveDynamicGeometry(void),
  RwD3D9DestroyVertexBuffer(void), RpAtomicGetWorldBoundingSphere(void), RwD3D9CameraIsSphereFullyInsideFrustum(void), RwD3D9CameraIsBBoxFullyInsideFrustum(void),
  FUN_004e55d0(void), _rwD3D9RenderStateVertexAlphaEnable(void), RwD3D9SetSurfaceProperties(void), _rwD3D9TextureHasAlpha(void),
  _rwD3D9SetStreams(void);      /* 0x4ea960 .. 0x4db2d0, original code or DLL thunks; FUN_004e55d0 = RwD3D9SetTexture */
/*004ea870*/ extern int __cdecl RwResourcesFreeResEntry(void *entry);      /* these five are C of ours: declared as defined (declcheck) */
/*004db980*/ extern int __cdecl RwD3D9IndexBufferCreate(int n, void *out);
/*004ecca0*/ extern void __cdecl RwD3D9DynamicVertexBufferDestroy(void *vb);
/*004e3c50*/ extern void __cdecl RwD3D9GetRenderState(int s, int *out);
/*004e5950*/ extern unsigned int __cdecl FUN_004e5950(void);      /* _rwD3D9RenderStateIsVertexAlphaEnable */
static void hk_bind_d3d9pipe(int mflrw)
{
  static void *const d3d9pipeTable[32] = {
    &g_d3d9_device, &RwEngineInstance, &g_tristripstriptris_947778, g_rw_5ea2f4, &_rwD3D9LastFVFUsed, &_rwD3D9LastVertexDeclarationUsed,
    &_rwD3D9LastVertexShaderUsed, &_rwD3D9LastPixelShaderUsed, &_rwD3D9LastIndexBufferUsed,
    (void *)RwResourcesAllocateResEntry, (void *)RwResourcesFreeResEntry, (void *)RwD3D9IndexBufferCreate, (void *)_rwD3D9ConvertToTriList, (void *)qsort,
    (void *)_rwD3D9ResourceEntryInstanceDataDestroy, (void *)_rpD3D9RemoveDynamicGeometry, (void *)RwD3D9DeleteVertexDeclaration, (void *)RwD3D9DestroyVertexBuffer,
    (void *)RwD3D9DynamicVertexBufferDestroy, (void *)RpAtomicGetWorldBoundingSphere, (void *)RwD3D9CameraIsSphereFullyInsideFrustum,
    (void *)RwD3D9CameraIsBBoxFullyInsideFrustum, (void *)RwD3D9SetRenderState, (void *)RwD3D9GetRenderState, (void *)RwD3D9SetTextureStageState,
    (void *)FUN_004e55d0, (void *)_rwD3D9RenderStateVertexAlphaEnable, (void *)FUN_004e5950, (void *)RwD3D9SetSurfaceProperties, (void *)_rwD3D9TextureHasAlpha,
    (void *)_rwD3D9RenderStateFlushCache, (void *)_rwD3D9SetStreams};
  MflrwD3dim2dBind b = mflrw ? (MflrwD3dim2dBind)GetProcAddress(mflrw, "mflrw_d3d9pipe_bind") : 0;
  int linked = rxD3D9Instance[0] == 0xff && rxD3D9Instance[1] == 0x25;
  int ok = b != 0 && b(32, d3d9pipeTable);
  if (linked && !ok) {
    MessageBoxA(0, "mflrw.dll has no usable mflrw_d3d9pipe_bind, but this exe takes the D3D9 world pipeline instance / render callbacks (LIB_RW group d3d9pipe) from it.", "MFL", 0x10);
    ExitProcess(3);
  }
}
#endif
