/* mflrw.dll d3d9vdtan binding (librw-mfl/capi/d3d9vdtan.cpp, LIB_RW group d3d9vdtan, engine switch window 3 D6): the tangent instancer FUN_00508600 works on the exe's
   engine (RwMalloc / RwFree), its six rdata constants (0x5ec3e0 det eps, 0x5dd330 1.0, 0x5ec3d8 merge distance (a double), 0x5ec3d0 1/128, 0x5ec3d4 normal dot,
   0x5df368 tangent cosine 0.7: Ghidra named it as a string) and these exe callees (table order = d3d9vdtan.cpp enum B; RwV3dLength and _rwInvSqrt declared above).
   A missing or refused bind is fatal when the group is linked, optional when it is not. */
#ifndef MFL_DIFFEMU
/*00508600*/ extern unsigned char FUN_00508600[];      /* a jmp [__imp_] thunk (ff 25) when LIB_RW lists d3d9vdtan */
/*005063f0*/ extern void __cdecl _rpD3D9VertexDeclarationInstV3d(void);
extern char k_f_1_0en12[], float_1_0[], k_f_n1_0842022en19_5ec3d8[], k_f_0_0078125[], k_f_0_9999[], s_333_Lap_frames_005df368[];      /* 0x5ec3e0 .. 0x5df368 */
static void hk_bind_d3d9vdtan(int mflrw)
{
  static void *const d3d9vdtanTable[10] = {
    &RwEngineInstance, k_f_1_0en12, float_1_0, k_f_n1_0842022en19_5ec3d8, k_f_0_0078125, k_f_0_9999, s_333_Lap_frames_005df368,
    (void *)RwV3dLength, (void *)_rwInvSqrt, (void *)_rpD3D9VertexDeclarationInstV3d};
  MflrwD3dim2dBind b = mflrw ? (MflrwD3dim2dBind)GetProcAddress(mflrw, "mflrw_d3d9vdtan_bind") : 0;
  int linked = FUN_00508600[0] == 0xff && FUN_00508600[1] == 0x25;
  int ok = b != 0 && b(10, d3d9vdtanTable);
  if (linked && !ok) {
    MessageBoxA(0, "mflrw.dll has no usable mflrw_d3d9vdtan_bind, but this exe takes the D3D9 tangent instancer (LIB_RW group d3d9vdtan) from it.", "MFL", 0x10);
    ExitProcess(3);
  }
}
#endif
