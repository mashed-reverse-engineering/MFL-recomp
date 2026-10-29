/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stores two values in fields +0x144 / +0x148; returns the object. */
void *FUN_005d0b40(void *o, int a, int b)
{
  FLD(o, int, 0x144) = a;
  FLD(o, int, 0x148) = b;
  return o;
}
