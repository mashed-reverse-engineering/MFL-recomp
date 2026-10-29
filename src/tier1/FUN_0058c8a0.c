/* compile: /G5 */
#include "mfl_tier2.h"
/*007daa18*/ extern int g_data_7daa18;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Value stored at the DMorph plugin offset of an object. */
int FUN_0058c8a0(void *o)
{
  return FLD(o, int, g_data_7daa18);
}
