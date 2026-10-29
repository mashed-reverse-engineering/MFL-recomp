/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Matrix array of an animation hierarchy. */
void *RpHAnimHierarchyGetMatrixArray(void *a)
{
  return FLD(a, void *, 8);
}
