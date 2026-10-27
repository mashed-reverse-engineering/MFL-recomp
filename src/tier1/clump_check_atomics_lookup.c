#include "mfl_tier2.h"
/*004f6810*/ extern struct RpClump * RpClumpForAllAtomics(struct RpClump *p0, RpAtomicCallBack p1, void *p2);
extern undefined4 atomic_lookup_error(undefined4 param_1,int *param_2);


/* clump_check_atomics_lookup */

/* Iterates over all atomics in the clump to check for lookup errors. */
/* "RpClumpForAllAtomics(clump, atomic_lookup_error)" */

void clump_check_atomics_lookup(undefined4 *clump_info)
{
  struct { int unknown; undefined4 * clump_data; } lcl_;
#define unknown lcl_.unknown
#define clump_data lcl_.clump_data
  
  /* Initialize callback context (unknown field and clump data pointer) */
  unknown = 0;
  clump_data = clump_info;
  RpClumpForAllAtomics((RpClump *)*clump_info,atomic_lookup_error,&unknown);
  return;
}
