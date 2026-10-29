#include "mfl_tier2.h"
/*0053e640*/ extern RwBool RtAnimInterpolatorAddAnimTime(struct RtAnimInterpolator *p0, RwReal p1);
/*00544450*/ extern RwBool RpHAnimHierarchyUpdateMatrices(struct RpHAnimHierarchy *p0);
/* Advances a hierarchy's current animation by delta_time, then rebuilds the hierarchy's node matrices from the updated animation pose. */
/* FUN_004c4890
 * Advances a hierarchy's current animation by delta_time, then rebuilds the
 * hierarchy's node matrices from the updated animation pose.
 * hierarchy: skinned hierarchy to update.
 * delta_time: time step passed to the animation interpolator.
 * Returns nothing.
 */
void FUN_004c4890(RpHAnimHierarchy *hierarchy,RwReal delta_time)
{
  /* Step the animation interpolator (inferred) */
  RtAnimInterpolatorAddAnimTime(RpHAnimHierarchy_currentAnim(hierarchy),delta_time);
  /* Recompute the matrices for the new pose */
  RpHAnimHierarchyUpdateMatrices(hierarchy);
  return;
}
