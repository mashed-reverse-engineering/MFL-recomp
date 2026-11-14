/* mflrw.dll d3d9vs2 binding (librw-mfl/capi/d3d9vs2.cpp; LIB_RW group d3d9vs2, engine switch window 8 lane K4): rxD3D9VertexShaderDefaultMeshRenderCallBack and
   rxD3D9VertexShaderDefaultLightingCallBack work on the exe's last pixel / vertex shader globals, the device, the engine (current world, light frame word) and these
   exe callees (table order = d3d9vs2.cpp enum V, 13 slots).  The three _rwD3D9VSGet*InLocalSpace slots are the original addresses, which the link turns into the
   DLL thunks of group d3d9vsu.  Declared earlier (an_hk_612.c / an_hk_612_d3d9pipe.inc): _rwD3D9LastPixelShaderUsed, _rwD3D9LastVertexShaderUsed, g_d3d9_device,
   FUN_004e55d0 (= RwD3D9SetTexture), RwD3D9SetTextureStageState, _rwD3D9RenderStateFlushCache, RwEngineInstance, RwFrameGetLTM, RpAtomicGetWorldBoundingSphere,
   RpLightGetConeAngle.  No Win32 import slot, no slot filled at run time, no 0 slot, no trap row (vertex shader code is never trapped). */
#ifndef MFL_DIFFEMU
extern void __cdecl _rwD3D9VSGetNormalInLocalSpace(void), _rwD3D9VSGetPointInLocalSpace(void), _rwD3D9VSGetRadiusInLocalSpace(void);
typedef void (__cdecl *MflrwD3d9vs2Bind)(void *const *table);
static void hk_bind_d3d9vs2(int mflrw)
{
  static void *const d3d9vs2Table[13] = {
    &_rwD3D9LastPixelShaderUsed, &_rwD3D9LastVertexShaderUsed, &g_d3d9_device, (void *)FUN_004e55d0, (void *)RwD3D9SetTextureStageState,
    (void *)_rwD3D9RenderStateFlushCache, &RwEngineInstance, (void *)RwFrameGetLTM, (void *)RpAtomicGetWorldBoundingSphere, (void *)RpLightGetConeAngle,
    (void *)_rwD3D9VSGetNormalInLocalSpace, (void *)_rwD3D9VSGetPointInLocalSpace, (void *)_rwD3D9VSGetRadiusInLocalSpace};
  MflrwD3d9vs2Bind b = mflrw ? (MflrwD3d9vs2Bind)GetProcAddress(mflrw, "mflrw_d3d9vs2_bind") : 0;
  if (b != 0) b(d3d9vs2Table);      /* the DLL copies the table */
}
#endif
