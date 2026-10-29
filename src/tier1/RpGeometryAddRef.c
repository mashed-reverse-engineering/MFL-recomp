/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Increments the reference count of a geometry; returns it. */
void *RpGeometryAddRef(void *g)
{
  FLD(g, short, 0xe)++;
  return g;
}
