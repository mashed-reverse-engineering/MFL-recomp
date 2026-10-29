/* compile: /G5 */
#include "mfl_tier2.h"
/*007da6ec*/ extern int g_rp_7da6ec;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Atomic plugin constructor: clears the collision slot, returns the object. */
void *CollisionAtomicInit(void *o)
{
  FLD(o, int, g_rp_7da6ec) = 0;
  return o;
}
