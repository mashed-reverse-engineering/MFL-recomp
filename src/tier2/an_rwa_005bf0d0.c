/* rwa_malloc_005bf0d0 (0x005bf0d0): RWA memory: allocate through the RenderWare engine's memory functions (RwMalloc) */
#include "mfl_tier2.h"

/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
void *rwa_malloc_005bf0d0(RwUInt32 size, RwUInt32 hint)
{
  return RwMalloc(size, hint);
}
