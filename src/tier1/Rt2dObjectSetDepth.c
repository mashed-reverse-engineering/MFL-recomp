/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets the depth of a 2D object; returns the object. */
void *Rt2dObjectSetDepth(void *o, int v)
{
  FLD(o, int, 4) = v;
  return o;
}
