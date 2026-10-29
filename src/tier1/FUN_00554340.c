/* compile: /G5 */
#include "mfl_tier2.h"
/*007da77c*/ extern int g_rp_7da77c;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* User data array record number i of the second slot, or NULL. */
char *FUN_00554340(void *g, int i)
{
  if (i < FLD(g, int, g_rp_7da77c)) return FLD(g, char *, g_rp_7da77c + 4) + (i << 4);
  return 0;
}
