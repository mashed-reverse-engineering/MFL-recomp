/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Replaces the contribution of field +0xc in the running total at +0x18 by a new value. (Rwp library, proposed name rwp_trilist_00573fd0) */
void FUN_00573fd0(void *o, int v)
{
  if (FLD(o, int, 0xc)) FLD(o, int, 0x18) -= FLD(o, int, 0xc);
  FLD(o, int, 0xc) = v;
  FLD(o, int, 0x18) = FLD(o, int, 0x18) + v;
}
