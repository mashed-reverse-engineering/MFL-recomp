/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Address one past the last entry of a simple list. */
void *rwSListGetEnd(void *o)
{
  return (char *)FLD(o, char *, 0) + FLD(o, int, 4) * FLD(o, int, 0xc);
}
