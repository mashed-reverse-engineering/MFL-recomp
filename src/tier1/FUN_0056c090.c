/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets 16-bit field +0x5a of a 2D object. (Rwp library, proposed name rwp_sphere_0056c090) */
void FUN_0056c090(void *o, short v)
{
  FLD(o, short, 0x5a) = v;
}
