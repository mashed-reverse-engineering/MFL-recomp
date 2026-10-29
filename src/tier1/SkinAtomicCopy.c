/* compile: /G5 */
#include "mfl_tier2.h"
/*007da644*/ extern int g_rp_7da644;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Atomic plugin copy: copies the skin pointer, returns the destination. */
void *SkinAtomicCopy(void *d, void *s)
{
  FLD(d, int, g_rp_7da644) = FLD(s, int, g_rp_7da644);
  return d;
}
