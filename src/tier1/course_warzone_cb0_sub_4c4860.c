#include "mfl_tier2.h"
/*0053e3c0*/ extern RwBool RtAnimInterpolatorSetCurrentAnim(struct RtAnimInterpolator *p0, struct RtAnimAnimation *p1);
/*00544450*/ extern RwBool RpHAnimHierarchyUpdateMatrices(struct RpHAnimHierarchy *p0);
/* course_warzone_cb0_sub_4c4860 */

/* single caller course_warzone_cb0_sub_45ca50 */

RpHAnimHierarchy * course_warzone_cb0_sub_4c4860(undefined4 param_1,RtAnimAnimation *param_2)

{
  RpHAnimHierarchy *hierarchy;
  
  hierarchy = (RpHAnimHierarchy *)course_warzone_cb0_sub_4c4830(param_1);
  RtAnimInterpolatorSetCurrentAnim(RpHAnimHierarchy_currentAnim(hierarchy),param_2);
  RpHAnimHierarchyUpdateMatrices(hierarchy);
  return hierarchy;
}
