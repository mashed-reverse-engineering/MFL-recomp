/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets the delta-time scale of a 2D animation. */
void Rt2dAnimSetDeltaTimeScale(void *f, float v)
{
  FLD(f, float, 4) = v;
}
