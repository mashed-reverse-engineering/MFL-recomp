#include "mfl_tier2.h"
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;


/* Frees the entries buffer and the pool header itself using the engine memory allocator. */
void __cdecl pool_destroy(undefined4 *pool)
{
  /* Free allocated entries buffer */
  RwFree(pool[2]);
  pool[2] = 0;
  pool[1] = 0;
  *pool = 0;
  /* Free the pool header structure */
  RwFree(pool);
  return;
}
