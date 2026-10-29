/* compile: /G5 */
#include "mfl_tier2.h"
/*007d54d8*/ extern int g_rp_7d54d8;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets the value stored at the first D3D9 usage-flag plugin offset of an object. */
void FUN_00509530(void *o, int v)
{
  FLD(o, int, g_rp_7d54d8) = v;
}
