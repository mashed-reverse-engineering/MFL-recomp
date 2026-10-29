/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stream size of a keyframe array: 36 bytes per frame. */
int RpHAnimKeyFrameStreamGetSize(void *a)
{
  return FLD(a, int, 4) * 36;
}
