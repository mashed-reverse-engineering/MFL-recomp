/* compile: /G5 */
#include "mfl_tier2.h"
/*004f5e10*/ extern int __cdecl RpWorldForAllWorldSectors(void *w, void *cb, int d);
/*00573e80*/ extern void LAB_00573e80(void);
/*00573ef0*/ extern void FUN_00573ef0(void);
/* Registers two sector callbacks on a world with RpWorldForAllWorldSectors; returns the world. (Rwp library, proposed name rwp_trilist_00573e50) */
void *FUN_00573e50(void *w)
{
  RpWorldForAllWorldSectors(w, LAB_00573e80, 0);
  RpWorldForAllWorldSectors(w, FUN_00573ef0, 0);
  return w;
}
