/* compile: /G5 */
#include "mfl_tier2.h"
/*007da6f0*/ extern int g_rp_7da6f0;
/*0053e3b0*/ extern void __cdecl RtAnimInterpolatorDestroy(void *p);
/* Frees the collision tree of a world sector and clears the plugin slot; returns the sector. */
void *RpCollisionWorldSectorDestroyData(void *s)
{
  void **slot = (void **)((char *)s + g_rp_7da6f0);
  if (*slot) { RtAnimInterpolatorDestroy(*slot); *slot = 0; }
  return s;
}
