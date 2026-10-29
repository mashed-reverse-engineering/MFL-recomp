/* compile: /G5 */
#include "mfl_tier2.h"
/*007da72c*/ extern int g_rp_7da72c;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Plugin constructor: clears the slot at the MatFX offset, returns the object. */
void *lib_54ffd0(void *o)
{
  FLD(o, int, g_rp_7da72c) = 0;
  return o;
}
