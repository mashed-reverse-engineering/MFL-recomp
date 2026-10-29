/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Zeroes fields +0xc and +0x10. */
void FUN_005d1ff0(void *o)
{
  FLD(o, int, 0xc) = 0;
  FLD(o, int, 0x10) = 0;
}
