/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets float field +0x4c of a 2D object. (Rwp library, proposed name rwp_sphere_0056c080) */
void FUN_0056c080(void *o, float v)
{
  FLD(o, float, 0x4c) = v;
}
