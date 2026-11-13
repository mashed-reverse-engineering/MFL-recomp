/* RenderWare cpuext (Criterion Software), MFL.exe 0x004ed0b0-0x004ed252: the four CPU feature queries D3D9System asks at device
 * open (rwCPUHaveMMX / SSE / SSE2 / 3DNow).  Written from the MFL.exe disassembly (objdump).
 *
 * The originals are inline assembly: an EFLAGS.ID toggle test (written with 16-bit pushfw / popfw, so the test can never fail and the
 * "no cpuid" branch is dead), then cpuid leaf 1 (or 0x80000000 / 0x80000001 for 3DNow) and one feature bit of edx:
 *   MMX = edx bit 23, SSE = edx bit 25 OR 26 (SSE or SSE2), SSE2 = edx bit 26, 3DNow = extended edx bit 31.
 * Port policy: one CPU class (the D3DX decision, recomp/d3dx_dispatch.c): an SSE2 host without 3DNow (every x86-64 CPU has
 * MMX / SSE / SSE2; current AMD and all Intel parts report no 3DNow).  The answers are therefore constants; no inline assembly. */
#include "mfl_tier2.h"

/* TIER2: unvalidated 2026-11-12 authority=diffemu */
RwBool _rwCPUHaveMMX(void)
{
  return 1;
}

/* TIER2: unvalidated 2026-11-12 authority=diffemu */
RwBool _rwCPUHaveSSE(void)
{
  return 1;
}

/* TIER2: unvalidated 2026-11-12 authority=diffemu */
RwBool _rwCPUHaveSSE2(void)
{
  return 1;
}

/* TIER2: unvalidated 2026-11-12 authority=diffemu */
RwBool _rwCPUHave3DNow(void)
{
  return 0;
}
