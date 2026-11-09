/* mflrw.dll d3d9texdic binding (librw-mfl/capi/d3d9texdic.cpp, LIB_RW group d3d9texdic, engine switch window 3 D6): the native texture reader / stream size work on
   the exe's device, raster extension offset, device caps, stream, rasters and textures through these exe callees (table order = d3d9texdic.cpp enum B; RwStreamRead,
   the raster lock / palette functions, _rwD3D9CheckValidTextureFormat, RwRasterCreate / Destroy, _rwD3D9CheckAutoMipmapGenTextureFormat and RwD3D9RasterCreate are
   declared above).  A missing or refused bind is fatal when the group is linked, optional when it is not. */
#ifndef MFL_DIFFEMU
/*004ed780*/ extern unsigned char rwD3D9NativeTextureRead[];      /* a jmp [__imp_] thunk (ff 25) when LIB_RW lists d3d9texdic */
/*004dc720*/ extern void __cdecl RwStreamFindChunk(void);
/*004e3860*/ extern void __cdecl _rwD3D9CheckAutoMipmapGenCubeTextureFormat(void);
/*004d5b90*/ extern void __cdecl RwTextureCreate(void);
/*004d5c70*/ extern void __cdecl RwTextureSetName(void);
/*004d5ce0*/ extern void __cdecl RwTextureSetMaskName(void);
static void hk_bind_d3d9texdic(int mflrw)
{
  static void *const d3d9texdicTable[18] = {
    &g_d3d9_device, &_RwD3D9RasterExtOffset, _RwD3D9DeviceCaps, (void *)RwStreamFindChunk, (void *)RwStreamRead, (void *)_rwD3D9CheckValidTextureFormat,
    (void *)RwRasterCreate, (void *)_rwD3D9CheckAutoMipmapGenTextureFormat, (void *)_rwD3D9CheckAutoMipmapGenCubeTextureFormat, (void *)RwD3D9RasterCreate,
    (void *)RwRasterLockPalette, (void *)RwRasterUnlockPalette, (void *)RwRasterDestroy, (void *)RwRasterLock, (void *)RwRasterUnlock,
    (void *)RwTextureCreate, (void *)RwTextureSetName, (void *)RwTextureSetMaskName};
  MflrwD3dim2dBind b = mflrw ? (MflrwD3dim2dBind)GetProcAddress(mflrw, "mflrw_d3d9texdic_bind") : 0;
  int linked = rwD3D9NativeTextureRead[0] == 0xff && rwD3D9NativeTextureRead[1] == 0x25;
  int ok = b != 0 && b(18, d3d9texdicTable);
  if (linked && !ok) {
    MessageBoxA(0, "mflrw.dll has no usable mflrw_d3d9texdic_bind, but this exe takes the D3D9 native texture reader (LIB_RW group d3d9texdic) from it.", "MFL", 0x10);
    ExitProcess(3);
  }
}
#endif
