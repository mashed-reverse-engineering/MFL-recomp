/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Writes the 16 bit entry idx of the table at +0xb810 of an Rwp block; returns the block. (Rwp library, proposed name rwp_farfield_00576f60) */
void *FUN_00576f60(void *o, unsigned idx, unsigned short v)
{
  FLD(o, unsigned short, (idx & 0xffff) * 2 + 0xb810) = v;
  return o;
}
