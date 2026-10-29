/* compile: /G5 */
#include "mfl_tier2.h"
/*007daa18*/ extern int g_data_7daa18;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Geometry plugin constructor: clears the DMorph slot, returns the object. */
void *DMorphGeometryCtor(void *o)
{
  FLD(o, int, g_data_7daa18) = 0;
  return o;
}
