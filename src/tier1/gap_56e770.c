/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets field +0x3c of a 2D object; returns the object. (Rwp library, proposed name rwp_obj_0056e770) */
void *gap_56e770(void *o, int v)
{
  FLD(o, int, 0x3c) = v;
  return o;
}
