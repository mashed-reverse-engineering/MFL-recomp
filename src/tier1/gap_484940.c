#include "mfl_tier2.h"
/*004f8230*/ extern struct RpMaterial * RpMaterialSetTexture(struct RpMaterial *p0, struct RwTexture *p1);
/* gap_484940 */

/* no context */

RpMaterial * gap_484940(RpMaterial *param_1,RwTexture *param_2)

{
  if (param_2 != (RwTexture *)0x0) {
    RpMaterialSetTexture(param_1,param_2);
    return param_1;
  }
  return param_1;
}
