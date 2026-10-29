/* compile: /G5 */
#include "mfl_tier2.h"
/*007d52a0*/ extern int g_rp_7d52a0;
/*004d0f70*/ extern void __cdecl RwFrameUpdateObjects(void *f);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Marks the atomic frame dirty and stores the world in the atomic's world plugin slot; returns the world. */
void *RpWorldAddAtomic(void *w, void *atomic)
{
  char *e = (char *)atomic + g_rp_7d52a0;
  if (FLD(atomic, void *, 4)) RwFrameUpdateObjects(FLD(atomic, void *, 4));
  FLD(e, void *, 0) = w;
  return w;
}
