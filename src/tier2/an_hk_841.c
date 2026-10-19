#include "mfl_tier2.h"
/*0053e4d0*/ extern RwBool RtAnimInterpolatorSubAnimTime(struct RtAnimInterpolator *p0, RwReal p1);
/*00544450*/ extern RwBool RpHAnimHierarchyUpdateMatrices(struct RpHAnimHierarchy *p0);
/* anim_update_hierarchy */

/* WARNING: Unknown calling convention */
/* "RtAnimInterpolatorSubAnimTime + RpHAnimHierarchyUpdateMatrices" */

/* Advances the animation of the hierarchy held by anim_owner and rebuilds its matrices.
 *   anim_owner: object whose +0x50 holds the RpHAnimHierarchy pointer and whose +0x44 holds the
 *               time step (RwReal) (inferred); +0x48 is set to 5 (meaning unknown, inferred state marker).
 *   return_value: passed through unchanged.
 * Returns return_value. */
undefined4 anim_update_hierarchy(undefined4 return_value,char *anim_owner)

{
  RwReal t = *(RwReal *)(anim_owner + 0x44);
  RtAnimInterpolatorSubAnimTime
            (RpHAnimHierarchy_currentAnim((*(RpHAnimHierarchy **)(anim_owner + 0x50))),t);
  RpHAnimHierarchyUpdateMatrices(*(RpHAnimHierarchy **)(anim_owner + 0x50));
  *(undefined4 *)(anim_owner + 0x48) = 5;
  return return_value;
}



