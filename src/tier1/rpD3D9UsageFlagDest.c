/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Plugin destructor: clears the usage flag, returns the object. */
void *rpD3D9UsageFlagDest(void *o, int off)
{
  FLD(o, int, off) = 0;
  return o;
}
