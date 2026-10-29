#include "mfl_tier2.h"
/*004c52b0*/ extern int atomic_geometry_user_data_int(void *p0, undefined4 p1, undefined4 p2);
/*004f7f90*/ extern RwBool __cdecl RpAtomicInstance(struct RpAtomic *p0);
#define ATOMIC_ID_SKIP_FIRST (0x3b)
#define ATOMIC_ID_SKIP_LAST (0x3e)
#define ATOMIC_ID_SKIP_EXTRA (0x40)
/* atomic_instance_filter */
/* "RpAtomicInstance unless the atomic id is in 0x3B..0x3E or 0x40" */
/* Instances `atomic` unless its atomic id (inferred: value returned by atomic_geometry_user_data_int) falls in
   ATOMIC_ID_SKIP_FIRST..ATOMIC_ID_SKIP_LAST or equals ATOMIC_ID_SKIP_EXTRA.
   Parameter: atomic - the atomic to filter. Returns atomic unchanged. */
RpAtomic * atomic_instance_filter(RpAtomic *atomic)
{
  int atomic_id;

  atomic_id = atomic_geometry_user_data_int(atomic,0,0);
  if ((atomic_id < ATOMIC_ID_SKIP_FIRST) ||
     ((ATOMIC_ID_SKIP_LAST < atomic_id && (atomic_id != ATOMIC_ID_SKIP_EXTRA)))) {
    RpAtomicInstance(atomic);
  }
  return atomic;
}
