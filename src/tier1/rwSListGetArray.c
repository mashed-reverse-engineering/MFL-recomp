/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Start of a simple list, or NULL when it has no entries. */
void *rwSListGetArray(void *l)
{
  return FLD(l, int, 4) ? FLD(l, void *, 0) : 0;
}
