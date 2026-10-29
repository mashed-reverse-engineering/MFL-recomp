/* compile: /G5 */
#include "mfl_tier2.h"
/*007d9e18*/ extern int g_rt_7d9e18;
/*00947ac0*/ extern void *g_data_947ac0;
/*004e92b0*/ extern void __cdecl RwFreeListDestroy(void *fl);
/* Module close: clears the animation global, destroys the animation free list; returns its argument. */
void *AnimClose_53dea0(void *p)
{
  g_rt_7d9e18 = 0;
  if (g_data_947ac0) { RwFreeListDestroy(g_data_947ac0); g_data_947ac0 = 0; }
  return p;
}
