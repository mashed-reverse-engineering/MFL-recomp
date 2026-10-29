/* compile: /G5 */
#include "mfl_tier2.h"
/*007da774*/ extern int g_rp_7da774;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Number of user data arrays of the geometry that have data (array record: 16 bytes, data pointer at +0xc). */
int FUN_005542c0(void *g)
{
  int cnt = 0;
  int n = FLD(g, int, g_rp_7da774);
  int *p;
  if (n > 0) { p = (int *)(FLD(g, char *, g_rp_7da774 + 4) + 0xc); do { if (*p) cnt++; p += 4; } while (--n); }
  return cnt;
}
