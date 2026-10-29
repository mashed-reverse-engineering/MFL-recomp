/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Reads the 16 bit entry idx (16 bit index) of the table at +0xb810 of an Rwp block. (Rwp library, proposed name rwp_farfield_00576f40) */
unsigned short FUN_00576f40(void *o, unsigned idx)
{
  return FLD(o, unsigned short, (idx & 0xffff) * 2 + 0xb810);
}
