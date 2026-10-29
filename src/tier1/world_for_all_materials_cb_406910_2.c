#include "mfl_tier2.h"
/*004f8230*/ extern struct RpMaterial * RpMaterialSetTexture(struct RpMaterial *p0, struct RwTexture *p1);
/*00843e80*/ extern undefined4 g_course_843e80;
/*0089de04*/ extern undefined4 g_app_89de04;
/* world_for_all_materials_cb_406910_2 */

/* function pointer pushed/stored by world_for_all_materials_cb_406910 (PUSH 0x406910) (name existed
   on another function) */

RpMaterial * world_for_all_materials_cb_406910_2(RpMaterial *param_1,RwTexture *param_2)

{
  int *piVar1;
  
  piVar1 = (int *)((char *)&RpMaterial_texture(param_1) + g_app_89de04);
  if (*piVar1 != 0) {
    if (g_course_843e80 != 1) {
      RpMaterialSetTexture(param_1,param_2);
    }
    piVar1[2] = (int)param_2;
  }
  return param_1;
}
