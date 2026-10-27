#include "mfl_tier2.h"
extern char LAB_004068a0[];
/*004f6810*/ extern struct RpClump * RpClumpForAllAtomics(struct RpClump *p0, RpAtomicCallBack p1, void *p2);
/* clump_for_all_atomics_cb_4068a0 */
/* "RpClumpForAllAtomics(clump, FUN_004068a0, data)" */
/* Runs the atomic callback LAB_004068a0 (inferred: FUN_004068a0) over every atomic
   in `clump` via RpClumpForAllAtomics, passing `user_data` through to each call.
   Parameters: clump - clump to iterate; user_data - opaque pointer forwarded to the callback.
   Returns nothing. */
void clump_for_all_atomics_cb_4068a0(RpClump *clump,void *user_data)
{
  RpClumpForAllAtomics(clump,(RpAtomicCallBack)&LAB_004068a0,user_data);
  return;
}
