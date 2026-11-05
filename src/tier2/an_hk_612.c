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
typedef void (__cdecl *MflrwExeBind)(char **engine, int *rasterModule, int *textureModule, void *rasterTK, void *textureTK, void *texDictTK, void *errorSet);

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
#endif

  /* Step 1: core RenderWare init with the default resource arena. */
  stub_return_zero(stub_buf);
  memFuncs = (RwMemoryFunctions *)stub_return_zero(stub_buf);
  log_print("Calling RwEngineInit\n");
  resArenaSize = DEFAULT_RES_ARENA_SIZE;
  initFlags = stub_return_zero();
  ok = RwEngineInit(memFuncs,initFlags,resArenaSize);
  if (ok != 0) {
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


