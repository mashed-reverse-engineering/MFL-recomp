/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Dirty bits (low two) of the root frame's private flags. */
int RwFrameDirty(void *f)
{
  return FLD(FLD(f, void *, 0xa0), unsigned char, 3) & 3;
}
