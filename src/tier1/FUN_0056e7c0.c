/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets float field +0 of a 2D object. (Rwp library, proposed name rwp_obj_0056e7c0) */
void FUN_0056e7c0(void *o, float v)
{
  FLD(o, float, 0) = v;
}
