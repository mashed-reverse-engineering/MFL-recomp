/* mflrw.dll d3d9dev binding (librw-mfl/capi/rwd3d9dev.cpp; LIB_RW group d3d9dev, engine switch window 5 lane K batch 4): the render target setters, vertex declaration delete,
   refresh rate, multisampling levels, camera end update and device state reset work on the exe's engine, device / Direct3D globals and the render target / cache variables
   (table order = rwd3d9dev.cpp enum D, 29 slots; globals declared by the d3dxform / d3dlights / d3dras2 blocks of an_hk_612.c are not repeated).  Include after
   an_hk_612_matfx2.inc. */
#ifndef MFL_DIFFEMU
extern int CurrentRenderSurface[], NumVertexDeclarations, VertexDeclarations, FullScreenRefreshRateInHz, g_d3d9calculatemaxmultisamplinglevels_9475f8, MaxMultisamplingLevels;
extern int NeedToCopyFromBackBuffer, g_rw_9476f4, CurrentDepthStencilSurface, g_rw_7d2688;
extern char BackBufferFormats[];
typedef void (__cdecl *MflrwD3d9devBind)(char **engine, void *const *table);
static void hk_bind_d3d9dev(int mflrw)
{
  static void *const d3d9devTable[29] = {
    &g_d3d9_device, CurrentRenderSurface, &_RwD3D9RasterExtOffset, &NumVertexDeclarations, &VertexDeclarations, &FullScreenRefreshRateInHz, BackBufferFormats,
    &_RwDirect3DObject, &_RwD3DAdapterIndex, &_RwD3DAdapterType, &g_rw_9475f0, &g_d3d9calculatemaxmultisamplinglevels_9475f8, &MaxMultisamplingLevels, &dgGGlobals,
    &NeedToCopyFromBackBuffer, &_RwD3D9RenderSurface, &g_rw_9476f4, &CurrentDepthStencilSurface, &_rwD3D9LastFVFUsed, LastVertexStreamUsed, &MatrixFreeList,
    &LastWorldMatrixUsedIdentity, LastMatrixUsed, (void *)RwFreeListCreate, (void *)RwFreeListPurge, &g_rw_7d2688, IdentityMatrix, &g_rw_7d269c, &g_rw_7d26a0};
  MflrwD3d9devBind b = mflrw ? (MflrwD3d9devBind)GetProcAddress(mflrw, "mflrw_d3d9dev_bind") : 0;
  if (b != 0) b(&RwEngineInstance, d3d9devTable);      /* the DLL copies the table */
}
#endif
