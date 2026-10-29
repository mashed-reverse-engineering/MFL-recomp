/* compile: /G5 */
#include "mfl_tier2.h"
/*0094808c*/ extern void *g_rt_94808c;
/*004e92b0*/ extern void __cdecl RwFreeListDestroy(void *fl);
/* Module close: destroys the 2D object free list. */
void rt2dObjectClose(void)
{
  RwFreeListDestroy(g_rt_94808c);
  g_rt_94808c = 0;
}
