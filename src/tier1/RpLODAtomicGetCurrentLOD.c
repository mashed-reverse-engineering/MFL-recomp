/* compile: /G5 */
#include "mfl_tier2.h"
/*009488d4*/ extern int g_rp_9488d4;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Current LOD index of an atomic, -1 without LOD data. */
int RpLODAtomicGetCurrentLOD(void *a)
{
  int r = -1;
  void *p = FLD(a, void *, g_rp_9488d4);
  if (p) r = FLD(p, int, 0x14);
  return r;
}
