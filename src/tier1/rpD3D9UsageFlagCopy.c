/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Plugin copy: copies the usage flag word; returns the destination. */
void *rpD3D9UsageFlagCopy(void *dst, void *src, int off)
{
  FLD(dst, int, off) = FLD(src, int, off);
  return dst;
}
