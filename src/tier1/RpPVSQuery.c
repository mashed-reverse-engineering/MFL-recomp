/* compile: /G5 */
#include "mfl_tier2.h"
/*00947ae0*/ extern void *g_rp_947ae0;
/*00947ae4*/ extern int g_rp_947ae4;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Remembers the queried world and returns its PVS plugin data. */
int RpPVSQuery(void *w)
{
  g_rp_947ae0 = w;
  return FLD(w, int, g_rp_947ae4);
}
