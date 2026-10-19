#include "mfl_tier2.h"
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/* FUN_004c3fd0 */

/* Passes the clump to clump_get_atomics() with a NULL output list, so the
 * call only runs the atomic iteration on the clump (inferred).
 * Parameter: clump - the RpClump to process. No return value. */
void FUN_004c3fd0(undefined4 clump)

{
  clump_get_atomics(clump,0);
  return;
}


