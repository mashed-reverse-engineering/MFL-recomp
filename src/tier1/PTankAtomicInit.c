/* compile: /G5 */
#include "mfl_tier2.h"
/*007da6bc*/ extern int g_ptank_plugin_offset;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Atomic plugin constructor: clears the point-tank slot, returns the object. */
void *PTankAtomicInit(void *o)
{
  FLD(o, int, g_ptank_plugin_offset) = 0;
  return o;
}
