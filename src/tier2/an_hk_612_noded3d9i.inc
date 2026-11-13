/* mflrw.dll noded3d9i binding (librw-mfl/capi/noded3d9i.cpp; LIB_RW group noded3d9i, engine switch window 6 lane K1): the default instance callbacks of the world sector
   and atomic all-in-one nodes (D3D9WorldSectorDefaultInstanceCallback 0x5005b0, FUN_004ff980 = D3D9AtomicDefaultInstanceCallback) work on the exe's engine, device caps
   and these exe callees (table order = noded3d9i.cpp enum E, 17 slots).  No Win32 import slot, no trap row slot (none of the 17 addresses is in tools/delink/rw_trap.txt),
   no slot filled at run time: the table is const.
   Include after an_hk_612_noded3d9.inc.  Declared earlier and therefore not declared here: _RwD3D9DeviceCaps, qsort (an_hk_612.c), RwD3D9DynamicVertexBufferDestroy,
   _rpD3D9VertexDeclarationInstV3d, FUN_00508600, RpD3D9GeometryGetUsageFlags, FUN_00509580 (earlier .inc files).
   Compiled in the lane against a scratch copy of an_hk_612.c (agents/k1_w6/hk_scratch_compile.py: i686-w64-mingw32-gcc -fsyntax-only with tools/port/msvc_shim.h). */
#ifndef MFL_DIFFEMU
#define RpD3D9WorldSectorGetUsageFlags FUN_00509580            /* 0x509580 (sector): usage flags of a world sector, the sibling of RpD3D9GeometryGetUsageFlags */
#define rpD3D9VertexDeclarationInstTangent FUN_00508600        /* 0x508600: the tangent element writer */
#define D3D9VertexElementSortCB LAB_00500310                   /* 0x500310: qsort compare of two vertex elements (row of group noded3d9) */
extern void __cdecl RwD3D9CreateVertexDeclaration(void), RwD3D9CreateVertexBuffer(void), RwD3D9DestroyVertexBuffer(void), RwD3D9DynamicVertexBufferCreate(void);
extern void __cdecl _rpD3D9AddDynamicGeometry(void), LAB_00500310(void), rpD3D9VertexDeclarationInstV3dComp(void), _rpD3D9VertexDeclarationInstV3dMorph(void);
extern void __cdecl _rpD3D9VertexDeclarationInstV2d(void), _rpD3D9VertexDeclarationInstColor(void);
typedef void (__cdecl *MflrwNoded3d9iBind)(char **engine, void *const *table);
static void hk_bind_noded3d9i(int mflrw)
{
  static void *const noded3d9iTable[17] = {
    (void *)RpD3D9WorldSectorGetUsageFlags, (void *)RpD3D9GeometryGetUsageFlags, _RwD3D9DeviceCaps, (void *)RwD3D9CreateVertexDeclaration, (void *)RwD3D9CreateVertexBuffer,
    /* 5.. */ (void *)RwD3D9DestroyVertexBuffer, (void *)RwD3D9DynamicVertexBufferCreate, (void *)RwD3D9DynamicVertexBufferDestroy, (void *)_rpD3D9AddDynamicGeometry,
    /* 9.. */ (void *)qsort, (void *)D3D9VertexElementSortCB,
    /* 11.. */ (void *)_rpD3D9VertexDeclarationInstV3d, (void *)rpD3D9VertexDeclarationInstV3dComp, (void *)_rpD3D9VertexDeclarationInstV3dMorph,
    (void *)_rpD3D9VertexDeclarationInstV2d, (void *)_rpD3D9VertexDeclarationInstColor, (void *)rpD3D9VertexDeclarationInstTangent};
  MflrwNoded3d9iBind b = mflrw ? (MflrwNoded3d9iBind)GetProcAddress(mflrw, "mflrw_noded3d9i_bind") : 0;
  if (b != 0) b(&RwEngineInstance, noded3d9iTable);      /* the DLL copies the table */
}
#endif
