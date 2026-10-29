/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Clears field +0x40 of an Rwp shape; returns TRUE. (Rwp library, proposed name rwp_trilist_00574030) */
int gap_574030(void *o)
{
  FLD(o, int, 0x40) = 0;
  return 1;
}
