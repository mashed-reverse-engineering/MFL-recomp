/* compile: /G5 */
#include "mfl_tier2.h"
/*007d54d8*/ extern int g_rp_7d54d8;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Usage flags stored in the geometry's D3D9 plugin slot. */
int RpD3D9GeometryGetUsageFlags(void *o)
{
  return FLD(o, int, g_rp_7d54d8);
}
