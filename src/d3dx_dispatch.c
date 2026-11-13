/* d3dx_dispatch.c - D3DX math dispatch setup, D3DXCpuOptimizations (0x0050501a), port policy: one CPU class, the SSE2 table.
 *
 * D3DX keeps its math entry points in a table of 0x47 function pointers (0x62ec88).  Every public stub jumps through its slot;
 * the slot's static start value is an init thunk (`push 1; call D3DXCpuOptimizations; jmp [slot]`), so the first D3DX call
 * of the process fills the table.  The original picked one installer per CPU: registry value D3DXCpuOptimizations (GetD3DRegValue),
 * then 3DNow (IsProcessorFeaturePresent 7) -> x3d_D3DXInitFastTable, SSE2 (cpuid helper 0x504f75) -> sse2_D3DXInitFastTable,
 * SSE (IsProcessorFeaturePresent 6) -> sse_D3DXInitFastTable, always on top of x86_D3DXInitFastTable.
 * This port has one CPU-independent target, so it always takes what every SSE2 host already took: default table, then the SSE2
 * installer.  The x86 installer the original ran first writes only slots 0-7 and 34, which the SSE2 installer overwrites, so the
 * table, the class (2) and the return value are the same as the original's on an SSE2 host without 3DNow and without the registry
 * value (B2 2026-11-05: static table compare + cpuopt_check.py, both installers run for real, 70 cases EQUAL).
 * Dropped with it (int3 in the standalone link, tools/delink/rw_trap.txt): GetD3DRegValue, the cpuid helper, the x86 / x3d installers
 * and their math.  Not a tier 2 function: diffemu stubs the original's CPU probes, so it cannot be EQUAL by design. */

#define D3DX_SLOT_COUNT 0x47
#define D3DX_CPU_UNSET 0xffff      /* table not filled yet (static start value) / optimizations switched off */
#define D3DX_CPU_SSE2 2

typedef void (*D3DXMathFn)(void);

/*0062ec88*/ extern D3DXMathFn PTR_FUN_0062ec88[D3DX_SLOT_COUNT];                  /* the live dispatch table */
/*0062eda8*/ extern D3DXMathFn PTR_c_D3DXVec2Transform_0062eda8[D3DX_SLOT_COUNT];  /* defaults: the plain C math entries */
/*0062efc4*/ extern int g_d3dxcpuoptimizations_62efc4;                               /* selected CPU class */
/*0050c60e*/ extern void __cdecl sse2_D3DXInitFastTable(D3DXMathFn *table);

/* sse2_D3DXInitFastTable 0x0050c60e (lane S, window 6; diffemu EQUAL 200 cases 7f / 27f, HS 3895): the SSE2 installer.  Slot meanings from
 * the default C table 0x62eda8.  Only four slots have a live reader (public stubs `jmp [slot]` in live code): 3, 4, 6, 10, filled with the SSE
 * matrix / vector entry points that the pinned librw DLL replaces (tools/delink/rw_d3dx.txt).  Every other slot's readers are dead-filled /
 * trap rows, and so are the functions written into them: those words are written as constants (D3DX_CODE), so no installed C names a filled
 * body (trapcheck); the standalone link keeps original code at its original address, so the table is word for word the original's. */
#define D3DX_CODE(va) ((D3DXMathFn)(va))      /* address of original D3DX code that no live code calls (filled in the standalone link) */

/*00510b45*/ extern void __stdcall sse_D3DXMatrixMultiply(void);
/*0050f872*/ extern void __stdcall sse_D3DXMatrixMultiplyTranspose(void);
/*0050f5d9*/ extern void __stdcall sse_D3DXVec3TransformNormal(void);
/*00511050*/ extern void __stdcall sse_D3DXVec3TransformCoord(void);

/* Overwrites the SSE2 / SSE entries over the C defaults already in the table (slot = byte offset / 4). */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
void __cdecl sse2_D3DXInitFastTable(D3DXMathFn *table)
{
  /* live slots */
  table[3] = (D3DXMathFn)sse_D3DXMatrixMultiply;
  table[4] = (D3DXMathFn)sse_D3DXMatrixMultiplyTranspose;
  table[6] = (D3DXMathFn)sse_D3DXVec3TransformNormal;
  table[10] = (D3DXMathFn)sse_D3DXVec3TransformCoord;

  /* slots without a live reader */
  table[2] = D3DX_CODE(0x00510a79);   /* sse2 Vec4Transform (also slot 34) */
  table[34] = D3DX_CODE(0x00510a79);
  table[0] = D3DX_CODE(0x0050e956);   /* sse Vec2Transform */
  table[1] = D3DX_CODE(0x0050f6ae);
  table[5] = D3DX_CODE(0x0050e9eb);   /* sse Vec2TransformNormal */
  table[9] = D3DX_CODE(0x0050e994);   /* sse Vec2TransformCoord */
  table[7] = D3DX_CODE(0x0050f782);   /* sse Vec3Normalize */
  table[8] = D3DX_CODE(0x0050edde);   /* sse_D3DXVec4Normalize */
  table[21] = D3DX_CODE(0x00511153);
  table[16] = D3DX_CODE(0x00510c79);
  table[20] = D3DX_CODE(0x0050d139);  /* sse_D3DXQuaternionNormalize */
  table[19] = D3DX_CODE(0x0050d0c0);  /* sse_D3DXQuaternionMultiply */
  table[24] = D3DX_CODE(0x005114f1);
  table[25] = D3DX_CODE(0x0051142f);  /* sse2_D3DXQuaternionRotationYawPitchRoll */
  table[22] = D3DX_CODE(0x0050f31f);  /* sse_D3DXPlaneIntersectLine */
  table[18] = D3DX_CODE(0x0050ea22);
  table[49] = D3DX_CODE(0x0050f054);  /* sse_D3DXVec3Hermite */
  table[50] = D3DX_CODE(0x0050ee78);  /* sse_D3DXVec3CatmullRom */
  table[51] = D3DX_CODE(0x0050f227);  /* sse Vec3BaryCentric */
  table[52] = D3DX_CODE(0x0050f107);  /* sse_D3DXVec4Hermite */
  table[53] = D3DX_CODE(0x0050efaf);  /* sse_D3DXVec4CatmullRom */
  table[54] = D3DX_CODE(0x0050f284);  /* sse Vec4BaryCentric */
  table[17] = D3DX_CODE(0x0050f1a1);  /* sse_D3DXVec2Hermite */
  table[47] = D3DX_CODE(0x0050ef2a);  /* sse_D3DXVec2CatmullRom */
  table[48] = D3DX_CODE(0x0050f2cd);  /* sse Vec2BaryCentric */
  table[11] = D3DX_CODE(0x00510d39);
  table[27] = D3DX_CODE(0x00511268);  /* sse MatrixShadow */
  table[13] = D3DX_CODE(0x00510e44);
  table[28] = D3DX_CODE(0x00510f44);  /* sse MatrixReflect */
  table[58] = D3DX_CODE(0x005109a7);  /* sse2_D3DXFloat16To32Array */
  table[69] = D3DX_CODE(0x0050e200);  /* sse_D3DXPSGPUpdateSkinnedMesh */
  table[59] = D3DX_CODE(0x0050fe73);  /* sse2_D3DXVec2TransformArray */
  table[60] = D3DX_CODE(0x00510001);  /* sse2_D3DXVec2TransformCoordArray */
  table[61] = D3DX_CODE(0x00510167);  /* sse2_D3DXVec2TransformNormalArray */
  table[62] = D3DX_CODE(0x0050fc6d);  /* sse2_D3DXVec3TransformArray */
  table[63] = D3DX_CODE(0x00510685);  /* sse2_D3DXVec3TransformCoordArray */
  table[64] = D3DX_CODE(0x005104b5);  /* sse2_D3DXVec3TransformNormalArray */
  table[65] = D3DX_CODE(0x00510277);  /* sse2_D3DXVec4TransformArray (also slot 68) */
  table[68] = D3DX_CODE(0x00510277);
}

/* enable = 0: back to the plain C table.  Otherwise: fill the table once, return the CPU class. */
int __stdcall D3DXCpuOptimizations(int enable)
{
  int i;

  if (enable && g_d3dxcpuoptimizations_62efc4 != D3DX_CPU_UNSET)
    return g_d3dxcpuoptimizations_62efc4;      /* already set up */
  for (i = 0; i < D3DX_SLOT_COUNT; i++)
    PTR_FUN_0062ec88[i] = PTR_c_D3DXVec2Transform_0062eda8[i];
  if (enable) {
    sse2_D3DXInitFastTable(PTR_FUN_0062ec88);
    g_d3dxcpuoptimizations_62efc4 = D3DX_CPU_SSE2;
  } else {
    g_d3dxcpuoptimizations_62efc4 = D3DX_CPU_UNSET;
  }
  return g_d3dxcpuoptimizations_62efc4;
}
