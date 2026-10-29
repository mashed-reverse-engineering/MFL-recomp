/* compile: /G5 */
#include "mfl_tier2.h"
/*007d5360*/ extern int g_rp_7d5360;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Plugin constructor: clears the three list link words of the light plugin slot; returns the object. */
void *lib_4f6770(void *o)
{
  char *e = (char *)o + g_rp_7d5360;
  FLD(e, int, 8) = 0;
  FLD(e, int, 4) = 0;
  FLD(e, int, 0) = 0;
  return o;
}
