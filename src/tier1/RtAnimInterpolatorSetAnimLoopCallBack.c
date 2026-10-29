/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets the animation loop callback and its data. */
void RtAnimInterpolatorSetAnimLoopCallBack(void *i, void *cb, void *d)
{
  FLD(i, void *, 0x18) = cb;
  FLD(i, void *, 0x1c) = d;
}
