/* compile: /G5 */
#include "mfl_tier2.h"
/*007da868*/ extern int g_rp_7da868;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Geometry plugin constructor: clears two patch fields; returns the object. */
void *rpPatchGeometryConstructor(void *o)
{
  char *e = (char *)o + g_rp_7da868;
  FLD(e, int, 0) = 0;
  FLD(e, int, 4) = 0;
  return o;
}
