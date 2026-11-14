/* mflrw.dll d3d9nat binding (librw-mfl/capi/d3d9nat.cpp; LIB_RW group d3d9nat, engine switch window 8 lane K4): _rpD3D9UsageFlagPluginAttach, _rpGeometryNativeSize,
   _rpWorldSectorNativeSize and RpBuildMeshGenerateDefaultTriStrip work on the two usage-flag plugin offsets and these exe callees (table order = d3d9nat.cpp enum V,
   11 slots).  The plugin callbacks (LAB_00509510 / LAB_00509520 engine ctor / dtor, LAB_0054d840 object ctor / dtor, LAB_005094f0 copy) stay exe code and are passed
   by address.  TriStripMeshGenerate is the original address (the link turns it into the DLL thunk when LIB_RW lists tristrip).  No Win32 import slot, no slot filled
   at run time, no 0 slot, no trap row. */
#ifndef MFL_DIFFEMU
extern int _rpD3D9GeometryUsageFlagOffset, _rpD3D9WorldSectorUsageFlagOffset;
extern void __cdecl RwEngineRegisterPlugin(void), RpGeometryRegisterPlugin(void), RpWorldSectorRegisterPlugin(void), LAB_00509510(void), LAB_00509520(void),
  LAB_0054d840(void), LAB_005094f0(void), RpWorldSectorGetWorld(void), TriStripMeshGenerate(void);
typedef void (__cdecl *MflrwD3d9natBind)(void *const *table);
static void hk_bind_d3d9nat(int mflrw)
{
  static void *const d3d9natTable[11] = {
    (void *)RwEngineRegisterPlugin, (void *)RpGeometryRegisterPlugin, (void *)RpWorldSectorRegisterPlugin, (void *)LAB_00509510, (void *)LAB_00509520,
    (void *)LAB_0054d840, (void *)LAB_005094f0, &_rpD3D9GeometryUsageFlagOffset, &_rpD3D9WorldSectorUsageFlagOffset, (void *)RpWorldSectorGetWorld,
    (void *)TriStripMeshGenerate};
  MflrwD3d9natBind b = mflrw ? (MflrwD3d9natBind)GetProcAddress(mflrw, "mflrw_d3d9nat_bind") : 0;
  if (b != 0) b(d3d9natTable);      /* the DLL copies the table */
}
#endif
