/* compile: /G5 */
#include "mfl_tier2.h"
/*007d52a0*/ extern int g_rp_7d52a0;
/*004d0f70*/ extern void __cdecl RwFrameUpdateObjects(void *f);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Marks the atomic's frame dirty and stores the world in its world plugin slot; returns the atomic. */
void *WorldAddClumpAtomic(void *atomic, void *world)
{
  char *e = (char *)atomic + g_rp_7d52a0;
  if (FLD(atomic, void *, 4)) { RwFrameUpdateObjects(FLD(atomic, void *, 4)); FLD(e, void *, 0) = world; return atomic; }
  FLD(e, void *, 0) = world;
  return atomic;
}
