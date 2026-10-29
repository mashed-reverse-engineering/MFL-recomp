/* compile: /G5 */
#include "mfl_tier2.h"
/*007da6f0*/ extern int g_rp_7da6f0;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* TRUE when the world sector has collision data. */
int RpCollisionWorldSectorQueryData(void *o)
{
  return FLD(o, int, g_rp_7da6f0) != 0;
}
