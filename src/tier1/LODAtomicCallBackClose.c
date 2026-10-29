/* compile: /G5 */
#include "mfl_tier2.h"
/*009488dc*/ extern int g_rp_9488dc;
/*004e92b0*/ extern void __cdecl RwFreeListDestroy(void *fl);
/* Module close: destroys the LOD callback free list and clears the pointer; returns its argument. */
void *LODAtomicCallBackClose(void *p)
{
  if (g_rp_9488dc) RwFreeListDestroy((void *)g_rp_9488dc);
  g_rp_9488dc = 0;
  return p;
}
