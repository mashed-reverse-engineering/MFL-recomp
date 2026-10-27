#include "mfl_tier2.h"
extern char LAB_00405bb0[];
/*004757a0*/ extern void * get_viewport_camera_matrix();
/*004f6810*/ extern struct RpClump * RpClumpForAllAtomics(struct RpClump *p0, RpAtomicCallBack p1, void *p2);
/* clump_for_all_atomics_with_camera_matrix
 * Runs RpClumpForAllAtomics on `clump`, calling the callback at LAB_00405bb0 for each atomic
 * and passing the viewport camera matrix as its data pointer.
 * Parameters: clump - the RpClump whose atomics are visited.
 * Returns nothing.
 */
/* "RpClumpForAllAtomics(clump, FUN_00405bb0, viewport camera matrix 0)" */
void __cdecl clump_for_all_atomics_with_camera_matrix(RpClump *clump)
{
  void *viewport_camera_matrix;

  /* viewport 0's camera matrix, used as the callback's data pointer (inferred) */
  viewport_camera_matrix = (void *)get_viewport_camera_matrix(0);
  RpClumpForAllAtomics(clump,(RpAtomicCallBack)&LAB_00405bb0,viewport_camera_matrix);
  return;
}
