/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Destroys a simple list and returns its array when the list had entries, NULL otherwise. */
void *rwSListToArray(void *l)
{
  void *arr = 0;
  if (FLD(l, int, 4)) arr = FLD(l, void *, 0);
  else if (FLD(l, void *, 0)) { RwFree(FLD(l, void *, 0)); FLD(l, int, 0) = 0; arr = 0; }
  RwFree(l);
  return arr;
}
