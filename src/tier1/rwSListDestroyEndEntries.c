/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Removes n entries from the end of a simple list. */
void rwSListDestroyEndEntries(void *o, int n)
{
  FLD(o, int, 4) -= n;
}
