/* compile: /G5 */
#include "mfl_tier2.h"
/*0053e3b0*/ extern void __cdecl RtAnimInterpolatorDestroy(void *p);
/* Frees the collision tree stored at the given plugin offset and clears it; returns the object. */
void *CollisionDataDestructor(void *o, int off)
{
  void **slot = (void **)((char *)o + off);
  if (*slot) { RtAnimInterpolatorDestroy(*slot); *slot = 0; }
  return o;
}
