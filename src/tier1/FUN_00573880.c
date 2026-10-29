/* compile: /G5 */
#include "mfl_tier2.h"
/*00579480*/ extern void __cdecl FUN_00579480(void *a, void *b);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Calls FUN_00579480 with the object's +0xc0 member and the embedded record at +0xc4; returns the object. (Rwp library, proposed name rwp_pipeline_00573880) */
void *FUN_00573880(void *o)
{
  FUN_00579480(FLD(o, void *, 0xc0), (char *)o + 0xc4);
  return o;
}
