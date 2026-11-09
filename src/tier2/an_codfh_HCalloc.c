/* HCalloc (MFL.exe 0x004e97e0): RenderWare Graphics 3.7 (Criterion Software), the default (ANSI) RwCalloc, calloc ignoring the memory hint.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/bamemory.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build), not static (its address is stored by rwMemoryOpen).
 * Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

/*004b466e*/ extern void *calloc(size_t numObj, size_t sizeObj);

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *HCalloc(size_t numObj, size_t sizeObj, RwUInt32 hint)
{
    return calloc(numObj, sizeObj);
}
