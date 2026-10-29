/* compile: /G5 */
#include "mfl_tier2.h"
/*00579680*/ extern void __cdecl FUN_00579680(void *a, void *b);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Calls FUN_00579680 with the object's +0xc0 member and the embedded record at +0xc4; returns the object. (Rwp library, proposed name rwp_pipeline_00573860) */
void *FUN_00573860(void *o)
{
  FUN_00579680(FLD(o, void *, 0xc0), (char *)o + 0xc4);
  return o;
}
