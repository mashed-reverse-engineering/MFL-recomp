/* compile: /G5 */
#include "mfl_tier2.h"
/*007da79c*/ extern int g_rp_7da79c;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* User data array record number i of the third slot, or NULL. */
char *FUN_005543a0(void *g, int i)
{
  if (i < FLD(g, int, g_rp_7da79c)) return FLD(g, char *, g_rp_7da79c + 4) + (i << 4);
  return 0;
}
