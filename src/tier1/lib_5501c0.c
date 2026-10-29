/* compile: /G5 */
#include "mfl_tier2.h"
/*007da72c*/ extern int g_rp_7da72c;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Value stored at the MatFX plugin offset of an object. */
int lib_5501c0(void *o)
{
  return FLD(o, int, g_rp_7da72c);
}
