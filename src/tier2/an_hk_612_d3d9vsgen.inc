/* mflrw.dll d3d9vsgen binding (librw-mfl/capi/d3d9vsgen.cpp; LIB_RW group d3d9vsgen, engine switch window 8 lane K3b): rpD3D9GenerateVertexShader writes vertex
   shader assembly text into the exe's source buffer (g_data_7d68e0, 0x1000 B) through the engine's sprintf (RwEngineInstance + 0xc4), reads the constants-per-
   effect table, the version / max-constant globals and the device caps (RasterCaps at +0x24), then calls these exe functions: D3DXAssembleShader (D3DX code linked
   into MFL.exe, not an import) and RwD3D9CreateVertexShader.  Table order = d3d9vsgen.cpp enum V, 8 slots.  No Win32 import slot, no slot filled at run time, no
   0 slot, no trap row (vertex shader code is never trapped, rule (a)).  RwEngineInstance / _RwD3D9DeviceCaps: declared in an_hk_612.c; D3D9NumConstantsEffect /
   D3D9UseVersion / _rwD3D9VertexShaderMaxConstants: declared by an_hk_612_d3d9vs.inc (include this file after it). */
#ifndef MFL_DIFFEMU
extern char g_data_7d68e0[];
extern void __cdecl RwD3D9CreateVertexShader(void);
extern void __cdecl D3DXAssembleShader(void);      /* stdcall in reality; only the address is taken here (no @N decoration wanted) */
typedef void (__cdecl *MflrwD3d9vsgenBind)(void *const *table);
static void hk_bind_d3d9vsgen(int mflrw)
{
  static void *const d3d9vsgenTable[8] = {
    &RwEngineInstance, g_data_7d68e0, D3D9NumConstantsEffect, &D3D9UseVersion, &_rwD3D9VertexShaderMaxConstants, _RwD3D9DeviceCaps,
    (void *)D3DXAssembleShader, (void *)RwD3D9CreateVertexShader};
  MflrwD3d9vsgenBind b = mflrw ? (MflrwD3d9vsgenBind)GetProcAddress(mflrw, "mflrw_d3d9vsgen_bind") : 0;
  if (b != 0) b(d3d9vsgenTable);      /* the DLL copies the table */
}
#endif
