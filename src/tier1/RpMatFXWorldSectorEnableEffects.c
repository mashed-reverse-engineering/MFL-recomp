/* compile: /G5 */
#include "mfl_tier2.h"
/*007da72c*/ extern int g_rp_7da72c;
/*005540a0*/ extern int __cdecl rpMatFXPipelineAtomicSetup(void *a);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Enables MatFX on a world sector: sets up its pipeline the first time; returns the sector or NULL. */
void *RpMatFXWorldSectorEnableEffects(void *s)
{
  char *e = (char *)s + g_rp_7da72c;
  if (FLD(e, int, 0) == 0) { if (!rpMatFXPipelineAtomicSetup(s)) return 0; FLD(e, int, 0) = 1; }
  return s;
}
