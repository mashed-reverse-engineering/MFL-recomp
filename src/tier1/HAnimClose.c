/* compile: /G5 */
#include "mfl_tier2.h"
/*00947acc*/ extern int g_rp_947acc;
/*004e92b0*/ extern void __cdecl RwFreeListDestroy(void *fl);
/* Module close: destroys the hierarchy free list and clears the pointer; returns its argument. */
void *HAnimClose(void *p)
{
  if (g_rp_947acc) { RwFreeListDestroy((void *)g_rp_947acc); g_rp_947acc = 0; }
  return p;
}
