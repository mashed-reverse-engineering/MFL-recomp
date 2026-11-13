/* mflrw.dll matfx binding (librw-mfl/capi/rwmatfx.cpp; LIB_RW group matfx, engine switch window 5 lane K): the rpmatfx user data arrays, bump map setup, D3D9 instance /
   render callbacks, default / env / bump map render, env map matrix and pipelines create work on the exe's engine, device, matfx / video card globals, shader data and
   these exe callees (table order = rwmatfx.cpp enum M, 80 slots; functions declared above are not repeated).  _rpMatFXD3D9AtomicMatFXRenderBlack is a trap row
   (rw_trap.txt): its slot is the trap stub mfl_trap__rpMatFXD3D9AtomicMatFXRenderBlack_00550aa0 (an_hk_612.c, W7 T17; trapcheck forbids the name).  The instance / render callbacks the pipelines store are passed by their
   original addresses (the aliases.txt names *_00553ee0 / *_00553f60, LAB_00553a90), as the original PipelinesCreate stores them, also when the matfx rows replace those functions. */
#ifndef MFL_DIFFEMU
extern int userDataGeometryOffset, userDataWorldSectorOffset, MatFXMaterialDataOffset, VideoCardSupportsTangents, D3D9DefaultInstanceCallbackAtomic,
  D3D9DefaultInstanceCallbackWorldSector, MatFXAtomicPipe, MatFXWorldSectorPipe, VideoCardMaxTextureBlendStages, VideoCardSupportsMultitexture,
  VideoCardSupportsMultiplyAdd, VideoCardSupportsModulate2X, VideoCardSupportsDiffuseInStage0, NeedToValidateModulate2X, NeedToValidateDiffuseInStage0,
  BumpMapPixelShader;      /* BumpMapPixelShader .. +0x14: the six shader slots */
extern char g_rp_63a2d0[], g_data_5f5d48[], dwBumpMapPixelShader[], dwEnvMapPixelShader[], dwEnvMapNoBaseTexturePixelShader[], dwBumpEnvMapPixelShader[],
  dwBumpModulatedEnvMapPixelShader[], dwBumpModulatedEnvMapNoAlphaPixelShader[], D3D9AtomicMatFXInstanceCallback_00553ee0[], D3D9WorldSectorMatFXInstanceCallback_00553f60[], LAB_00553a90[];
extern void __cdecl RpD3D9GeometryGetUsageFlags(void), FUN_00509530(void), FUN_00509580(void), FUN_00509560(void), RwD3D9RasterIsCompressed(void),
  _rpMatFXTextureMaskCreate(void), _rwD3D9EnableClippingIfNeeded(void), _rpMatFXD3D9AtomicMatFXBumpMapRender(void), _rpMatFXD3D9AtomicMatFXEnvRender(void),
  _rpMatFXD3D9AtomicMatFXDualPassRender(void), _rpMatFXD3D9AtomicMatFXDefaultRender(void), RwD3D9SetTransform(void), rwD3D9RasterIsCubeRaster(void),
  RxPipelineCreate(void), RxPipelineLock(void), FUN_005003b0(void), FUN_005009c0(void), RxLockedPipeAddFragment(void), RxLockedPipeUnlock(void),
  _rxPipelineDestroy_004deff0(void), RxPipelineFindNodeByName(void), RxD3D9AllInOneGetInstanceCallBack(void), RxD3D9AllInOneSetInstanceCallBack(void),
  RxD3D9AllInOneSetRenderCallBack(void), RwD3D9CreatePixelShader(void), RwD3D9DynamicVertexBufferLock(void), RwD3D9DynamicVertexBufferUnlock(void),
  CalculatePerturbedUVs(void), _rwD3D9RasterConvertToNonPalettized(void);
typedef void (__cdecl *MflrwMatfxBind)(char **engine, void *const *table);
static void hk_bind_matfx(int mflrw)
{
  static void *const matfxTable[80] = {
    &userDataGeometryOffset, &userDataWorldSectorOffset, &MatFXMaterialDataOffset, &VideoCardSupportsTangents,
    &D3D9DefaultInstanceCallbackAtomic, &D3D9DefaultInstanceCallbackWorldSector, (void *)RpD3D9GeometryGetUsageFlags, (void *)FUN_00509530, (void *)FUN_00509580,
    (void *)FUN_00509560, (void *)RwD3D9RasterIsCompressed, (void *)_rpMatFXTextureMaskCreate, (void *)_rwD3D9EnableClippingIfNeeded, (void *)RwD3D9GetRenderState,
    (void *)FUN_004e55d0, (void *)RwD3D9SetRenderState, (void *)RwD3D9SetTextureStageState, (void *)_rwD3D9SetStreams, (void *)_rwD3D9RenderStateVertexAlphaEnable,
    (void *)RwD3D9SetSurfaceProperties, (void *)_rpMatFXD3D9AtomicMatFXBumpMapRender, (void *)_rpMatFXD3D9AtomicMatFXEnvRender, (void *)_rpMatFXD3D9AtomicMatFXDualPassRender,
    (void *)_rpMatFXD3D9AtomicMatFXDefaultRender, (void *)mfl_trap__rpMatFXD3D9AtomicMatFXRenderBlack_00550aa0 /* trap row (W7 T17 stub) */, &_rwD3D9LastIndexBufferUsed, &_rwD3D9LastVertexDeclarationUsed,
    &_rwD3D9LastFVFUsed, &g_d3d9_device,
    /* 29.. */ (void *)RwD3D9SetTransform, (void *)_rwD3D9RenderStateFlushCache, &_rwD3D9LastPixelShaderUsed, &_rwD3D9LastVertexShaderUsed, g_rp_63a2d0,
    /* 34.. */ (void *)rwD3D9RasterIsCubeRaster, (void *)RwFrameGetLTM, (void *)RwMatrixInvert, (void *)RwMatrixMultiply, g_data_5f5d48,
    /* 39.. */ (void *)RxPipelineCreate, (void *)RxPipelineLock, (void *)FUN_005003b0, (void *)FUN_005009c0, (void *)RxLockedPipeAddFragment, (void *)RxLockedPipeUnlock,
    (void *)_rxPipelineDestroy_004deff0, (void *)RxPipelineFindNodeByName, (void *)RxD3D9AllInOneGetInstanceCallBack, (void *)RxD3D9AllInOneSetInstanceCallBack,
    (void *)RxD3D9AllInOneSetRenderCallBack, (void *)FUN_004dbca0, (void *)RwD3D9CreatePixelShader,
    /* 52.. */ &MatFXAtomicPipe, &MatFXWorldSectorPipe, &VideoCardMaxTextureBlendStages, &VideoCardSupportsMultitexture,
    &VideoCardSupportsMultiplyAdd, &VideoCardSupportsModulate2X, &VideoCardSupportsDiffuseInStage0, &NeedToValidateModulate2X,
    &NeedToValidateDiffuseInStage0, &BumpMapPixelShader, D3D9AtomicMatFXInstanceCallback_00553ee0, D3D9WorldSectorMatFXInstanceCallback_00553f60, LAB_00553a90,
    /* 65.. */ dwBumpMapPixelShader, dwEnvMapPixelShader, dwEnvMapNoBaseTexturePixelShader, dwBumpEnvMapPixelShader,
    dwBumpModulatedEnvMapPixelShader, dwBumpModulatedEnvMapNoAlphaPixelShader,
    /* 71.. */ (void *)_rwD3D9TextureHasAlpha, (void *)RwD3D9DynamicVertexBufferLock, (void *)RwD3D9DynamicVertexBufferUnlock, (void *)CalculatePerturbedUVs,
    (void *)RwD3D9SetStreamSource, (void *)FUN_004e5950, (void *)rwD3D9RenderStateSrcBlend, (void *)rwD3D9RenderStateDestBlend,
    /* 79 */ (void *)_rwD3D9RasterConvertToNonPalettized};
  MflrwMatfxBind b = mflrw ? (MflrwMatfxBind)GetProcAddress(mflrw, "mflrw_matfx_bind") : 0;
  if (b != 0) b(&RwEngineInstance, matfxTable);      /* the DLL copies the table */
}
#endif
