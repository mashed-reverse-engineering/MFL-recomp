/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* User data plugin constructor: clears the array count and pointer; returns the object. */
void *UserDataObjectConstruct(void *o, int off)
{
  FLD(o, int, off) = 0;
  FLD(o, int, off + 4) = 0;
  return o;
}
