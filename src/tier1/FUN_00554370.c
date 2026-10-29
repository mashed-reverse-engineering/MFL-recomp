/* compile: /G5 */
#include "mfl_tier2.h"
/*007da79c*/ extern int g_rp_7da79c;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Number of user data arrays of the second slot that have data. */
int FUN_00554370(void *g)
{
  int cnt = 0;
  int n = FLD(g, int, g_rp_7da79c);
  int *p;
  if (n > 0) { p = (int *)(FLD(g, char *, g_rp_7da79c + 4) + 0xc); do { if (*p) cnt++; p += 4; } while (--n); }
  return cnt;
}
