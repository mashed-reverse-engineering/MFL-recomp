/* Rwp physics library, MFL.exe 0x005850c0: decides once whether the solver may use its SSE code paths (g_rwp_simd_enabled).
 * Written from the MFL.exe disassembly (objdump).
 *
 * The original asks three things, each of the first and the last inside a __try / __except(1) guard:
 *   0x584fc0  cpuid leaf 0 -> the 12-byte vendor string; vendor must be "GenuineIntel" or "AuthenticAMD" (strncmp, 12 chars);
 *             cpuid leaf 1 inline: edx bit 25 (SSE) must be set;
 *   0x585050  executes `xorps xmm0, xmm0`: faults (-> 0) when the OS has not enabled SSE state.
 * Flag = 1 only when all three hold; a failing vendor probe stores 0 at once.
 * Port policy: one CPU class (the D3DX decision, recomp/d3dx_dispatch.c): an Intel or AMD SSE2 host with an SSE-enabled OS, so the
 * flag is always 1 and the two probes (0x584fc0, 0x585050, with their __except handlers) have no caller left (rows_trap.txt).
 * Not a tier 2 function (like D3DXCpuOptimizations): diffemu stubs the original's probe calls, so it cannot be EQUAL by design.
 * Evidence: agents/s_w6/cpuclass_check.py runs the original bytes for real (probes, strncmp, SEH frames): flag 1 on Intel / AMD SSE2,
 * 0 on the controls (other vendor, no SSE bit).
 * Known difference: on a CPU of another vendor with SSE (e.g. Hygon "HygonGenuine", VIA / Zhaoxin) the original stored 0 and fell back to
 * its non-SSE solver paths; the port stores 1 there too (same one-CPU-class decision as D3DX). */
#include "mfl_tier2.h"

/*007daa14*/ extern RwInt32 g_rwp_simd_enabled;

void rwp_simd_detect_005850c0(void)
{
  g_rwp_simd_enabled = 1;
}
