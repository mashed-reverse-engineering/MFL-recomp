/* compile: /G5 */
#include "mfl_tier2.h"
/*007da644*/ extern int g_rp_7da644;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Atomic plugin destructor: clears the skin pointer if set, returns the object. */
void *SkinAtomicDestructor(void *o)
{
  if (FLD(o, int, g_rp_7da644) != 0) FLD(o, int, g_rp_7da644) = 0;
  return o;
}
