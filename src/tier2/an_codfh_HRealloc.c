/* HRealloc (MFL.exe 0x004e97c0): RenderWare Graphics 3.7 (Criterion Software), the default (ANSI) RwRealloc, realloc ignoring the memory hint.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/bamemory.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build), not static (its address is stored by rwMemoryOpen).
 * Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

/*004b4729*/ extern void *realloc(void *mem, size_t newSize);

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *HRealloc(void *mem, size_t newSize, RwUInt32 hint)
{
    return realloc(mem, newSize);
}
