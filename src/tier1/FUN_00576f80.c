/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets field +0xc018 of a physics block; returns the block. (Rwp library, proposed name rwp_farfield_00576f80) */
void *FUN_00576f80(void *o, int v)
{
  FLD(o, int, 0xc018) = v;
  return o;
}
