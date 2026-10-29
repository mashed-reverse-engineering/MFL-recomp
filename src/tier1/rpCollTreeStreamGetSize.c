/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stream size of a collision tree: (node count + 3) * 16, plus 2 bytes per face when flag 1 is set. */
int rpCollTreeStreamGetSize(void *t)
{
  int n = (FLD(t, unsigned short, 0x1e) + 3) << 4;
  if (FLD(t, unsigned char, 0) & 1) n += FLD(t, unsigned short, 0x1c) * 2;
  return n;
}
