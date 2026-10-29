/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Returns the root frame of a frame (RwFrameGetRoot). */
void *RwFrameGetRoot(void *f)
{
  return FLD(f, void *, 0xa0);
}
