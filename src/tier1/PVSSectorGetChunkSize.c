/* compile: /G5 */
#include "mfl_tier2.h"
/*00947ae8*/ extern int g_pvscalculatesectorbounds_947ae8;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stream size of the PVS sector data: 0 without data, otherwise data size plus a 12 byte header. */
int PVSSectorGetChunkSize(void *s)
{
  char *p = (char *)s + g_pvscalculatesectorbounds_947ae8;
  if (FLD(p, int, 0) == -1) return 0;
  return FLD(p, int, 4) + 0xc;
}
