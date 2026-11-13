/* mflrw.dll d3d9vs binding (librw-mfl/capi/d3d9vs.cpp; LIB_RW group d3d9vs, engine switch window 7 lane K3): _rpD3D9VertexShaderCacheOpen, _rpD3D9VertexShaderCachePurge,
   rpD3D9GetVertexShader and rxD3D9VertexShaderDefaultGetMaterialShaderCallBack work on the exe's vertex shader cache (VertexShaderCache 0x7d54e0, 256 x 0x14 B),
   its count / last-entry / version / max-constants globals, the constants-per-effect table (D3D9NumConstantsEffect 0x7d78e0), the engine (frame word at +8), the
   device caps and these exe callees (table order = d3d9vs.cpp enum V, 11 slots).  No Win32 import slot, no slot filled at run time, no 0 slot.
   rpD3D9GenerateVertexShader 0x509870 (the shader generator, K3b, deferred to W8) is NOT a trap row: it stays original exe code and is passed by its exe address
   (rule (a): vertex shader code is never trapped).  RwEngineInstance and _RwD3D9DeviceCaps are declared in an_hk_612.c (used by earlier tables). */
#ifndef MFL_DIFFEMU
extern char VertexShaderCache[], D3D9NumConstantsEffect[];
extern int NumShadersInCache, LastVertexShaderDesc, D3D9UseVersion, _rwD3D9VertexShaderMaxConstants;
extern void __cdecl rpD3D9GenerateVertexShader(void), RwD3D9DeletePixelShader(void), _memmove(void);
typedef void (__cdecl *MflrwD3d9vsBind)(void *const *table);
static void hk_bind_d3d9vs(int mflrw)
{
  static void *const d3d9vsTable[11] = {
    VertexShaderCache, D3D9NumConstantsEffect, &NumShadersInCache, &LastVertexShaderDesc, &D3D9UseVersion, &_rwD3D9VertexShaderMaxConstants,
    &RwEngineInstance, _RwD3D9DeviceCaps, (void *)rpD3D9GenerateVertexShader, (void *)RwD3D9DeletePixelShader, (void *)_memmove};
  MflrwD3d9vsBind b = mflrw ? (MflrwD3d9vsBind)GetProcAddress(mflrw, "mflrw_d3d9vs_bind") : 0;
  if (b != 0) b(d3d9vsTable);      /* the DLL copies the table */
}
#endif
