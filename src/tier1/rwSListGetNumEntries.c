/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Entry count of a simple list. */
int rwSListGetNumEntries(void *a)
{
  return FLD(a, int, 4);
}
