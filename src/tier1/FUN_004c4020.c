#include "mfl_tier2.h"
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/* FUN_004c4020 */
/*
 * Returns the geometry of the first atomic in a clump.
 * clump: the RpClump to inspect.
 * Returns the atomic's geometry (RpGeometry *), or 0 if the clump has no atomics.
 */
undefined4 __cdecl FUN_004c4020(undefined4 clump)
{
  RpAtomic *atomic;
  undefined4 g = 0;
  atomic = clump_first_atomic(clump);
  if (atomic != 0) {
    g = RpAtomic_geometry(atomic);
  }
  return g;
}
