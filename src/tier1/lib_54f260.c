/* compile: /G5 */
#include "mfl_tier2.h"
/*007da71c*/ extern int g_rp_7da71c;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Plugin constructor: clears the MatFX slot at +g_rp_7da71c, returns the object. */
void *lib_54f260(void *o)
{
  FLD(o, int, g_rp_7da71c) = 0;
  return o;
}
