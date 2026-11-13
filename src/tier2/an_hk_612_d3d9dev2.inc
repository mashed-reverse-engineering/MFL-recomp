/* mflrw.dll d3d9dev2 binding (librw-mfl/capi/rwd3d9dev2.cpp; LIB_RW group d3d9dev2, engine switch window 5 lane K batch 5): the vertex declaration create, 32 bit index buffer,
   camera attach window / clear, device stop, begin scene, release video memory, standards, display modes list, show raster and present parameter functions work on the exe's
   engine, device / Direct3D globals, the present parameters, the render target caches and these exe callees / Win32 import slots (table order = rwd3d9dev2.cpp enum E, 113 slots).
   Include after an_hk_612_d3d9dev.inc (it declares NumVertexDeclarations, VertexDeclarations, FullScreenRefreshRateInHz, CurrentRenderSurface, CurrentDepthStencilSurface, g_rw_9476f4,
   g_rw_7d2688, BackBufferFormats).  The rows' own slots stay the original addresses, which the link turns into the DLL thunks.  Names an earlier block of an_hk_612.c or its
   .inc files declares (stub_return_zero, RwErrorSet, RwFrameGetLTM, RwMatrixInvert, RwD3D9SetTransform / SetRenderState, RwImage*, RwRasterCreate / Destroy, RwFreeListDestroy,
   FUN_0053eac0, _rwD3D9CheckValidCameraTextureFormat, _rxD3D9VideoMemoryRasterListRestore, the palette / video memory raster variables, dgGGlobals ...) are not repeated. */
#ifndef MFL_DIFFEMU
extern int MaxVertexDeclarations, g_rw_947760, SystemStarted, DisplayModes, NumDisplayModes, _rwD3D9CPUSupportsSSE, D3D9RestoreDeviceCallback, D3D9ReleaseDeviceCallback, InsideScene;
extern int g_d3d9createdisplaymodeslist_9475e0, g_data_947774, SelectedMultisamplingLevels, SelectedMultisamplingLevelsNonMask, g_data_7d226c, _RwHasStencilBuffer;
extern int StencilClearValue, g_data_5ea0d0[];
extern int _RwD3D9CurrentRasterTarget;
extern int _RwD3D9D3D9ViewTransform[], _RwD3D9D3D9ProjTransform[], _RwD3D9ActiveViewProjTransform;
extern void __cdecl FUN_005dc432(void), rwD3D9RasterRenderGeneric(void);
extern void __cdecl D3D9RasterClearGeneric(void), rwD3D9RasterCopyQuad(void), rwD3D9RasterRenderQuad(void);
extern char s_SRC__DST_Raster_render_combinati_0062e880[];
extern void __cdecl _rwD3D9CheckValidCameraCubeTextureFormat(void);
/* the 8 user32 imports the DLL calls through a slot pointer: the exe's IAT slots have no symbol in the standalone IMPLIBS link (PTR_..._005dd2xx only exist in the overlay
   build), so the slots are d3d9dev2Imp[], filled from the dllimport functions before the bind (same pointer values as the IAT slots; the DLL reads a slot at call time) */
extern __declspec(dllimport) int __stdcall GetWindowRect(int wnd, void *rect);
extern __declspec(dllimport) int __stdcall GetClientRect(int wnd, void *rect);
extern __declspec(dllimport) int __stdcall IsIconic(int wnd);
extern __declspec(dllimport) int __stdcall GetWindowLongA(int wnd, int index);
extern __declspec(dllimport) int __stdcall SetWindowLongA(int wnd, int index, int value);
extern __declspec(dllimport) int __stdcall GetMenu(int wnd);
extern __declspec(dllimport) int __stdcall AdjustWindowRectEx(void *rect, unsigned int style, int menu, unsigned int exStyle);
extern __declspec(dllimport) int __stdcall SetWindowPos(int wnd, int after, int x, int y, int cx, int cy, unsigned int flags);
extern void __cdecl FUN_005dc42b(void), _rwD3D9RasterClose(void), _rwD3D9VertexBufferManagerClose(void);
extern void __cdecl _rwD3D9Im2DRenderClose(void), D3D9DeviceReleaseVideoMemory(void), _rwD3D9DynamicVertexBufferRestore(void);
extern void __cdecl _rwD3D9RenderStateReset(void), _rwD3D9Im2DRenderOpen(void), _rwD3D9Im3DRenderOpen(void), _rwD3D9Im3DRenderClose(void), _rwD3D9DynamicVertexBufferRelease(void);
extern void __cdecl _rxD3D9VideoMemoryRasterListRelease(void), RwFreeListPurgeAllFreeLists(void), rwD3D9DynamicVertexBufferManagerForceDiscard(void), RwD3D9CameraAttachWindow(void);
extern void __cdecl LAB_004da2a0(void), LAB_004dac70(void), LAB_004d9420(void), LAB_004dad50(void), _rwD3D9RGBToPixel(void), _rwD3D9PixelToRGB(void);
extern void __cdecl _rwD3D9RasterSetFromImage(void), _rwD3D9ImageGetFromRaster(void), _rwD3D9RasterDestroy(void), _rwD3D9RasterCreate(void), LAB_004ef150(void), LAB_004e3280(void);
extern void __cdecl _rwD3D9RasterLock(void), _rwD3D9RasterUnlock(void), LAB_004e02d0(void), LAB_004e0360(void), LAB_004e13a0(void), LAB_004e0db0(void), LAB_004e03f0(void);
extern void __cdecl LAB_004e0d90(void), LAB_004e0d70(void), LAB_004e13e0(void), LAB_004e3290(void), _rwD3D9NativeTextureGetSize(void), LAB_004ed780(void), LAB_004e3200(void);
typedef void (__cdecl *MflrwD3d9dev2Bind)(char **engine, void *const *table);
HK_TRAP_STUB(mfl_trap_rwD3D9NativeTextureWrite_004ed400)
static void hk_bind_d3d9dev2(int mflrw)
{
  /* the original's 27 index / function pairs of D3D9DeviceSystemStandards, in the order of its constant pairs (the stub first).  rwD3D9NativeTextureWrite 0x4ed400 is a trap
     row (rw_trap.txt): no kept code may name a trapped function (trapcheck), so its slot is a trap stub (HK_TRAP_STUB, an_hk_612.c): a call faults inside a function whose
     name carries the row's name and address, which the release exe's filter logs (recomp/release/mfl_trap.c). */
  static void *const d3d9dev2Std[28] = {
    (void *)stub_return_zero, (void *)LAB_004da2a0, (void *)LAB_004dac70, (void *)LAB_004d9420, (void *)LAB_004dad50, (void *)_rwD3D9RGBToPixel, (void *)_rwD3D9PixelToRGB,
    (void *)_rwD3D9RasterSetFromImage, (void *)_rwD3D9ImageGetFromRaster, (void *)_rwD3D9RasterDestroy, (void *)_rwD3D9RasterCreate, (void *)LAB_004ef150, (void *)LAB_004e3280,
    (void *)_rwD3D9RasterLock, (void *)_rwD3D9RasterUnlock, (void *)LAB_004e02d0, (void *)LAB_004e0360, (void *)LAB_004e13a0, (void *)LAB_004e0db0, (void *)LAB_004e03f0,
    (void *)LAB_004e0d90, (void *)LAB_004e0d70, (void *)LAB_004e13e0, (void *)LAB_004e3290, (void *)_rwD3D9NativeTextureGetSize, (void *)mfl_trap_rwD3D9NativeTextureWrite_004ed400, (void *)LAB_004ed780,
    (void *)LAB_004e3200};
  static void *d3d9dev2Imp[8];      /* GetWindowRect, GetClientRect, IsIconic, GetWindowLongA, SetWindowLongA, GetMenu, AdjustWindowRectEx, SetWindowPos */
  static void *const d3d9dev2Table[113] = {
    &g_d3d9_device, &NumVertexDeclarations, &MaxVertexDeclarations, &VertexDeclarations, &d3d9dev2Imp[0], &g_rw_947760, &WindowHandle, Present,
    &_RwD3D9RasterExtOffset, _rwD3D9PixelFormatInfo, (void *)RwRasterCreate, (void *)RwRasterDestroy, &SystemStarted, &DisplayModes, &NumDisplayModes, (void *)FUN_005dc42b,
    &_rwD3D9CPUSupportsSSE, LastMatrixUsed, &MatrixFreeList, (void *)RwFreeListDestroy, &g_rw_7d269c, &g_rw_7d26a0, (void *)_rwD3D9RasterClose,
    (void *)_rwD3D9VertexBufferManagerClose, (void *)_rwD3D9Im2DRenderClose, (void *)D3D9DeviceReleaseVideoMemory, &_RwDirect3DObject, &_RwD3DAdapterIndex, &g_rw_9475f0,
    &g_rw_9475f4, &_RwD3D9RenderSurface, &_RwD3D9DepthStencilSurface, (void *)_rxD3D9VideoMemoryRasterListRestore, (void *)_rwD3D9DynamicVertexBufferRestore,
    (void *)_rwD3D9RenderStateReset, (void *)_rwD3D9Im2DRenderOpen, (void *)_rwD3D9Im3DRenderOpen, &D3D9RestoreDeviceCallback, &InsideScene, &g_d3d9createdisplaymodeslist_9475e0,
    BackBufferFormats, (void *)d3d9dev2Std,
    /* 42.. */ &_rwD3D9LastFVFUsed, LastVertexStreamUsed, &LastWorldMatrixUsedIdentity, &g_rw_7d2688, IdentityMatrix, CurrentRenderSurface, &CurrentDepthStencilSurface,
    &D3D9ReleaseDeviceCallback, (void *)_rwD3D9Im3DRenderClose, (void *)_rwD3D9DynamicVertexBufferRelease, (void *)_rxD3D9VideoMemoryRasterListRelease,
    (void *)RwFreeListPurgeAllFreeLists, (void *)RwFreeListCreate, (void *)RwFreeListPurge, &g_data_947774, (void *)rwD3D9DynamicVertexBufferManagerForceDiscard,
    /* 58.. */ &_RwD3DAdapterType, &SelectedMultisamplingLevels, &SelectedMultisamplingLevelsNonMask, &g_data_7d226c, &_RwD3D9ZBufferDepth, &_RwHasStencilBuffer,
    &FullScreenRefreshRateInHz,
    /* 65.. */ &d3d9dev2Imp[1], &d3d9dev2Imp[2], &d3d9dev2Imp[3], &d3d9dev2Imp[4], &d3d9dev2Imp[5],
    &d3d9dev2Imp[6], &d3d9dev2Imp[7], (void *)RwD3D9CameraAttachWindow, &g_rx_9475e4, &g_rx_9475e8, &StencilClearValue, &g_rw_9476f4, g_data_5ea0d0,
    /* 78.. */ (void *)FUN_005dc432, (void *)RwFrameGetLTM, (void *)RwMatrixInvert, (void *)RwD3D9SetTransform, (void *)RwD3D9SetRenderState, _RwD3D9D3D9ViewTransform,
    _RwD3D9D3D9ProjTransform, &dgGGlobals, &_RwD3D9ActiveViewProjTransform, &NeedToCopyFromBackBuffer,
    /* 88.. */ &VideoMemoryRasters, &VideoMemoryRastersFreeList, &PaletteIndexFree, &NumPaletteIndexFree, &MaxPaletteIndexFree, &MaxPaletteIndex, &PaletteFreeList,
    (void *)rwD3D9RasterRenderGeneric, (void *)D3D9RasterClearGeneric, &_RwD3D9CurrentRasterTarget, (void *)RwErrorSet, (void *)FUN_0053eac0,
    /* 100.. */ (void *)rwD3D9RasterCopyQuad, (void *)rwD3D9RasterRenderQuad, s_SRC__DST_Raster_render_combinati_0062e880,
    /* 103.. */ (void *)RwImageCreate, (void *)RwImageAllocatePixels, (void *)_rwD3D9ImageGetFromRaster, (void *)_rwD3D9CheckValidCameraTextureFormat,
    (void *)_rwD3D9CheckValidCameraCubeTextureFormat, (void *)_rwD3D9RasterDestroy, (void *)_rwD3D9RasterCreate, (void *)_rwD3D9RasterSetFromImage, (void *)RwImageDestroy,
    &g_rw_947768};
  MflrwD3d9dev2Bind b = mflrw ? (MflrwD3d9dev2Bind)GetProcAddress(mflrw, "mflrw_d3d9dev2_bind") : 0;
  d3d9dev2Imp[0] = (void *)GetWindowRect; d3d9dev2Imp[1] = (void *)GetClientRect; d3d9dev2Imp[2] = (void *)IsIconic; d3d9dev2Imp[3] = (void *)GetWindowLongA;
  d3d9dev2Imp[4] = (void *)SetWindowLongA; d3d9dev2Imp[5] = (void *)GetMenu; d3d9dev2Imp[6] = (void *)AdjustWindowRectEx; d3d9dev2Imp[7] = (void *)SetWindowPos;
  if (b != 0) b(&RwEngineInstance, d3d9dev2Table);      /* the DLL copies the table */
}
#endif
