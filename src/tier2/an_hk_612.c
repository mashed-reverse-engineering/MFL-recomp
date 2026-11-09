#include "mfl_tier2.h"
extern char LAB_004a2b30[];
extern char LAB_004a2b40[];
/*00405e40*/ extern int stub_return_zero();
/*004a2a30*/ extern int rw_register_image_formats();
/*004a2a70*/ extern int rw_plugins_attach();
/*004a4320*/ extern int filesystem_register_native();
/*004a4330*/ extern int video_select_and_set_mode();
/*004a43c0*/ extern int video_get_device_pointer();
/*004a5530*/ extern undefined __cdecl log_print(char *p0);
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0, ...);
/*004d3060*/ extern struct RwVideoMode * RwEngineGetVideoModeInfo(struct RwVideoMode *p0, RwInt32 p1);
/*004d3090*/ extern RwInt32 __cdecl RwEngineGetCurrentVideoMode(void);
/*004d3140*/ extern RwBool __cdecl RwEngineStart(void);
/*004d31d0*/ extern RwBool __cdecl RwEngineClose(void);
/*004d3240*/ extern RwBool RwEngineOpen(struct RwEngineOpenParams *p0);
/*004d3400*/ extern RwBool RwEngineTerm();
/*004d3440*/ extern RwBool __cdecl RwEngineInit(struct RwMemoryFunctions *p0, RwUInt32 p1, RwUInt32 p2);
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*004da040*/ extern void RwD3D9EngineSetRefreshRate(RwUInt32 p0);
/*004da0e0*/ extern void FUN_004da0e0(undefined4 p0);
/*004dbda0*/ extern void FUN_004dbda0(void *p0, void *p1, undefined4 p2, void *p3, undefined4 p4, undefined4 p5, undefined4 p6, void *p7, undefined4 p8, undefined4 p9, undefined4 p10, void *p11, undefined4 p12);
/*004dbdb0*/ extern int FUN_004dbdb0(void);
/*004dbdc0*/ extern void FUN_004dbdc0();
/*004dbdd0*/ extern int FUN_004dbdd0(void);
/*0050b5e0*/ extern int nop_stub();
/*005609c0*/ extern void FUN_005609c0(void);
/*00568a90*/ extern int RtCharsetOpen();
/*0076e864*/ extern undefined4 g_rw_engine_76e864;
/*0076e868*/ extern undefined4 g_rw_engine_76e868;
/* mflrw.dll g3 binding (librw-mfl/capi/g3.cpp): the RW 3.7 raster / texture layer in the DLL works on these exe globals */
extern __declspec(dllimport) int __stdcall GetModuleHandleA(const char *name);
extern __declspec(dllimport) void *__stdcall GetProcAddress(int module, const char *name);
/*007d2128*/ extern char *RwEngineInstance;
/*007d21d8*/ extern int g_rw_7d21d8;
/*007d2184*/ extern int g_rw_7d2184;
/*0062e718*/ extern char g_rw_62e718[];
/*0062e6d0*/ extern char g_rw_62e6d0[];
/*0062e6e8*/ extern char g_rw_62e6e8[];
/*004ea160*/ extern void *__cdecl RwErrorSet(void *error);
/* mflrw.dll collis binding (librw-mfl/capi/collis.cpp): the collision plugin data offsets of world sectors / geometries */
/*007da6f4*/ extern int g_worldforalltriintsecsphere_7da6f4;
/*007da6f0*/ extern int _rpCollisionGeometryDataOffset;
typedef void (__cdecl *MflrwCollisBind)(int *sectorOffset, int *geometryOffset);
/* mflrw.dll splanim binding (librw-mfl/capi/splanim.cpp): the spline plugin's table block (rapGTable[11] and the 12 basis table pointers behind it) */
/*007da7bc*/ extern unsigned char rapGTable[];
typedef void (__cdecl *MflrwSplanimBind)(char **engine, void *splineTables, void *errorSet);
/* mflrw.dll hanimcam binding (librw-mfl/capi/hanimcam.cpp): RpHAnimKeyFrameApply (the apply callback RpHAnimHierarchyUpdateMatrices inlines) and the exe frame / vector functions it calls */
/*00542fe0*/ extern void __cdecl FUN_00542fe0(void);
/*004d0c30*/ extern int __cdecl RwFrameDirty(void *frame);
/*004d0ff0*/ extern void *__cdecl RwFrameGetLTM(void *frame);
/*004d0f70*/ extern void *__cdecl RwFrameUpdateObjects(void *frame);
/*004d5670*/ extern void *__cdecl RwV3dTransformPoints(void *pointsOut, const void *pointsIn, int numPoints, const void *matrix);
typedef void (__cdecl *MflrwHanimcamBind)(char **engine, void *keyFrameApply, void *frameDirty, void *frameGetLTM, void *frameUpdateObjects, void *v3dTransformPoints);
/* mflrw.dll rwmath binding (librw-mfl/capi/rwmath.cpp): the engine (dirty frame list, z buffer range) and RwBBoxCalculate for RpMorphTargetCalcBoundingSphere */
/*004d18d0*/ extern void *__cdecl RwBBoxCalculate(void *boundBox, const void *verts, int numVerts);
typedef void (__cdecl *MflrwRwmathBind)(char **engine, void *bboxCalculate);
/* mflrw.dll pixfmt binding (librw-mfl/capi/pixfmt.cpp): RwErrorSet for the unknown-format error */
typedef void (__cdecl *MflrwPixfmtBind)(void *errorSet);
/* mflrw.dll tristrip binding (librw-mfl/capi/rwtristrip.cpp): the engine, the mesh module offset and the exe functions the strip builders call */
/*004e90b0*/ extern void *__cdecl RwFreeListCreate(int entrySize, int entriesPerBlock, int alignment, int hint);
/*004e92b0*/ extern void __cdecl RwFreeListDestroy(void *freeList);
/*00500ba0*/ extern void *__cdecl _rpMeshHeaderCreate(unsigned int size);
/*00500fb0*/ extern void *__cdecl _rpMeshHeaderForAllMeshes(void *meshHeader, void *callBack, void *data);
/*005d38b0*/ extern void __cdecl qsort(void *base, unsigned int num, unsigned int width, int (__cdecl *compare)(const void *, const void *));
/*00947778*/ extern int g_tristripstriptris_947778;
typedef void (__cdecl *MflrwTristripBind)(char **engine, int *meshModule, void *flCreate, void *flDestroy, void *hdrCreate, void *forAll, void *qsortFn, void *errorSet);
/* mflrw.dll colltree binding (librw-mfl/capi/rwcolltree.cpp): the engine and the exe's bounding box functions for the collision tree builder (needs the tristrip lines above) */
/*004d1990*/ extern void *__cdecl RwBBoxInitialize(void *boundBox, const void *vertex);
/*004d19d0*/ extern void *__cdecl RwBBoxAddPoint(void *boundBox, const void *vertex);
typedef void (__cdecl *MflrwColltreeBind)(char **engine, void *bboxInit, void *bboxAdd, void *qsortFn, void *errorSet);
/* mflrw.dll image binding (librw-mfl/capi/rwimage.cpp): the engine, the image module (free list, gamma table), the image plugin registry and RwImageAllocatePixels */
/*007d2724*/ extern int imageModule;
/*0062e848*/ extern char imageTKList[];
/*004e9ce0*/ extern void *__cdecl _rwPluginRegistryInitObject(void *registry, void *object);
/*004e9d40*/ extern void *__cdecl _rwPluginRegistryDeInitObject(void *registry, void *object);
/*004dd610*/ extern void *__cdecl RwImageAllocatePixels(void *image);
typedef void (__cdecl *MflrwImageBind)(char **engine, int *imageModule, void *imageTK, void *initObject, void *deInitObject, void *allocPixels, void *errorSet);
/* mflrw.dll uvanim binding (librw-mfl/capi/rwuvanim.cpp): the UV anim material plugin offset, the two interpolator info blocks (their apply callbacks), the pivots and the matrix functions */
/*00947b58*/ extern int RpUVAnimMaterialGlobals;
/*0063a3c0*/ extern char _rpUVAnimLinearInterpolatorInfo[];
/*0063a3f0*/ extern char _rpUVAnimParamInterpolatorInfo[];
/*005f5d8c*/ extern char rpUVAnimPivot[];
/*005f5d98*/ extern char rpUVAnimInvPivot[];
/*004d4e70*/ extern void *__cdecl RwMatrixCreate(void);
/*004d4e40*/ extern int __cdecl RwMatrixDestroy(void *matrix);
/*004d3cc0*/ extern void *__cdecl RwMatrixUpdate(void *matrix);
/*004d4870*/ extern void *__cdecl RwMatrixTranslate(void *matrix, const void *translation, int combineOp);
/*004d43f0*/ extern void *__cdecl RwMatrixRotate(void *matrix, const void *axis, float angle, int combineOp);
/*004d49c0*/ extern void *__cdecl RwMatrixTransform(void *matrix, const void *transform, int combineOp);
/*00550860*/ extern void *__cdecl RpMatFXMaterialSetUVTransformMatrices(void *material, void *baseTransform, void *dualTransform);
typedef void (__cdecl *MflrwUvanimBind)(int *globals, void *linInfo, void *parInfo, void *pivot, void *invPivot, void *create, void *destroy, void *update,
                                        void *translate, void *rotate, void *transform, void *setUV);
/* mflrw.dll camsync binding (librw-mfl/capi/rwcamsync.cpp): the exe functions the camera sync callback calls (RwBBoxCalculate: rwmath lines above) */
/*004d3c00*/ extern void *__cdecl RwMatrixOptimize(void *matrix, const void *tolerance);
/*004d2110*/ extern void *__cdecl CameraBuildPerspClipPlanes(void *camera);
/*004d2bc0*/ extern void *__cdecl CameraBuildParallelViewMatrix(void *camera);
/*004d27b0*/ extern void *__cdecl CameraBuildParallelClipPlanes(void *camera);
typedef void (__cdecl *MflrwCamsyncBind)(void *optimize, void *perspClip, void *parView, void *parClip, void *bboxCalc);
/* mflrw.dll rwleaf binding (librw-mfl/capi/rwleaf.cpp): the exe's CRT qsort for the triangle list sort (qsort declared with the tristrip lines above) */
typedef void (__cdecl *MflrwRwleafBind)(void *qsortFn);
/* mflrw.dll rwleaf2 binding (librw-mfl/capi/rwleaf2.cpp): RwErrorSet (above), _rwSqrt, RwV3dLength and the PVS plugin offset */
/*004d5410*/ extern float __cdecl _rwSqrt(float x);
/*004d53a0*/ extern float __cdecl RwV3dLength(const void *v);
/*00947ae8*/ extern int g_pvscalculatesectorbounds_947ae8;
typedef void (__cdecl *MflrwRwleaf2Bind)(void *errorSet, void *sqrtFn, void *v3dLength, int *pvsOffset);
/* mflrw.dll splopen binding (librw-mfl/capi/rwsplopen.cpp): the engine (allocator), the spline plugin's open counter and rapGTable (splanim lines above), RwErrorSet */
/*007da7b8*/ extern int g_data_7da7b8;
typedef void (__cdecl *MflrwSplopenBind)(char **engine, int *openCount, void *rapGTable, void *errorSet);
/* mflrw.dll single heap (librw-mfl/capi/core.cpp): the DLL allocates through the exe's RW memory table (engine +0x108..+0x114, filled by RwEngineInit) */
/*004e9940*/ extern void *RwOsGetMemoryInterface(void);
typedef void (__cdecl *MflrwMemBind)(void *exeTable);
/* mflrw.dll imgread binding (librw-mfl/capi/rwimgread.cpp): the engine (allocator), the stream and image functions RtBMPImageRead calls, BMPImageSetSpan, RwErrorSet */
/*004dc370*/ extern void *__cdecl RwStreamOpen(int type, int access, const void *data);
/*004dbe70*/ extern unsigned int __cdecl RwStreamRead(void *stream, void *buffer, unsigned int length);
/*004dc190*/ extern void *__cdecl RwStreamSkip(void *stream, unsigned int offset);
/*004dc2a0*/ extern int __cdecl RwStreamClose(void *stream, void *data);
/*004dd550*/ extern void *__cdecl RwImageCreate(int width, int height, int depth);
/*004dd5b0*/ extern int __cdecl RwImageDestroy(void *image);
/*0053d6c0*/ extern int __cdecl BMPImageSetSpan(void *image, int y, const void *src, int bpp, int bgr);
typedef void (__cdecl *MflrwImgreadBind)(char **engine, void *open, void *read, void *skip, void *close, void *create, void *destroy, void *allocPixels,
                                         void *setSpan, void *errorSet);
/* mflrw.dll rt2d binding (librw-mfl/capi/rwrt2d.cpp): the engine (free list alloc / free, free), Rt2dGlobals from brushFreeList on, the SList functions that
   allocate, Rt2dPathLock, Rt2dBrushSetUV, RwTextureDestroy, _rwSqrt (above) / _rwInvSqrt, the matrix functions _rt2dCTMGet uses, RwFrameGetLTM (above) */
/*00948060*/ extern int g_rt_948060;
/*005d5e90*/ extern void *__cdecl _rwSListGetNewEntry(void *list, unsigned int hint);
/*005d6030*/ extern int __cdecl _rwSListDestroy(void *list);
/*00564510*/ extern void *__cdecl Rt2dPathLock(void *path);
/*00564d90*/ extern void *__cdecl Rt2dBrushSetUV(void *brush, const void *uv0, const void *uv1, const void *uv2, const void *uv3);
/*004d5bf0*/ extern int __cdecl RwTextureDestroy(void *texture);
/*004d5470*/ extern float __cdecl _rwInvSqrt(float x);
/*004d3cd0*/ extern void *__cdecl RwMatrixMultiply(void *dst, const void *a, const void *b);
/*004d46e0*/ extern void *__cdecl RwMatrixScale(void *matrix, const void *scale, int combineOp);
/*004d4490*/ extern void *__cdecl RwMatrixInvert(void *dst, const void *src);
typedef void (__cdecl *MflrwRt2dBind)(char **engine, void *globals, void *slistNewEntry, void *slistDestroy, void *pathLock, void *brushSetUV, void *textureDestroy,
                                      void *sqrtFn, void *invSqrtFn, void *matMultiply, void *matScale, void *matTranslate, void *matInvert, void *frameGetLTM);
/* mflrw.dll pngread binding (librw-mfl/capi/rwpngread.cpp): the engine (malloc / free), the stream and image functions RtPNGImageRead calls, RwErrorSet, the
   error code helper (_rwerror, identical-code folded at 0x53eac0); the stream / image ones are declared with the imgread lines above */
/*004dd6d0*/ extern void *__cdecl RwImageFreePixels(void *image);
/*0053eac0*/ extern int __cdecl FUN_0053eac0(int code);
typedef void (__cdecl *MflrwPngreadBind)(char **engine, void *open, void *read, void *close, void *create, void *allocPixels, void *freePixels, void *destroy,
                                         void *errorSet, void *errorCode);
/* mflrw.dll lod binding (librw-mfl/capi/rwlod.cpp): the engine (camera, free list), RpLODAtomicGlobals, the geometry reference functions, RpAtomicSetGeometry,
   RwFrameGetLTM (above), and the exe addresses of the default LOD selector and the render hook (stored into atomics) */
/*009488d0*/ extern int RpLODAtomicGlobals;
/*004f9050*/ extern void *__cdecl RpGeometryAddRef(void *geometry);
/*004f9060*/ extern int __cdecl RpGeometryDestroy(void *geometry);
/*004f69e0*/ extern void *__cdecl RpAtomicSetGeometry(void *atomic, void *geometry, unsigned int flags);
/*00568030*/ extern int __cdecl LODAtomicCallBackSelectLOD(void *atomic);
/*00568140*/ extern void *__cdecl LODAtomicCallBackRender(void *atomic);
typedef void (__cdecl *MflrwLodBind)(char **engine, void *globals, void *geomAddRef, void *geomDestroy, void *atomicSetGeometry, void *frameGetLTM, void *defaultSelect,
                                     void *renderHook);
/* mflrw.dll d3dconv binding (librw-mfl/capi/d3dconv.cpp): the callees and data of the image <-> raster conversion functions of d3d9convrt.obj and the exe's
   per-pixel converters, as one table in the order of d3dconv.cpp enum B (RwErrorSet, _rwerror, RwImageCreate / Destroy / AllocatePixels declared above) */
/*004d79f0*/ extern void __cdecl RwRasterLock(void);
/*004d7850*/ extern void __cdecl RwRasterLockPalette(void);
/*004d7790*/ extern void __cdecl RwRasterUnlock(void);
/*004d77b0*/ extern void __cdecl RwRasterUnlockPalette(void);
/*004e3660*/ extern void __cdecl _rwD3D9CheckValidTextureFormat(void);
/*004e1ad0*/ extern void __cdecl _rwD3D9CheckRasterFormat(void);
/*004eb400*/ extern void __cdecl RwImageCreateResample(void);
/*00947144*/ extern int _RwD3D9RasterExtOffset;
/*00947160*/ extern char _rwD3D9PixelFormatInfo[];
/*005ea0f0*/ extern char _rwRasterConvTable[];
/*009475f4*/ extern int g_rw_9475f4;
/*00947600*/ extern char _RwD3D9DeviceCaps[];
/*0062e9f4*/ extern char s_Conversion_from_rasters_to_4bit_i_0062e9f4[];
/*0062ea30*/ extern char s_Conversion_from_non_palettised_r_0062ea30[];
/*0062ea78*/ extern char s_Conversion_from_compressed_raste_0062ea78[];
/*0062eab0*/ extern char s_Conversion_to_compressed_rasters_0062eab0[];
extern void __cdecl D3D9ConvR8G8B8A8ToR5G5B5(void), D3D9ConvR8G8B8A8ToR5G6B5(void), D3D9ConvR8G8B8A8ToA1R5G5B5(void), D3D9ConvR8G8B8A8ToA4R4G4B4(void),
  D3D9ConvR8G8B8A8ToA8R3G3B2(void), D3D9ConvR8G8B8A8ToR4G4B4(void), D3D9ConvR8G8B8A8ToL6V5U5(void), D3D9ConvR8G8B8A8ToX8R8G8B8(void),
  D3D9ConvR8G8B8A8ToX8B8G8R8(void), D3D9ConvR8G8B8A8ToA8R8G8B8(void), D3D9ConvR8G8B8A8ToA8B8G8R8(void), D3D9ConvR8G8B8A8ToV8U8(void),
  D3D9ConvR8G8B8A8ToL8(void), D3D9ConvR8G8B8A8ToA8L8(void), D3D9ConvR8G8B8A8ToL16(void);      /* 0x4ef990..0x4efc80 */
extern void __cdecl D3D9ConvR5G5B5ToR8G8B8A8(void), D3D9ConvR5G6B5ToR8G8B8A8(void), D3D9ConvL6V5U5ToR8G8B8A8(void), D3D9ConvA1R5G5B5ToR8G8B8A8(void),
  D3D9ConvA4R4G4B4ToR8G8B8A8(void), D3D9ConvA8R3G3B2ToR8G8B8A8(void), D3D9ConvR3G3B2ToR8G8B8A8(void), D3D9ConvX4R4G4B4ToR8G8B8A8(void),
  D3D9ConvX8R8G8B8ToR8G8B8A8(void), D3D9ConvX8B8G8R8ToR8G8B8A8(void), D3D9ConvV8U8ToR8G8B8A8(void), D3D9ConvA8R8G8B8ToR8G8B8A8(void),
  D3D9ConvA8B8G8R8ToR8G8B8A8(void), D3D9ConvL8ToR8G8B8A8(void), D3D9ConvA8ToR8G8B8A8(void), D3D9ConvA4L4ToR8G8B8A8(void), D3D9ConvA8L8ToR8G8B8A8(void),
  D3D9ConvL16ToR8G8B8A8(void);                                                                   /* 0x4eec40..0x4eef40 */
typedef void (__cdecl *MflrwD3dconvBind)(int n, void *const *table);
/* mflrw.dll d3drs binding (librw-mfl/capi/d3d9rs.cpp): the D3D9 render-state module (d3d9rendst) works on the exe's device, its bss block, the caps,
   the raster extension offset, the ambient colour, 1/255, its rdata tables and the 4 rendst functions that are C in the exe */
extern __declspec(dllimport) int __stdcall MessageBoxA(int wnd, const char *text, const char *caption, unsigned int type);
extern __declspec(dllimport) void __stdcall ExitProcess(unsigned int code);
/*004e5be0*/ extern unsigned char FUN_004e5be0[];      /* rwD3D9RWSetRenderState: a jmp [__imp_] thunk (ff 25) when LIB_RW lists d3drs */
/*007d2240*/ extern int g_d3d9_device;
/*007d3258*/ extern int g_rw_7d3258[];      /* the rendst dirty list; its bss block starts 0xa48 bytes before (lib spans have no symbol in the overlay) */
/*00947600*/ extern char g_data_947600[];
/*00947144*/ extern int g_rw_947144;
/*005dff00*/ extern float k_f_0_003921569;
/*005ea148*/ extern int g_rw_5ea148[];
/*004e3c00*/ extern void __cdecl RwD3D9SetRenderState(int state, int value);
/*004e3cd0*/ extern void __cdecl RwD3D9SetSamplerState(int stage, int type, int value);
/*004e53a0*/ extern int __cdecl rwD3D9RenderStateSrcBlend(int v);
/*004e53f0*/ extern int __cdecl rwD3D9RenderStateDestBlend(int v);
typedef void (__cdecl *MflrwD3dBind)(void *deviceVar, void *rsBss, void *caps, void *rasExtOffset, void *ambient, void *k255, void *tables,
                                     void *setRenderState, void *setSamplerState, void *srcBlend, void *destBlend);
/* mflrw.dll d3dim2d binding (librw-mfl/capi/d3dim2d.cpp): the Im2D renderer and the vertex buffer manager work on the exe's device, caps, engine (free-list
   alloc / free), their bss block 0x7d51c8..0x7d522c, DefaultVBSize, the current camera, the primitive type table and these exe callees (table order = d3dim2d.cpp enum B) */
/*004ec9c0*/ extern unsigned char FUN_004ec9c0[];      /* _rwD3D9DynamicVertexBufferManagerForceDiscard: a jmp [__imp_] thunk (ff 25) when LIB_RW lists d3dim2d */
/*007d51c8*/ extern int UseOwnVertexBuffer;
/*007d521c*/ extern int DynamicVertexBufferManager[];
/*0062e9d0*/ extern int DefaultVBSize;
/*009473c0*/ extern int dgGGlobals;
/*005ea28c*/ extern int g_rw_5ea28c[];
extern void __cdecl RwRasterGetCurrentContext(void), _rwD3D9SetVertexDeclaration(void), FUN_004db1f0(void), FUN_004db230(void), RwD3D9SetStreamSource(void),
  FUN_004db400(void), RwD3D9CreateVertexDeclaration(void), RwD3D9DeleteVertexDeclaration(void), RwD3D9SetTextureStageState(void), _rwD3D9RenderStateFlushCache(void),
  _rwD3D9RWGetRasterStage(void);
typedef int (__cdecl *MflrwD3dim2dBind)(int n, void *const *table);
/* mflrw.dll d3dras binding (librw-mfl/capi/d3dras.cpp): the raster create / lock / destroy core of d3d9raster works on the exe's device, IDirect3D9 object, adapter,
   caps / display / depth formats, the pixel format tables, the d3d9raster bss (palette index stack, palette free list, video-memory raster list), its 3 error strings
   and these exe callees (table order = d3dras.cpp enum B; the last entry 0 = GetClientRect from the DLL's own user32 import) */
/*004e32c0*/ extern unsigned char RwD3D9RasterCreate[];      /* a jmp [__imp_] thunk (ff 25) when LIB_RW lists d3dras */
extern int _RwDirect3DObject, _RwD3DAdapterIndex, _RwD3DAdapterType, WindowHandle, _RwD3D9DepthStencilSurface, _RwD3D9RenderSurface;      /* 0x7d2234..0x7d2248, 0x62e748 */
extern int g_rw_9475f0, _RwD3D9ZBufferDepth, g_rw_947748, g_rw_947768;      /* adapter format, z-buffer depth, display format, depth-stencil format */
extern int NumPaletteIndexFree, MaxPaletteIndexFree, MaxPaletteIndex, PaletteIndexFree, PaletteFreeList, VideoMemoryRasters, VideoMemoryRastersFreeList;      /* 0x7d27f4.. */
extern char s_Camera_raster_is_too_big__0062e8b4[], s_Failed_to_create_texture__0062e8d0[], s_Palletized_texture_can_only_be_r_0062e8f0[];
extern void __cdecl RwRasterCreate(void), RwRasterDestroy(void), _rwD3D9CheckRasterSize(void), _rwD3D9CheckValidCameraTextureFormat(void),
  _rwD3D9CheckValidZBufferTextureFormat(void), _rwD3D9CheckAutoMipmapGenTextureFormat(void), _rwD3D9CheckAutoMipmapGenCameraTextureFormat(void),
  rwD3D9CreateCameraTextureRaster(void), rwD3D9CreateZBufferRaster(void), RwTextureRasterGenerateMipmaps(void), _rwD3D9RWSetRasterStage(void);
/* mflrw.dll d3dlights binding (librw-mfl/capi/d3dlights.cpp): the D3D9 world lights and the device light setters work on the exe's device, engine (current world,
   memory table), the d3d9lights bss block (addressed from D3D9LightOffset 0x7d54a0, the only symbol of the span), the ambient colour, MaxNumLights / LightsCache and
   these exe callees (table order = d3dlights.cpp enum B; RwFrameGetLTM and RwD3D9SetRenderState declared above) */
/*004db740*/ extern unsigned char RwD3D9SetLight[];      /* a jmp [__imp_] thunk (ff 25) when LIB_RW lists d3dlights */
/*007d54a0*/ extern int g_rw_7d54a0;
/*007d269c*/ extern int g_rw_7d269c;
/*007d26a0*/ extern int g_rw_7d26a0;
/*004dbca0*/ extern void __cdecl FUN_004dbca0(void);      /* RwD3D9GetCaps */
/*004f4a70*/ extern float __cdecl RpLightGetConeAngle(const void *light);
/* mflrw.dll d3dxform binding (librw-mfl/capi/d3dxform.cpp): the transform / stream / declaration caches of d3d9device work on the exe's device, engine (free-list
   alloc / free), these cache variables and RwFreeListCreate (declared above) / Purge (table order = d3dxform.cpp enum B) */
/*004db5d0*/ extern unsigned char RwD3D9GetTransform[];      /* a jmp [__imp_] thunk (ff 25) when LIB_RW lists d3dxform */
extern int _rwD3D9LastFVFUsed, _rwD3D9LastVertexDeclarationUsed, _rwD3D9LastVertexShaderUsed, _rwD3D9LastPixelShaderUsed, _rwD3D9LastIndexBufferUsed;      /* 0x62e760.. */
extern int LastVertexStreamUsed[], LastWorldMatrixUsedIdentity, LastMatrixUsed[], MatrixFreeList;      /* 0x7d21f0, 0x7d2284, 0x7d2288 (0x104), 0x7d2698 */
extern char IdentityMatrix[], g_rw_62e7f0[];      /* 0x5ea090, the world D3DMATRIX scratch 0x62e7f0 */
/*004e9510*/ extern void __cdecl RwFreeListPurge(void);
typedef void (__cdecl *MflrwExeBind)(char **engine, int *rasterModule, int *textureModule, void *rasterTK, void *textureTK, void *texDictTK, void *errorSet);
#include "an_hk_612_d3d9pipe.inc"
#include "an_hk_612_d3d9texdic.inc"
#include "an_hk_612_d3d9vdtan.inc"
#include "an_hk_612_d3dras2.inc"

#define DEFAULT_RES_ARENA_SIZE (0x200000)
#define VIDEO_MEMORY_BYTES (0x400000)
#define BYTES_PER_PIXEL (10)
#define REFRESH_RATE_HZ (60)

/* rw_engine_init */

/* "'Calling RwEngineInit', RtFSManager, plugins" */

/*
 * Brings up the RenderWare engine: core init, file system, plugins, display open,
 * video mode set, engine start. Then logs the active video mode and the free video memory.
 * unused_arg is never read (the caller's argument is ignored).
 * Returns 1 when the engine started, 0 on any failure. Failure paths after a successful
 * RwEngineInit shut the engine down again before returning.
 */
undefined4 __cdecl rw_engine_init(char *unused_arg)

{
  RwMemoryFunctions *memFuncs;
  int mflrw;
  MflrwExeBind bind;
  RwUInt32 initFlags;
  RwBool ok;
  int status;
  RwInt32 mode_raw_or_width;
  int frame_buffer_bytes;
  int free_video_bytes;
  int current_mode;
  undefined1 *lab_2b30_ptr;
  undefined4 zero_arg;
  undefined4 video_mem_bytes;
  char *mode_format;
  RwInt32 mode_height;
  RwVideoMode *mode_info_ptr;
  RwUInt32 resArenaSize;
  char *msg;
  RwEngineOpenParams open_params;
  undefined1 stub_buf [16];
  RwVideoMode mode_info;

  /* Step 0 (not in MFL.exe): when mflrw.dll is loaded, hand it the addresses of the RenderWare globals its raster / texture layer works on.
     No DLL (original-only builds) or an older DLL without the export: nothing happens. */
#ifndef MFL_DIFFEMU      /* diffemu-define: MFL_DIFFEMU (the verification object leaves this block out; it is not in MFL.exe) */
  mflrw = GetModuleHandleA("mflrw.dll");
  bind = mflrw ? (MflrwExeBind)GetProcAddress(mflrw, "mflrw_exe_bind") : 0;
  if (bind != 0) {
    bind(&RwEngineInstance, &g_rw_7d21d8, &g_rw_7d2184, g_rw_62e718, g_rw_62e6d0, g_rw_62e6e8, (void *)RwErrorSet);
  }
  {
    MflrwCollisBind cbind = mflrw ? (MflrwCollisBind)GetProcAddress(mflrw, "mflrw_collis_bind") : 0;
    if (cbind != 0) cbind(&g_worldforalltriintsecsphere_7da6f4, &_rpCollisionGeometryDataOffset);
  }
  {
    MflrwSplanimBind sbind = mflrw ? (MflrwSplanimBind)GetProcAddress(mflrw, "mflrw_splanim_bind") : 0;
    if (sbind != 0) sbind(&RwEngineInstance, rapGTable, (void *)RwErrorSet);
  }
  {
    MflrwHanimcamBind hbind = mflrw ? (MflrwHanimcamBind)GetProcAddress(mflrw, "mflrw_hanimcam_bind") : 0;
    if (hbind != 0) hbind(&RwEngineInstance, (void *)FUN_00542fe0, (void *)RwFrameDirty, (void *)RwFrameGetLTM, (void *)RwFrameUpdateObjects, (void *)RwV3dTransformPoints);
  }
  {
    MflrwRwmathBind mbind = mflrw ? (MflrwRwmathBind)GetProcAddress(mflrw, "mflrw_rwmath_bind") : 0;
    if (mbind != 0) mbind(&RwEngineInstance, (void *)RwBBoxCalculate);
  }
  {
    MflrwPixfmtBind pbind = mflrw ? (MflrwPixfmtBind)GetProcAddress(mflrw, "mflrw_pixfmt_bind") : 0;
    if (pbind != 0) pbind((void *)RwErrorSet);
  }
  {
    MflrwTristripBind tbind = mflrw ? (MflrwTristripBind)GetProcAddress(mflrw, "mflrw_tristrip_bind") : 0;
    if (tbind != 0) tbind(&RwEngineInstance, &g_tristripstriptris_947778, (void *)RwFreeListCreate, (void *)RwFreeListDestroy, (void *)_rpMeshHeaderCreate,
                          (void *)_rpMeshHeaderForAllMeshes, (void *)qsort, (void *)RwErrorSet);
  }
  {
    MflrwColltreeBind kbind = mflrw ? (MflrwColltreeBind)GetProcAddress(mflrw, "mflrw_colltree_bind") : 0;
    if (kbind != 0) kbind(&RwEngineInstance, (void *)RwBBoxInitialize, (void *)RwBBoxAddPoint, (void *)qsort, (void *)RwErrorSet);
  }
  {
    MflrwImageBind ibind = mflrw ? (MflrwImageBind)GetProcAddress(mflrw, "mflrw_image_bind") : 0;
    if (ibind != 0) ibind(&RwEngineInstance, &imageModule, (void *)imageTKList, (void *)_rwPluginRegistryInitObject, (void *)_rwPluginRegistryDeInitObject,
                          (void *)RwImageAllocatePixels, (void *)RwErrorSet);
  }
  {
    MflrwUvanimBind ubind = mflrw ? (MflrwUvanimBind)GetProcAddress(mflrw, "mflrw_uvanim_bind") : 0;
    if (ubind != 0) ubind(&RpUVAnimMaterialGlobals, _rpUVAnimLinearInterpolatorInfo, _rpUVAnimParamInterpolatorInfo, rpUVAnimPivot, rpUVAnimInvPivot,
                          (void *)RwMatrixCreate, (void *)RwMatrixDestroy, (void *)RwMatrixUpdate, (void *)RwMatrixTranslate, (void *)RwMatrixRotate,
                          (void *)RwMatrixTransform, (void *)RpMatFXMaterialSetUVTransformMatrices);
  }
  {
    MflrwCamsyncBind ybind = mflrw ? (MflrwCamsyncBind)GetProcAddress(mflrw, "mflrw_camsync_bind") : 0;
    if (ybind != 0) ybind((void *)RwMatrixOptimize, (void *)CameraBuildPerspClipPlanes, (void *)CameraBuildParallelViewMatrix, (void *)CameraBuildParallelClipPlanes,
                          (void *)RwBBoxCalculate);
  }
  {
    MflrwRwleafBind lbind = mflrw ? (MflrwRwleafBind)GetProcAddress(mflrw, "mflrw_rwleaf_bind") : 0;
    if (lbind != 0) lbind((void *)qsort);
  }
  {
    MflrwRwleaf2Bind l2bind = mflrw ? (MflrwRwleaf2Bind)GetProcAddress(mflrw, "mflrw_rwleaf2_bind") : 0;
    if (l2bind != 0) l2bind((void *)RwErrorSet, (void *)_rwSqrt, (void *)RwV3dLength, &g_pvscalculatesectorbounds_947ae8);
  }
  {
    MflrwSplopenBind obind = mflrw ? (MflrwSplopenBind)GetProcAddress(mflrw, "mflrw_splopen_bind") : 0;
    if (obind != 0) obind(&RwEngineInstance, &g_data_7da7b8, rapGTable, (void *)RwErrorSet);
  }
  {
    MflrwImgreadBind ibind2 = mflrw ? (MflrwImgreadBind)GetProcAddress(mflrw, "mflrw_imgread_bind") : 0;
    if (ibind2 != 0) ibind2(&RwEngineInstance, (void *)RwStreamOpen, (void *)RwStreamRead, (void *)RwStreamSkip, (void *)RwStreamClose, (void *)RwImageCreate,
                            (void *)RwImageDestroy, (void *)RwImageAllocatePixels, (void *)BMPImageSetSpan, (void *)RwErrorSet);
  }
  {
    MflrwRt2dBind rbind = mflrw ? (MflrwRt2dBind)GetProcAddress(mflrw, "mflrw_rt2d_bind") : 0;
    if (rbind != 0) rbind(&RwEngineInstance, &g_rt_948060, (void *)_rwSListGetNewEntry, (void *)_rwSListDestroy, (void *)Rt2dPathLock, (void *)Rt2dBrushSetUV,
                          (void *)RwTextureDestroy, (void *)_rwSqrt, (void *)_rwInvSqrt, (void *)RwMatrixMultiply, (void *)RwMatrixScale, (void *)RwMatrixTranslate,
                          (void *)RwMatrixInvert, (void *)RwFrameGetLTM);
  }
  {
    MflrwPngreadBind pbind2 = mflrw ? (MflrwPngreadBind)GetProcAddress(mflrw, "mflrw_pngread_bind") : 0;
    if (pbind2 != 0) pbind2(&RwEngineInstance, (void *)RwStreamOpen, (void *)RwStreamRead, (void *)RwStreamClose, (void *)RwImageCreate, (void *)RwImageAllocatePixels,
                            (void *)RwImageFreePixels, (void *)RwImageDestroy, (void *)RwErrorSet, (void *)FUN_0053eac0);
  }
  {
    MflrwLodBind lodbind = mflrw ? (MflrwLodBind)GetProcAddress(mflrw, "mflrw_lod_bind") : 0;
    if (lodbind != 0) lodbind(&RwEngineInstance, &RpLODAtomicGlobals, (void *)RpGeometryAddRef, (void *)RpGeometryDestroy, (void *)RpAtomicSetGeometry,
                              (void *)RwFrameGetLTM, (void *)LODAtomicCallBackSelectLOD, (void *)LODAtomicCallBackRender);
  }
  {
    static void *const d3dconvTable[59] = {
      (void *)RwErrorSet, (void *)FUN_0053eac0, (void *)RwRasterLock, (void *)RwRasterLockPalette, (void *)RwRasterUnlock, (void *)RwRasterUnlockPalette,
      (void *)_rwD3D9CheckValidTextureFormat, (void *)_rwD3D9CheckRasterFormat, (void *)RwImageCreateResample, (void *)RwImageCreate, (void *)RwImageAllocatePixels,
      (void *)RwImageDestroy,
      /* RwPalQuantInit/AddImage/ResolvePalette/MatchImage/Term are trap rows (rw_trap.txt: never reached in 172 coverage runs, no shipped asset needs them);
         the table keeps 0 so that no kept code names a trapped function (trapcheck). A call through 0 faults, as the trap did. */
      0, 0, 0, 0, 0,
      &_RwD3D9RasterExtOffset, _rwD3D9PixelFormatInfo, _rwRasterConvTable, &g_rw_9475f4, _RwD3D9DeviceCaps,
      s_Conversion_from_rasters_to_4bit_i_0062e9f4, s_Conversion_from_non_palettised_r_0062ea30, s_Conversion_from_compressed_raste_0062ea78,
      s_Conversion_to_compressed_rasters_0062eab0,
      (void *)D3D9ConvR8G8B8A8ToR5G5B5, (void *)D3D9ConvR8G8B8A8ToR5G6B5, (void *)D3D9ConvR8G8B8A8ToA1R5G5B5, (void *)D3D9ConvR8G8B8A8ToA4R4G4B4,
      (void *)D3D9ConvR8G8B8A8ToA8R3G3B2, (void *)D3D9ConvR8G8B8A8ToR4G4B4, (void *)D3D9ConvR8G8B8A8ToL6V5U5, (void *)D3D9ConvR8G8B8A8ToX8R8G8B8,
      (void *)D3D9ConvR8G8B8A8ToX8B8G8R8, (void *)D3D9ConvR8G8B8A8ToA8R8G8B8, (void *)D3D9ConvR8G8B8A8ToA8B8G8R8, (void *)D3D9ConvR8G8B8A8ToV8U8,
      (void *)D3D9ConvR8G8B8A8ToL8, (void *)D3D9ConvR8G8B8A8ToA8L8, (void *)D3D9ConvR8G8B8A8ToL16,
      (void *)D3D9ConvR5G5B5ToR8G8B8A8, (void *)D3D9ConvR5G6B5ToR8G8B8A8, (void *)D3D9ConvL6V5U5ToR8G8B8A8, (void *)D3D9ConvA1R5G5B5ToR8G8B8A8,
      (void *)D3D9ConvA4R4G4B4ToR8G8B8A8, (void *)D3D9ConvA8R3G3B2ToR8G8B8A8, (void *)D3D9ConvR3G3B2ToR8G8B8A8, (void *)D3D9ConvX4R4G4B4ToR8G8B8A8,
      (void *)D3D9ConvX8R8G8B8ToR8G8B8A8, (void *)D3D9ConvX8B8G8R8ToR8G8B8A8, (void *)D3D9ConvV8U8ToR8G8B8A8, (void *)D3D9ConvA8R8G8B8ToR8G8B8A8,
      (void *)D3D9ConvA8B8G8R8ToR8G8B8A8, (void *)D3D9ConvL8ToR8G8B8A8, (void *)D3D9ConvA8ToR8G8B8A8, (void *)D3D9ConvA4L4ToR8G8B8A8, (void *)D3D9ConvA8L8ToR8G8B8A8,
      (void *)D3D9ConvL16ToR8G8B8A8};
    MflrwD3dconvBind xbind = mflrw ? (MflrwD3dconvBind)GetProcAddress(mflrw, "mflrw_d3dconv_bind") : 0;
    if (xbind != 0) xbind(59, d3dconvTable);
  }
  {
    /* d3drs: the original rendst bytes are gone when the group is linked, so a missing bind is fatal there (no silent fallback); unlisted, it stays optional */
    MflrwD3dBind dbind = mflrw ? (MflrwD3dBind)GetProcAddress(mflrw, "mflrw_d3d_bind") : 0;
    if (dbind != 0) {
      dbind(&g_d3d9_device, (char *)g_rw_7d3258 - 0xa48, g_data_947600, &g_rw_947144, g_rw_62e718 + 0x20 /* ambient colour 0x62e738 */, &k_f_0_003921569, g_rw_5ea148,
            (void *)RwD3D9SetRenderState, (void *)RwD3D9SetSamplerState, (void *)rwD3D9RenderStateSrcBlend, (void *)rwD3D9RenderStateDestBlend);
    } else if (FUN_004e5be0[0] == 0xff && FUN_004e5be0[1] == 0x25) {
      MessageBoxA(0, "mflrw.dll has no mflrw_d3d_bind, but this exe takes the D3D9 render-state module (LIB_RW group d3drs) from it.", "MFL", 0x10);
      ExitProcess(3);
    }
  }
  {
    /* d3dim2d: as d3drs, a missing or refused bind is fatal when the group is linked (its original bytes are gone), optional when it is not.  The DLL addresses the
       bss block from UseOwnVertexBuffer, so the ring array must sit at its original distance (0x54) in this link's data layout. */
    static void *const d3dim2dTable[21] = {
      &g_d3d9_device, _RwD3D9DeviceCaps, &RwEngineInstance, &UseOwnVertexBuffer, &DefaultVBSize, &dgGGlobals, g_rw_5ea28c,
      (void *)RwRasterGetCurrentContext, (void *)_rwD3D9SetVertexDeclaration, (void *)FUN_004db1f0, (void *)FUN_004db230, (void *)RwD3D9SetStreamSource,
      (void *)FUN_004db400, (void *)RwD3D9CreateVertexDeclaration, (void *)RwD3D9DeleteVertexDeclaration, (void *)RwD3D9SetRenderState,
      (void *)RwD3D9SetTextureStageState, (void *)_rwD3D9RenderStateFlushCache, (void *)_rwD3D9RWGetRasterStage, (void *)RwFreeListCreate, (void *)RwFreeListDestroy};
    MflrwD3dim2dBind ibind3 = mflrw ? (MflrwD3dim2dBind)GetProcAddress(mflrw, "mflrw_d3dim2d_bind") : 0;
    int linked = FUN_004ec9c0[0] == 0xff && FUN_004ec9c0[1] == 0x25;
    int ok = (char *)DynamicVertexBufferManager - (char *)&UseOwnVertexBuffer == 0x54 && ibind3 != 0 && ibind3(21, d3dim2dTable);
    if (linked && !ok) {
      MessageBoxA(0, "mflrw.dll has no usable mflrw_d3dim2d_bind, but this exe takes the D3D9 Im2D renderer / vertex buffer manager (LIB_RW group d3dim2d) from it.", "MFL", 0x10);
      ExitProcess(3);
    }
  }
  {
    /* d3dras: as d3dim2d, a missing or refused bind is fatal when the group is linked, optional when it is not */
    static void *const d3drasTable[44] = {
      &g_d3d9_device, &_RwDirect3DObject, &_RwD3DAdapterIndex, &_RwD3DAdapterType, &WindowHandle, &_RwD3D9DepthStencilSurface, &_RwD3D9RenderSurface,
      &RwEngineInstance, &_RwD3D9RasterExtOffset, _RwD3D9DeviceCaps, &g_rw_9475f0, &g_rw_9475f4, &_RwD3D9ZBufferDepth, &g_rw_947748, &g_rw_947768,
      _rwD3D9PixelFormatInfo, _rwRasterConvTable, &NumPaletteIndexFree, &MaxPaletteIndexFree, &MaxPaletteIndex, &PaletteIndexFree, &PaletteFreeList,
      &VideoMemoryRasters, &VideoMemoryRastersFreeList,
      s_Camera_raster_is_too_big__0062e8b4, s_Failed_to_create_texture__0062e8d0, s_Palletized_texture_can_only_be_r_0062e8f0,
      (void *)RwErrorSet, (void *)FUN_0053eac0, (void *)RwFreeListCreate, (void *)RwRasterCreate, (void *)RwRasterDestroy, (void *)_rwD3D9CheckRasterSize,
      (void *)_rwD3D9CheckValidTextureFormat, (void *)_rwD3D9CheckValidCameraTextureFormat, (void *)_rwD3D9CheckValidZBufferTextureFormat,
      (void *)_rwD3D9CheckAutoMipmapGenTextureFormat, (void *)_rwD3D9CheckAutoMipmapGenCameraTextureFormat, (void *)rwD3D9CreateCameraTextureRaster,
      (void *)rwD3D9CreateZBufferRaster, (void *)RwTextureRasterGenerateMipmaps, (void *)_rwD3D9RWGetRasterStage, (void *)_rwD3D9RWSetRasterStage, 0};
    MflrwD3dim2dBind rbind3 = mflrw ? (MflrwD3dim2dBind)GetProcAddress(mflrw, "mflrw_d3dras_bind") : 0;
    int linked = RwD3D9RasterCreate[0] == 0xff && RwD3D9RasterCreate[1] == 0x25;
    int ok = rbind3 != 0 && rbind3(44, d3drasTable);
    if (linked && !ok) {
      MessageBoxA(0, "mflrw.dll has no usable mflrw_d3dras_bind, but this exe takes the D3D9 raster create / lock / destroy functions (LIB_RW group d3dras) from it.", "MFL", 0x10);
      ExitProcess(3);
    }
  }
  {
    /* d3dlights: as d3dim2d, a missing or refused bind is fatal when the group is linked, optional when it is not */
    static void *const d3dlightsTable[10] = {
      &g_d3d9_device, &RwEngineInstance, &g_rw_7d54a0, g_rw_62e718 + 0x20 /* ambient colour 0x62e738 */, &g_rw_7d269c, &g_rw_7d26a0,
      (void *)FUN_004dbca0, (void *)RwFrameGetLTM, (void *)RpLightGetConeAngle, (void *)RwD3D9SetRenderState};
    MflrwD3dim2dBind lbind3 = mflrw ? (MflrwD3dim2dBind)GetProcAddress(mflrw, "mflrw_d3dlights_bind") : 0;
    int linked = RwD3D9SetLight[0] == 0xff && RwD3D9SetLight[1] == 0x25;
    int ok = lbind3 != 0 && lbind3(10, d3dlightsTable);
    if (linked && !ok) {
      MessageBoxA(0, "mflrw.dll has no usable mflrw_d3dlights_bind, but this exe takes the D3D9 world lights / light setters (LIB_RW group d3dlights) from it.", "MFL", 0x10);
      ExitProcess(3);
    }
  }
  {
    /* d3dxform: as d3dim2d, a missing or refused bind is fatal when the group is linked, optional when it is not */
    static void *const d3dxformTable[15] = {
      &g_d3d9_device, &RwEngineInstance, &_rwD3D9LastFVFUsed, &_rwD3D9LastVertexDeclarationUsed, &_rwD3D9LastVertexShaderUsed, &_rwD3D9LastPixelShaderUsed,
      &_rwD3D9LastIndexBufferUsed, LastVertexStreamUsed, &LastWorldMatrixUsedIdentity, LastMatrixUsed, &MatrixFreeList, IdentityMatrix, g_rw_62e7f0,
      (void *)RwFreeListCreate, (void *)RwFreeListPurge};
    MflrwD3dim2dBind xbind3 = mflrw ? (MflrwD3dim2dBind)GetProcAddress(mflrw, "mflrw_d3dxform_bind") : 0;
    int linked = RwD3D9GetTransform[0] == 0xff && RwD3D9GetTransform[1] == 0x25;
    int ok = xbind3 != 0 && xbind3(15, d3dxformTable);
    if (linked && !ok) {
      MessageBoxA(0, "mflrw.dll has no usable mflrw_d3dxform_bind, but this exe takes the D3D9 transform / stream / declaration caches (LIB_RW group d3dxform) from it.", "MFL", 0x10);
      ExitProcess(3);
    }
  }
  hk_bind_d3d9pipe(mflrw);
  hk_bind_d3d9texdic(mflrw);
  hk_bind_d3d9vdtan(mflrw);
  hk_bind_d3dras2(mflrw);
#endif

  /* Step 1: core RenderWare init with the default resource arena. */
  stub_return_zero(stub_buf);
  memFuncs = (RwMemoryFunctions *)stub_return_zero(stub_buf);
  log_print("Calling RwEngineInit\n");
  resArenaSize = DEFAULT_RES_ARENA_SIZE;
  initFlags = stub_return_zero();
  ok = RwEngineInit(memFuncs,initFlags,resArenaSize);
  if (ok != 0) {
#ifndef MFL_DIFFEMU      /* not in MFL.exe: hand mflrw.dll the exe's memory table (one heap for DLL and exe, as in the original) */
    {
      MflrwMemBind mbind = mflrw ? (MflrwMemBind)GetProcAddress(mflrw, "mflrw_mem_bind") : 0;
      if (mbind != 0) mbind(RwOsGetMemoryInterface());
    }
#endif
    /* Step 2: file system manager, native file system, then plugins. */
    msg = "Calling RtFSManagerOpen\n";
    log_print("Calling RtFSManagerOpen\n");
    status = RtFSManagerOpen(0xffffffff,msg);
    if (status != 0) {
      log_print("Calling HardwareInstallFileSystem\n");
      filesystem_register_native();
      video_get_device_pointer(&open_params);
      log_print("Calling RenderwareAttachPlugins\n");
      rw_plugins_attach();
      /* Step 3: open the engine on the display device; on failure shut down and return 0. */
      log_print("Calling RwEngineOpen\n");
      ok = RwEngineOpen(&open_params);
      if (ok == 0) {
        FUN_005609c0();
        RwEngineTerm();
        return 0;
      }
      /* Step 4: pick and set the video mode; on failure close the engine and return 0. */
      log_print("Calling HardwareSetVideoMode\n");
      status = video_select_and_set_mode();
      if (status != 0) {
        /* Step 5: 60 Hz refresh rate, then start the engine. */
        FUN_004da0e0(1);
        RwD3D9EngineSetRefreshRate(REFRESH_RATE_HZ);
        log_print("Calling RwEngineStart\n");
        ok = RwEngineStart();
        if (ok != 0) {
          nop_stub();
          nop_stub();
          /* Current mode index is the low 16 bits of the returned word. */
          mode_raw_or_width = RwEngineGetCurrentVideoMode();
          current_mode = (int)(short)mode_raw_or_width;
          mode_info_ptr = &mode_info;
          /* status is reused here: its last value is passed on to FUN_004dbda0 below. */
          status = current_mode;
          RwEngineGetVideoModeInfo(mode_info_ptr,current_mode);
          mode_format = "\tUsing video mode %u : %ux%u\n";
          mode_raw_or_width = mode_info.width;
          mode_height = mode_info.height;
          FID_conflict__wprintf("\tUsing video mode %u : %ux%u\n");
          /* Free video memory = total minus width*height*10 bytes (per the log message).
             frame_buffer_bytes = width*height*12 (inferred). */
          free_video_bytes = mode_info.width * mode_info.height * -BYTES_PER_PIXEL + VIDEO_MEMORY_BYTES;
          frame_buffer_bytes = mode_info.width * mode_info.height * 0xc;
          video_mem_bytes = VIDEO_MEMORY_BYTES;
          msg =
          "At 10 bytes per pixel (16bit Z + 2*32bit FB) there is %d - %d = %d bytes of video memory free\n"
          ;
          FID_conflict__wprintf
                    (
                    "At 10 bytes per pixel (16bit Z + 2*32bit FB) there is %d - %d = %d bytes of video memory free\n"
                    );
          /* Post-start setup: image formats, charset, no current texture dictionary, then the
             hand-off of the mode info and memory figures to FUN_004dbda0 (inferred). */
          rw_register_image_formats();
          RtCharsetOpen();
          zero_arg = 0;
          RwTexDictionarySetCurrent((RwTexDictionary *)0x0);
          g_rw_engine_76e864 = FUN_004dbdd0();
          lab_2b30_ptr = &LAB_004a2b30;
          FUN_004dbdc0(0x4a2b30);
          g_rw_engine_76e868 = FUN_004dbdb0();
          FUN_004dbda0(&LAB_004a2b40,lab_2b30_ptr,zero_arg,msg,video_mem_bytes,frame_buffer_bytes,free_video_bytes,mode_format,current_mode,mode_raw_or_width,
                       mode_height,mode_info_ptr,status);
          return 1;
        }
      }
      FUN_005609c0();
      RwEngineClose();
      RwEngineTerm();
    }
  }
  return 0;
}


