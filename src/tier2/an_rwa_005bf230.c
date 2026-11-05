/* rwa_aligned_free_005bf230 (0x005bf230): free a block of the RWA aligned allocator */
#include "mfl_tier2.h"

/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

/* The aligned allocator stores the pointer RwMalloc returned in the word in front of the aligned block. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_aligned_free_005bf230(void *block)
{
  RwFree(((void **)block)[-1]);
}
