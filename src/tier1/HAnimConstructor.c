/* compile: /G5 */
#include "mfl_tier2.h"
/*00947ac8*/ extern int g_rp_947ac8;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Plugin constructor: hierarchy pointer = NULL, id = -1; returns the object. */
void *HAnimConstructor(void *o)
{
  char *e = (char *)o + g_rp_947ac8;
  FLD(e, int, 4) = 0;
  FLD(e, int, 0) = -1;
  return o;
}
