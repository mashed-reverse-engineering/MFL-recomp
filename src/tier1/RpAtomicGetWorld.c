/* compile: /G5 */
#include "mfl_tier2.h"
/*007d52a0*/ extern int g_rp_7d52a0;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Plugin data of an atomic at the world plugin offset (the world the atomic is in). */
void *RpAtomicGetWorld(void *o)
{
  return FLD(o, void *, g_rp_7d52a0);
}
