/* compile: /G5 */
#include "mfl_tier2.h"
/*007d54dc*/ extern int g_rp_7d54dc;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Value stored at the second D3D9 usage-flag plugin offset of an object. */
int FUN_00509580(void *o)
{
  return FLD(o, int, g_rp_7d54dc);
}
