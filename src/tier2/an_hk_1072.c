#include "mfl_tier2.h"
/*00562310*/ extern struct RpMaterial * __cdecl RpMaterialUVAnimApplyUpdate(struct RpMaterial *p0, undefined4 p1, undefined4 p2);
/*005625b0*/ extern struct RpMaterial * RpMaterialUVAnimAddAnimTime(struct RpMaterial *p0, RwReal p1);
/*005625f0*/ extern RwBool __cdecl RpMaterialUVAnimExists(struct RpMaterial *p0);
/* clump_for_all_materials_uv_anim_cb_484850 */

/* function pointer pushed/stored by clump_for_all_materials_uv_anim (MOV dword ptr [ESP +
   0xc],0x484850) */

RpMaterial * clump_for_all_materials_uv_anim_cb_484850(RpMaterial *param_1,RwReal *param_2)

{
  RwBool RVar1;
  RpMaterial *pRVar2;
  RwReal deltaTime;
  
  deltaTime = *param_2;
  RVar1 = RpMaterialUVAnimExists(param_1);
  if (RVar1 != 0) {
    pRVar2 = param_1;
    RpMaterialUVAnimAddAnimTime(param_1,deltaTime);
    RpMaterialUVAnimApplyUpdate((struct RpMaterial *)param_1,(undefined4)pRVar2,*(int *)&deltaTime);
  }
  return param_1;
}


