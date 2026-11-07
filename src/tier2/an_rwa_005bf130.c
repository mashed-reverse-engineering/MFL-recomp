/* rwa_calloc_005bf130 (0x005bf130): RWA memory: zeroed array allocation through the RenderWare engine's memory functions (RwCalloc, memoryFuncs + 0x114) */
#include "mfl_tier2.h"

typedef void *(*RwCallocFunc)(size_t count, size_t size, RwUInt32 hint);

/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
void *rwa_calloc_005bf130(RwUInt32 count, RwUInt32 size, RwUInt32 hint)
{
  return RW_GLOBAL(0x114, RwCallocFunc)(count, size, hint);
}
