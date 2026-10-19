#include "mfl_tier2.h"
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*006a07bc*/ extern undefined4 g_course_dem_6a07bc;
/*007ef804*/ extern int g_tick_number;
/* course_dem_cb2 */

/* the original ends in tail jumps to RpClumpRender (both arms); Ghidra inlined the target as a list walk */
RpClump * course_dem_cb2(int param_1)

{
  RpClumpRender(*(RpClump **)(param_1 + 0xc120));
  if (g_course_dem_6a07bc != 0) {
    RpClumpRender(*(RpClump **)(param_1 + 0xc124));
  }
  RpClumpRender(*(RpClump **)(param_1 + 0xc118));
  RpClumpRender(*(RpClump **)(param_1 + 0xc114));
  RpClumpRender(*(RpClump **)(param_1 + 0xc110));
  if (g_tick_number % 0x32 < 0x19) {
    return RpClumpRender(*(RpClump **)(param_1 + 0xc12c));
  }
  return RpClumpRender(*(RpClump **)(param_1 + 0xc130));
}
