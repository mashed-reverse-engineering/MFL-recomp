/* mflrw.dll d3d9vsu binding (librw-mfl/capi/d3d9vsu.cpp; LIB_RW group d3d9vsu, engine switch window 7 lane K3): _rwD3D9VSGetNormalInLocalSpace,
   _rwD3D9VSGetPointInLocalSpace and _rwD3D9VSGetRadiusInLocalSpace work on the exe's active transform and its two inverse caches (D3D9ActiveTransform 0x7d7a30 ..
   0x7d7a38), the two inverse buffers 0x62ff90 / 0x62ffd0, the identity marker 0x5ec460 and these exe callees (table order = d3d9vsu.cpp enum U, 12 slots).
   No Win32 import slot, no slot filled at run time, no 0 slot, no trap row (none of the callees is in tools/delink/rw_trap.txt).
   thunk_FUN_004fbc66 / thunk_FUN_004fbc3c = the D3DX Vec3TransformNormal / Vec3TransformCoord thunks (stdcall).  RwMatrixInvert and _rwInvSqrt are declared in
   an_hk_612.c (real prototypes; redeclaring them as void(void) is error C2040 / C2371). */
#ifndef MFL_DIFFEMU
extern int D3D9ActiveTransform, D3D9ActiveInverseTransform, D3D9ActiveInverseNormalizedTransform;
extern char D3D9InverseTransform[], D3D9InverseNormalizedTransform[], D3D9IdentityMatrix[];
extern void __cdecl D3D9MatrixInvertOrthoNormalized(void), RwMatrixOrthoNormalize(void), thunk_FUN_004fbc66(void), thunk_FUN_004fbc3c(void);
typedef void (__cdecl *MflrwD3d9vsuBind)(void *const *table);
static void hk_bind_d3d9vsu(int mflrw)
{
  static void *const d3d9vsuTable[12] = {
    &D3D9ActiveTransform, &D3D9ActiveInverseTransform, &D3D9ActiveInverseNormalizedTransform, D3D9InverseTransform, D3D9InverseNormalizedTransform,
    D3D9IdentityMatrix, (void *)D3D9MatrixInvertOrthoNormalized, (void *)RwMatrixInvert, (void *)RwMatrixOrthoNormalize, (void *)thunk_FUN_004fbc66,
    (void *)thunk_FUN_004fbc3c, (void *)_rwInvSqrt};
  MflrwD3d9vsuBind b = mflrw ? (MflrwD3d9vsuBind)GetProcAddress(mflrw, "mflrw_d3d9vsu_bind") : 0;
  if (b != 0) b(d3d9vsuTable);      /* the DLL copies the table */
}
#endif
