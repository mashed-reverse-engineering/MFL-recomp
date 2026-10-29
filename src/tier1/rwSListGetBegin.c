/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Start pointer of a simple list. */
void *rwSListGetBegin(void *a)
{
  return FLD(a, void *, 0);
}
