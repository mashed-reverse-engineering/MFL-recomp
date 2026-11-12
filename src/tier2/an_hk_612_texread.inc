/* mflrw.dll texread / texread2 bindings (librw-mfl/capi/rwtexread.cpp, rwtexread2.cpp; LIB_RW groups texread, texread2, engine switch window 5 lane K): the image path
   search, image / texture / texture dictionary stream readers, texture module accessors, rtpitexd readers, gamma, BMP writer, plugin open / close and mipmap helpers
   work on the exe's engine, image module (imageModule), texture module (0x7d2184), free lists and these exe callees (table orders = rwtexread.cpp enum and
   rwtexread2.cpp enum B2).  Functions already declared above (stream, image create / destroy, raster, texture create / name, free list destroy) are not repeated.
   RwPalQuant*, BMPWriteRow and rtpitexdImage2TextureReadLegacy are trap rows (rw_trap.txt): their slots stay 0 so that no kept code names a trapped function
   (trapcheck); a call through 0 faults, as the trap did (precedent: the d3dconv table).  The default texture callbacks are passed by their original addresses
   (LAB_ names / the aliases.txt name TextureDefaultMipmapName_004d62d0), as the original plugin open stores them, also when the texread rows replace those functions. */
#ifndef MFL_DIFFEMU
/*004b2c30*/ extern double __cdecl _pow(double x, double y);      /* the CRT function (as cl_hud_pickup_icons_update_render.c) */
extern void __cdecl _rwpathisabsolute(void), FUN_00561060(void), RwImageMakeMask(void), RwImageApplyMask(void), _rwPluginRegistryAddPlugin(void),
  _rwStringStreamFindAndRead(void), _rwPluginRegistrySkipDataChunks(void), _rwPluginRegistryReadDataChunks(void), _rwDeviceSystemRequest(void),
  RwTexDictionaryCreate(void), RwTexDictionaryAddTexture(void), RwTexDictionaryForAllTextures(void), RwTexDictionaryDestroy_004d5ac0(void), FUN_004df790(void);
extern void __cdecl RwImageStreamRead(void), RwImageGammaCorrect(void), RwImageFindRasterFormat(void), RwRasterGetNumLevels(void), RwRasterSetFromImage(void),
  RwTextureSetRaster(void), RwStreamWrite(void), RwStreamWriteInt32(void), FreeListCreate_004e92a0(void), RwImageFindFileType(void), RwImageReadMaskedImage(void),
  RwImageCopy(void), RwImageResample(void), PalettizeMipmaps(void), RwImageSetFromRaster(void), TextureDefaultMipmapRead(void);
extern char LAB_004ddfa0[];      /* RwImageFindFileType's per-file callback (no function of its own) */
extern char LAB_004d7620[], LAB_004d6340[], LAB_004d7390[], TextureDefaultMipmapName_004d62d0[];      /* TextureDefaultFind / Read / BuildMipmaps / MipmapName, original addresses */
extern int g_data_7d2728, g_data_7d21b0, g_data_7d2188, g_data_7d21dc, g_data_7d218c, g_data_7d2160, _rwRasterFreeList, _rwImageFreeList, _rwImageFormatFreeList;
extern char g_data_62e700[], _rwRasterFreeListBlockSize[], _rwImageFreeListBlockSize[], g_qh_7d1f9c[];      /* g_qh_7d1f9c: an empty string */
typedef void (__cdecl *MflrwTexreadBind)(char **engine, int *imageModule, int *textureModule, void *const *table);
typedef void (__cdecl *MflrwTexread2Bind)(char **engine, int *textureModule, void *const *table);
static void hk_bind_texread(int mflrw)
{
  static void *const texreadTable[24] = {
    (void *)RwErrorSet, (void *)_rwpathisabsolute, (void *)FUN_00561060, LAB_004ddfa0, (void *)RwImageMakeMask, (void *)RwImageApplyMask,
    (void *)RwStreamFindChunk, (void *)RwStreamRead, (void *)RwStreamSkip, (void *)RwImageAllocatePixels, imageTKList, (void *)_pow,
    (void *)_rwPluginRegistryAddPlugin, g_rw_62e6d0, (void *)_rwStringStreamFindAndRead, (void *)_rwPluginRegistrySkipDataChunks,
    (void *)_rwPluginRegistryReadDataChunks, (void *)_rwDeviceSystemRequest, (void *)RwTexDictionaryCreate, (void *)RwTexDictionaryAddTexture,
    (void *)RwTexDictionaryForAllTextures, (void *)RwTexDictionaryDestroy_004d5ac0, (void *)FUN_004df790, g_rw_62e6e8};
  static void *const texread2Table[71] = {
    (void *)RwErrorSet, (void *)RwStreamFindChunk, (void *)RwStreamRead, (void *)_rwPluginRegistrySkipDataChunks, (void *)_rwStringStreamFindAndRead, g_rw_62e6d0,
    (void *)RwTextureCreate, (void *)RwTextureSetName, (void *)RwTextureSetMaskName, (void *)RwTexDictionaryCreate, 0 /* rtpitexdImage2TextureReadLegacy (trap) */,
    (void *)RwImageStreamRead, (void *)RwImageGammaCorrect, (void *)RwImageFindRasterFormat, (void *)RwRasterCreate, (void *)RwRasterDestroy,
    (void *)RwRasterGetNumLevels, (void *)RwImageCreateResample, (void *)RwImageDestroy, (void *)RwRasterLock, (void *)RwRasterSetFromImage, (void *)RwRasterUnlock,
    (void *)RwTextureSetRaster, (void *)_rwPluginRegistryReadDataChunks, (void *)RwTexDictionaryAddTexture, (void *)_pow, &imageModule,
    (void *)RwStreamOpen, (void *)RwStreamWrite, (void *)RwStreamWriteInt32, 0 /* BMPWriteRow (trap) */, (void *)RwStreamClose,
    &g_data_7d2728, &g_data_7d21b0, &g_data_7d2188, (void *)RwFreeListDestroy, (void *)RwTextureDestroy, (void *)_rwPluginRegistryDeInitObject,
    g_rw_62e6e8, (void *)FreeListCreate_004e92a0, g_rw_62e718, imageTKList, g_data_62e700, _rwRasterFreeListBlockSize,
    _rwImageFreeListBlockSize, &g_data_7d218c, &g_data_7d2160, &_rwRasterFreeList, &_rwImageFreeList,
    &_rwImageFormatFreeList, &g_data_7d21dc, &g_rw_7d21d8, LAB_004d7620, LAB_004d6340, LAB_004d7390,
    TextureDefaultMipmapName_004d62d0,
    /* 56.. : palette quantizer (trap rows: 0), image helpers, the mipmap functions' callees */
    0, 0, 0, 0, 0, (void *)RwImageCreate,
    (void *)RwImageAllocatePixels, (void *)RwImageFindFileType, (void *)RwImageReadMaskedImage, (void *)RwImageCopy, (void *)RwImageResample,
    (void *)PalettizeMipmaps, (void *)RwImageSetFromRaster, (void *)TextureDefaultMipmapRead, g_qh_7d1f9c};
  MflrwTexreadBind b1 = mflrw ? (MflrwTexreadBind)GetProcAddress(mflrw, "mflrw_texread_bind") : 0;
  MflrwTexread2Bind b2 = mflrw ? (MflrwTexread2Bind)GetProcAddress(mflrw, "mflrw_texread2_bind") : 0;
  if (b1 != 0) b1(&RwEngineInstance, &imageModule, &g_rw_7d2184, texreadTable);      /* the DLL copies the tables */
  if (b2 != 0) b2(&RwEngineInstance, &g_rw_7d2184, texread2Table);
}
#endif
