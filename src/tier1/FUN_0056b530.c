/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Field +0x2c of a 2D object. (Rwp library, proposed name rwp_volume_0056b530) */
int FUN_0056b530(void *o)
{
  return FLD(o, int, 0x2c);
}
