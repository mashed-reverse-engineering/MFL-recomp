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
