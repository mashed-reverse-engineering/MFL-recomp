/* compile: /G5 */
#include "mfl_tier2.h"
/*007da730*/ extern int g_rp_7da730;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Plugin constructor: clears the slot at the second MatFX offset, returns the object. */
void *lib_5500b0(void *o)
{
  FLD(o, int, g_rp_7da730) = 0;
  return o;
}
