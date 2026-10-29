/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Address of entry i of a simple list. */
void *rwSListGetEntry(void *l, int i)
{
  return (char *)FLD(l, char *, 0) + FLD(l, int, 0xc) * i;
}
