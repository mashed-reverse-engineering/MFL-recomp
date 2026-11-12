/* mflrw.dll d3d9rasq binding (librw-mfl/capi/rwd3d9rasq.cpp; LIB_RW group d3d9rasq, engine switch window 5 lane K batch 6): raster plugin attach, render quad, copy quad,
   clear generic, raster open and raster render work on the exe's raster extension offset, pixel format table, quad vertices, current raster target, back buffer surfaces,
   device and these exe callees (table order = rwd3d9rasq.cpp enum E, 33 slots).
   Include after an_hk_612_d3d9dev2.inc (it and an_hk_612.c declare the other names used here: _RwD3D9RasterExtOffset, _rwD3D9PixelFormatInfo, _RwD3D9CurrentRasterTarget,
   _RwD3D9RenderSurface, _RwD3D9DepthStencilSurface, g_d3d9_device, g_rw_9476f4, RwD3D9GetRenderState, _rwD3D9RWSetRasterStage, _rwD3D9RenderStateVertexAlphaEnable,
   rwD3D9RenderStateSrcBlend, rwD3D9RenderStateDestBlend, FUN_004e5be0, RwD3D9SetRenderState, _rwD3D9PixelToRGB, _rwD3D9RGBToPixel, _rwD3D9RasterLock, _rwD3D9RasterUnlock,
   FUN_0053eac0, RwErrorSet, RwFreeListCreate, VideoMemoryRastersFreeList, rwD3D9RasterRenderGeneric).  The slots of rows of other groups (RwD3D9SetRenderTarget,
   _rwD3D9SetRenderTarget, _rwD3D9BeginScene, rwD3D9RasterRenderGeneric, LAB_004dfb80 / LAB_004dfba0 = raster ctor / dtor) stay the original addresses, which the link turns
   into the DLL thunks.  NOT compiled in this lane: declarations below that clash with an earlier one in an_hk_612.c (type or prototype) are to be dropped, the scratch compile tells. */
#ifndef MFL_DIFFEMU
extern int _RwD3D9QuadVerts;
extern void __cdecl RwRasterRegisterPlugin(void), LAB_004dfb80(void), LAB_004dfba0(void), _rwD3D9RWGetRenderState(void), _rwD3D9Im2DRenderPrimitive(void);
extern void __cdecl _rwD3D9SetRenderTarget(void), RwD3D9SetRenderTarget(void), _rwD3D9SetDepthStencilSurface(void), _rwD3D9BeginScene(void);
typedef void (__cdecl *MflrwD3d9rasqBind)(char **engine, void *const *table);
static void hk_bind_d3d9rasq(int mflrw)
{
  static void *const d3d9rasqTable[33] = {
    &_RwD3D9RasterExtOffset, _rwD3D9PixelFormatInfo, (void *)RwRasterRegisterPlugin, (void *)LAB_004dfb80, (void *)LAB_004dfba0,
    /* 5.. */ &_RwD3D9CurrentRasterTarget, &_RwD3D9RenderSurface, &_RwD3D9DepthStencilSurface, &g_d3d9_device, &g_rw_9476f4,
    /* 10.. */ (void *)RwD3D9SetRenderTarget, (void *)_rwD3D9SetRenderTarget, (void *)_rwD3D9SetDepthStencilSurface, (void *)_rwD3D9BeginScene, &_RwD3D9QuadVerts,
    /* 15.. */ (void *)_rwD3D9RWGetRenderState, (void *)RwD3D9GetRenderState, (void *)_rwD3D9RWSetRasterStage, (void *)_rwD3D9RenderStateVertexAlphaEnable,
    (void *)rwD3D9RenderStateSrcBlend, (void *)rwD3D9RenderStateDestBlend, (void *)FUN_004e5be0, (void *)RwD3D9SetRenderState, (void *)_rwD3D9Im2DRenderPrimitive,
    /* 24.. */ (void *)_rwD3D9PixelToRGB, (void *)_rwD3D9RGBToPixel, (void *)_rwD3D9RasterLock, (void *)_rwD3D9RasterUnlock, (void *)FUN_0053eac0, (void *)RwErrorSet,
    /* 30.. */ (void *)RwFreeListCreate, &VideoMemoryRastersFreeList, (void *)rwD3D9RasterRenderGeneric};
  MflrwD3d9rasqBind b = mflrw ? (MflrwD3d9rasqBind)GetProcAddress(mflrw, "mflrw_d3d9rasq_bind") : 0;
  if (b != 0) b(&RwEngineInstance, d3d9rasqTable);      /* the DLL copies the table */
}
#endif
