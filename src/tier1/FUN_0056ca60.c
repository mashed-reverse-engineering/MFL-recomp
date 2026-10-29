/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets field +0x48 of a 2D object. (Rwp library, proposed name rwp_sphere_0056ca60) */
void FUN_0056ca60(void *o, int v)
{
  FLD(o, int, 0x48) = v;
}
