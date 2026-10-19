#include "mfl_tier2.h"
/*00484630*/ extern int material_flag_4_set();
/*004f6810*/ extern struct RpClump * RpClumpForAllAtomics(struct RpClump *p0, RpAtomicCallBack p1, void *p2);
/* clump_set_flag_on_atomics */

/* Applies material_flag_4_set (0x484630) to every atomic of a clump via RpClumpForAllAtomics.
   The callback receives a pointer to flag_value as its data argument (the flag is inferred to
   be a visibility flag, set when a clump is added to / removed from the world).
   Does nothing when clump is NULL.
   (was: "RpClumpForAllAtomics with the callback at 0x484630 (visibility flag, called as (clump, 1/0)
   after adding / removing a clump)") */

void clump_set_flag_on_atomics(RpClump *clump, undefined4 flag_value)

{
  /* NULL clump: nothing to walk */
  if (clump != (RpClump *)0x0) {
    RpClumpForAllAtomics((struct RpClump *)clump,(RpAtomicCallBack)material_flag_4_set,(void *)&flag_value);
  }
  return;
}


