/* compile: /G5 */
#include "mfl_tier2.h"
/*00947ae4*/ extern int g_rp_947ae4;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stream size of the PVS world data: 8 when present, otherwise 0. */
int PVSWorldGetChunkSize(void *w)
{
  return FLD(w, int, g_rp_947ae4) ? 8 : 0;
}
