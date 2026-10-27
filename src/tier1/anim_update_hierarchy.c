#include "mfl_tier2.h"
/*0053e4d0*/ extern RwBool RtAnimInterpolatorSubAnimTime(struct RtAnimInterpolator *p0, RwReal p1);
/* layout of *anim_owner in anim_update_hierarchy, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct anim_update_hierarchy_anim_owner_layout {
    char pad_0x0[68];
    RwReal f_0x44;
    undefined4 f_0x48;
    char pad_0x4c[4];
    RpHAnimHierarchy * f_0x50;
} anim_update_hierarchy_anim_owner_layout;

/*00544450*/ extern RwBool RpHAnimHierarchyUpdateMatrices(struct RpHAnimHierarchy *p0);
/* anim_update_hierarchy */
/* WARNING: Unknown calling convention */
/* "RtAnimInterpolatorSubAnimTime + RpHAnimHierarchyUpdateMatrices" */
/* Advances the animation of the hierarchy held by anim_owner and rebuilds its matrices.
 *   anim_owner: object whose +0x50 holds the RpHAnimHierarchy pointer and whose +0x44 holds the
 *               time step (RwReal) (inferred); +0x48 is set to 5 (meaning unknown, inferred state marker).
 *   return_value: passed through unchanged.
 * Returns return_value. */
undefined4 anim_update_hierarchy(undefined4 return_value,anim_update_hierarchy_anim_owner_layout *anim_owner)
{
  RwReal t = anim_owner->f_0x44;
  RtAnimInterpolatorSubAnimTime
            (RpHAnimHierarchy_currentAnim((anim_owner->f_0x50)),t);
  RpHAnimHierarchyUpdateMatrices(anim_owner->f_0x50);
  anim_owner->f_0x48 = 5;
  return return_value;
}
