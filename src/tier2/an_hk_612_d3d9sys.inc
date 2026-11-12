/* mflrw.dll d3d9sys binding (librw-mfl/capi/rwd3d9sys.cpp; LIB_RW group d3d9sys, engine switch window 5 lane K batch 6): D3D9System works on the exe's engine, its
   Direct3D / device globals, display mode list, device caps, present parameters, CPU flags and these exe callees / import slots (table order = rwd3d9sys.cpp enum E, 62 slots;
   the last slot is the array of the 12 vector transform functions of request 17).
   Include after an_hk_612_d3d9dev2.inc (it, an_hk_612_d3d9dev.inc and an_hk_612.c declare the other names used here: SystemStarted, NumDisplayModes, DisplayModes, g_rx_9475e4,
   g_rw_9475f4, g_d3d9calculatemaxmultisamplinglevels_9475f8, MaxMultisamplingLevels, SelectedMultisamplingLevels, SelectedMultisamplingLevelsNonMask, _RwDirect3DObject,
   _RwD3DAdapterType, _RwD3DAdapterIndex, D3D9CreateDisplayModesList (declare it if the compile misses it), g_d3d9_device, g_rw_947760, WindowHandle, _RwD3D9DeviceCaps,
   g_d3d9createdisplaymodeslist_9475e0, BackBufferFormats, _rwD3D9CPUSupportsSSE, Present, g_data_7d226c, GetClientRect (dllimport), _RwD3D9RenderSurface,
   _RwD3D9DepthStencilSurface, CurrentDepthStencilSurface, CurrentRenderSurface, g_rw_7d26a0, g_rw_7d269c, _rwD3D9Im2DRenderOpen).  The slots of rows of other groups
   (D3D9CreateDisplayModesList, D3D9CalculateMaxMultisamplingLevels, FUN_004d8990 = D3D9SetPresentParameters, D3D9DeviceSystemStop, D3D9DeviceSystemStandards, _rwD3D9RasterOpen)
   stay the original addresses, which the link turns into the DLL thunks.  NOT compiled in this lane: declarations below that clash with an earlier one in an_hk_612.c (type or
   prototype) are to be dropped, the scratch compile tells. */
#ifndef MFL_DIFFEMU
extern int g_data_7d2230, MaxMultisamplingLevelsNonMask, g_data_62e7b8[], g_data_9473c4, g_data_7d21e0[], g_data_7d26b8, g_data_7d26c0, g_data_7d26c4;
extern int g_data_7d2264, g_data_7d2268;
extern char _RwD3D9AdapterInformation[], g_data_62e830[];
/* the 3 imports: the exe's IAT slots / import thunk have no symbol in the standalone IMPLIBS link (as in an_hk_612_d3d9dev2.inc, which declares GetClientRect): ShowWindow and
   GetClientRect are called through a slot pointer (d3d9sysImp[]), Direct3DCreate9 is called directly (slot 21, filled before the bind; so the table is not const) */
extern __declspec(dllimport) int __stdcall ShowWindow(int wnd, int cmd);
extern __declspec(dllimport) void *__stdcall Direct3DCreate9(unsigned int sdkVersion);
extern void __cdecl D3D9CalculateMaxMultisamplingLevels(void), _rwCPUHaveMMX(void), _rwCPUHaveSSE(void), _rwCPUHaveSSE2(void), _rwCPUHave3DNow(void);
extern void __cdecl FUN_004d8990(void), D3D9ClearCacheShaders(void), D3D9ClearCacheMatrix(void), _rwD3D9RasterOpen(void), _rwD3D9RenderStateOpen(void);
extern void __cdecl _rwD3D9VertexBufferManagerOpen(void), D3D9DeviceSystemStop(void), D3D9DeviceSystemStandards(void), FUN_005dc404(void), _rwIntelx86AsmMatrixMultiply(void);
extern void __cdecl rwMatrixSetMultFn(void), _rwVectorSetMultFn(void), D3D9CreateDisplayModesList(void);
extern void __cdecl _rwIntelSSEP4V3dTransformPoint(void), _rwIntelSSEP4V3dTransformPoints(void), _rwIntelSSEP4V3dTransformVector(void), _rwIntelSSEP4V3dTransformVectors(void);
extern void __cdecl _rwIntelSSEV3dTransformPoint(void), _rwIntelSSEV3dTransformPoints(void), _rwIntelSSEV3dTransformVector(void), _rwIntelSSEV3dTransformVectors(void);
extern void __cdecl _rwIntelx86AsmV3dTransformPoint(void), _rwIntelx86AsmV3dTransformPoints(void), _rwIntelx86AsmV3dTransformVector(void), _rwIntelx86AsmV3dTransformVectors(void);
typedef void (__cdecl *MflrwD3d9sysBind)(char **engine, void *const *table);
static void hk_bind_d3d9sys(int mflrw)
{
  /* request 17: 3 sets of {point, points, vector, vectors}: SSE P4, SSE, x86 */
  static void *const d3d9sysVec[12] = {
    (void *)_rwIntelSSEP4V3dTransformPoint, (void *)_rwIntelSSEP4V3dTransformPoints, (void *)_rwIntelSSEP4V3dTransformVector, (void *)_rwIntelSSEP4V3dTransformVectors,
    (void *)_rwIntelSSEV3dTransformPoint, (void *)_rwIntelSSEV3dTransformPoints, (void *)_rwIntelSSEV3dTransformVector, (void *)_rwIntelSSEV3dTransformVectors,
    (void *)_rwIntelx86AsmV3dTransformPoint, (void *)_rwIntelx86AsmV3dTransformPoints, (void *)_rwIntelx86AsmV3dTransformVector, (void *)_rwIntelx86AsmV3dTransformVectors};
  static void *d3d9sysImp[2];      /* ShowWindow, GetClientRect */
  void *t[62]; int i;      /* stack copy with slot 21 filled: a non-const static table would grow .data (the standalone layout check wants its length unchanged) */
  static void *const d3d9sysTable[62] = {
    &SystemStarted, &NumDisplayModes, &DisplayModes, &g_data_7d2230, &g_rx_9475e4, &g_rw_9475f4, &g_d3d9calculatemaxmultisamplinglevels_9475f8,
    /* 7.. */ &MaxMultisamplingLevels, &SelectedMultisamplingLevels, &MaxMultisamplingLevelsNonMask, &SelectedMultisamplingLevelsNonMask, &_RwDirect3DObject, &_RwD3DAdapterType,
    &_RwD3DAdapterIndex, (void *)D3D9CreateDisplayModesList,
    /* 15.. */ &g_d3d9_device, &g_rw_947760, &WindowHandle, &d3d9sysImp[0], g_data_62e7b8, &g_data_9473c4,
    /* 21.. */ 0 /* Direct3DCreate9, set below */, _RwD3D9AdapterInformation, g_data_62e830, _RwD3D9DeviceCaps, &g_d3d9createdisplaymodeslist_9475e0, BackBufferFormats, g_data_7d21e0,
    (void *)D3D9CalculateMaxMultisamplingLevels,
    /* 29.. */ (void *)_rwCPUHaveMMX, (void *)_rwCPUHaveSSE, (void *)_rwCPUHaveSSE2, (void *)_rwCPUHave3DNow, &g_data_7d26b8, &_rwD3D9CPUSupportsSSE, &g_data_7d26c0, &g_data_7d26c4,
    /* 37.. */ Present, (void *)FUN_004d8990, &g_data_7d226c, &g_data_7d2264, &g_data_7d2268, &d3d9sysImp[1], &_RwD3D9RenderSurface, &_RwD3D9DepthStencilSurface,
    &CurrentDepthStencilSurface, CurrentRenderSurface,
    /* 47.. */ (void *)D3D9ClearCacheShaders, (void *)D3D9ClearCacheMatrix, &g_rw_7d26a0, &g_rw_7d269c, (void *)_rwD3D9RasterOpen, (void *)_rwD3D9Im2DRenderOpen,
    (void *)_rwD3D9RenderStateOpen, (void *)_rwD3D9VertexBufferManagerOpen,
    /* 55.. */ (void *)D3D9DeviceSystemStop, (void *)D3D9DeviceSystemStandards, (void *)FUN_005dc404, (void *)_rwIntelx86AsmMatrixMultiply, (void *)rwMatrixSetMultFn,
    (void *)_rwVectorSetMultFn, (void *)d3d9sysVec};
  MflrwD3d9sysBind b = mflrw ? (MflrwD3d9sysBind)GetProcAddress(mflrw, "mflrw_d3d9sys_bind") : 0;
  d3d9sysImp[0] = (void *)ShowWindow; d3d9sysImp[1] = (void *)GetClientRect;
  for (i = 0; i < 62; i++) t[i] = d3d9sysTable[i];
  t[21] = (void *)Direct3DCreate9;
  if (b != 0) b(&RwEngineInstance, t);      /* the DLL copies the table */
}
#endif
