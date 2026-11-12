/* mflrw.dll noded3d9 binding (librw-mfl/capi/rwnoded3d9.cpp; LIB_RW group noded3d9, engine switch window 5 lane K batch 6): the world sector / atomic all-in-one nodes, their
   pipeline init functions, lighting callbacks, the vertex element sort compare and the atomic reinstance callback work on the exe's engine, resources module offset, ambient
   sum and these exe callees (table order = rwnoded3d9.cpp enum E, 19 slots).  No Win32 import slot.
   Include after an_hk_612_d3d9dev2.inc (RwFrameGetLTM, RwD3D9SetRenderState, RwD3D9GetRenderState are declared by an_hk_612.c / the earlier .inc files).  Slot 3 (FUN_004ff980 =
   D3D9AtomicDefaultInstanceCallback) and slot 0 (D3D9WorldSectorDefaultInstanceCallback) are still original code; slots 1, 4 and 5 are rows of this group (the original
   addresses, which the link turns into the DLL thunks).  D3D9AtomicDefaultReinstanceCallback needs the alias of rows_fnsizes_unanalysed.txt first.
   NOT compiled in this lane: declarations below that clash with an earlier one in an_hk_612.c (type or prototype) are to be dropped, the scratch compile tells. */
#ifndef MFL_DIFFEMU
extern int resourcesModule;
extern float AmbientSaturated[];
extern void __cdecl D3D9WorldSectorDefaultInstanceCallback(void), D3D9WorldSectorDefaultLightingCallback(void), rxD3D9DefaultRenderCallback(void), FUN_004ff980(void);
extern void __cdecl D3D9AtomicDefaultReinstanceCallback(void), _rwD3D9AtomicDefaultLightingCallback(void), rwD3D9LightsGlobalEnable(void), _rwD3D9LightLocalEnable(void);
extern void __cdecl _rwD3D9LightsEnable(void), RwD3D9SetTransformWorld(void), _rwD3D9LightDirectionalEnable(void);      /* RwResourcesFreeResEntry, rxD3D9Instance: an_hk_612_d3d9pipe.inc */
extern void __cdecl RpAtomicGetWorldBoundingSphere(void);
typedef void (__cdecl *MflrwNoded3d9Bind)(char **engine, void *const *table);
static void hk_bind_noded3d9(int mflrw)
{
  static void *const noded3d9Table[19] = {
    (void *)D3D9WorldSectorDefaultInstanceCallback, (void *)D3D9WorldSectorDefaultLightingCallback, (void *)rxD3D9DefaultRenderCallback, (void *)FUN_004ff980,
    (void *)D3D9AtomicDefaultReinstanceCallback, (void *)_rwD3D9AtomicDefaultLightingCallback,
    /* 6.. */ (void *)rwD3D9LightsGlobalEnable, (void *)_rwD3D9LightLocalEnable, (void *)_rwD3D9LightsEnable, (void *)RwResourcesFreeResEntry, (void *)rxD3D9Instance,
    &resourcesModule, (void *)RwD3D9SetTransformWorld, (void *)RwD3D9SetRenderState,
    /* 14.. */ AmbientSaturated, (void *)_rwD3D9LightDirectionalEnable, (void *)RwFrameGetLTM, (void *)RpAtomicGetWorldBoundingSphere, (void *)RwD3D9GetRenderState};
  MflrwNoded3d9Bind b = mflrw ? (MflrwNoded3d9Bind)GetProcAddress(mflrw, "mflrw_noded3d9_bind") : 0;
  if (b != 0) b(&RwEngineInstance, noded3d9Table);      /* the DLL copies the table */
}
#endif
