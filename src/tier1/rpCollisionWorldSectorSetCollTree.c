/* compile: /G5 */
#include "mfl_tier2.h"
/*007da6f0*/ extern int g_rp_7da6f0;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stores the collision tree in the world sector plugin slot; returns the sector. */
void *rpCollisionWorldSectorSetCollTree(void *o, int v)
{
  FLD(o, int, g_rp_7da6f0) = v;
  return o;
}
