/* compile: /G5 */
#include "mfl_tier2.h"
/*007da864*/ extern int g_rp_7da864;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Area light colour from the lightmap material plugin slot. */
int RtLtMapMaterialGetAreaLightColor(void *m)
{
  return FLD(m, int, g_rp_7da864 + 8);
}
