/* compile: /G5 */
#include "mfl_tier2.h"
/*007da648*/ extern int g_rp_7da648;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Geometry plugin constructor: clears the skin pointer, returns the object. */
void *SkinGeometryConstructor(void *o)
{
  FLD(o, int, g_rp_7da648) = 0;
  return o;
}
