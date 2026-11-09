/* mflrw.dll d3dras2 binding (librw-mfl/capi/d3dras2.cpp, engine switch window 3 lane X): the rest of d3d9raster (format checks, CheckRasterFormat, camera-texture /
   z-buffer creators, mip levels, texture set raster, sub raster, video-memory list release / restore) works on the exe's device, IDirect3D9 object, adapter, caps,
   adapter / depth formats, z-buffer and display depths, the pixel format tables, the video-memory raster list, the present parameters (0x947740, 14 words), the
   previous mode size 0x9475e4 / 0x9475e8, its 3 error strings and these exe callees (table order = d3dras2.cpp enum B; the last entry 0 = GetClientRect from the
   DLL's own user32 import).  A missing or refused bind is fatal when the group is linked, optional when it is not. */
#ifndef MFL_DIFFEMU
/*004e1be0*/ extern unsigned char _rxD3D9VideoMemoryRasterListRestore[];      /* a jmp [__imp_] thunk (ff 25) when LIB_RW lists d3dras2 */
/*004e13f0*/ extern void __cdecl _rwD3D9FindCorrectRasterFormat(void);
extern int Present[], g_rx_9475e4, g_rx_9475e8;      /* D3DPRESENT_PARAMETERS 0x947740, the previous mode's width / height */
extern char s_rwRASTERTYPECAMERATEXTURE_can_no_0062e938[], s_Failed_to_create_raster__0062e96c[], s_Out_of_video_memory__Failed_to_c_0062e988[];
static void hk_bind_d3dras2(int mflrw)
{
  static void *const d3dras2Table[26] = {
    &g_d3d9_device, &_RwDirect3DObject, &_RwD3DAdapterIndex, &_RwD3DAdapterType, &WindowHandle, &_RwD3D9DepthStencilSurface, &_RwD3D9RasterExtOffset,
    _RwD3D9DeviceCaps, &g_rw_9475f0, &g_rw_9475f4, &_RwD3D9ZBufferDepth, &g_rw_947768, _rwD3D9PixelFormatInfo, _rwRasterConvTable, &VideoMemoryRasters,
    Present, &g_rx_9475e4, &g_rx_9475e8,
    s_rwRASTERTYPECAMERATEXTURE_can_no_0062e938, s_Failed_to_create_raster__0062e96c, s_Out_of_video_memory__Failed_to_c_0062e988,
    (void *)RwErrorSet, (void *)FUN_0053eac0, (void *)_rwD3D9CheckRasterSize, (void *)_rwD3D9FindCorrectRasterFormat, 0};
  MflrwD3dim2dBind b = mflrw ? (MflrwD3dim2dBind)GetProcAddress(mflrw, "mflrw_d3dras2_bind") : 0;
  int linked = _rxD3D9VideoMemoryRasterListRestore[0] == 0xff && _rxD3D9VideoMemoryRasterListRestore[1] == 0x25;
  int ok = b != 0 && b(26, d3dras2Table);
  if (linked && !ok) {
    MessageBoxA(0, "mflrw.dll has no usable mflrw_d3dras2_bind, but this exe takes the D3D9 raster format checks / creators / video-memory list (LIB_RW group d3dras2) from it.", "MFL", 0x10);
    ExitProcess(3);
  }
}
#endif
