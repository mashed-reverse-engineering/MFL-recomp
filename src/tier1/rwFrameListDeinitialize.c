/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Frees the frame array of a frame list when it has entries; returns the list. */
void *rwFrameListDeinitialize(void *fl)
{
  if (FLD(fl, int, 4)) RwFree(FLD(fl, void *, 0));
  return fl;
}
