/* rwa_free_005bf120 (0x005bf120): RWA memory: free through the RenderWare engine's memory functions (RwFree) */
#include "mfl_tier2.h"

/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
void rwa_free_005bf120(void *block)
{
  RwFree(block);
}
