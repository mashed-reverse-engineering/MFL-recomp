/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Frees the data (when owned) and the header of a simple list; returns TRUE. */
int rwSListDestroy(void *l)
{
  if (FLD(l, void *, 0) && FLD(l, int, 8)) { RwFree(FLD(l, void *, 0)); FLD(l, int, 0) = 0; FLD(l, int, 8) = 0; }
  RwFree(l);
  return 1;
}
