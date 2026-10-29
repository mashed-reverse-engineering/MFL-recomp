/* compile: /G5 */
#include "mfl_tier2.h"
/*007da774*/ extern int g_rp_7da774;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* User data array record number i of the geometry (16 bytes each), or NULL when out of range. */
char *FUN_005542f0(void *g, int i)
{
  if (i < FLD(g, int, g_rp_7da774)) return FLD(g, char *, g_rp_7da774 + 4) + (i << 4);
  return 0;
}
