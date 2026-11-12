/* mflrw.dll matfx2 binding (librw-mfl/capi/rwmatfx2.cpp; LIB_RW group matfx2, engine switch window 5 lane K batch 4): _rpMatFXD3D9AtomicMatFXDualPassRender and
   CalculatePerturbedUVs work on the exe's engine, device, matfx / video card globals and these exe callees (table order = rwmatfx2.cpp enum X, 30 slots).  Include after
   an_hk_612_matfx.inc (it declares the matfx globals and render functions used here); the rows' own slots stay the original addresses, which the link turns into the DLL
   thunks. */
#ifndef MFL_DIFFEMU
extern void __cdecl RwV3dTransformPoint(void), RwV3dTransformVector(void), _rpD3D9VertexDeclarationUnInstV3d(void), _rpD3D9VertexDeclarationUnInstV2d(void);
typedef void (__cdecl *MflrwMatfx2Bind)(char **engine, void *const *table);
static void hk_bind_matfx2(int mflrw)
{
  static void *const matfx2Table[30] = {
    &MatFXMaterialDataOffset, &VideoCardSupportsMultitexture, &VideoCardMaxTextureBlendStages, &VideoCardSupportsMultiplyAdd, &VideoCardSupportsModulate2X,
    &NeedToValidateModulate2X, (void *)FUN_004e55d0, (void *)RwD3D9SetTextureStageState, (void *)RwD3D9SetRenderState, (void *)RwD3D9GetRenderState,
    (void *)_rwD3D9RasterConvertToNonPalettized, (void *)RwD3D9SetTransform, (void *)_rwD3D9RenderStateFlushCache, g_rp_63a2d0,
    &g_d3d9_device, &_rwD3D9LastPixelShaderUsed, &_rwD3D9LastVertexShaderUsed, (void *)FUN_004e5950, (void *)_rwD3D9RenderStateVertexAlphaEnable,
    (void *)rwD3D9RenderStateSrcBlend, (void *)rwD3D9RenderStateDestBlend, (void *)_rpMatFXD3D9AtomicMatFXDefaultRender,
    /* 22.. */ (void *)RwD3D9GetTransform, (void *)RwMatrixInvert, (void *)RwFrameGetLTM, (void *)RwV3dTransformPoint, (void *)RwV3dTransformVector, (void *)_rwInvSqrt,
    (void *)_rpD3D9VertexDeclarationUnInstV3d, (void *)_rpD3D9VertexDeclarationUnInstV2d};
  MflrwMatfx2Bind b = mflrw ? (MflrwMatfx2Bind)GetProcAddress(mflrw, "mflrw_matfx2_bind") : 0;
  if (b != 0) b(&RwEngineInstance, matfx2Table);      /* the DLL copies the table */
}
#endif
