#include "mfl_tier2.h"
/*004521f0*/ extern int course_arctic_frames_sway_update();
/*00452300*/ extern int course_arctic_frames_update();
/*00475730*/ extern char *get_viewport_camera();
/*0049af00*/ extern int fx_slot_set_field_b8();
/*004a1220*/ extern undefined __cdecl rain_update_render(struct RwCamera *p0);
/*005dd374*/ extern float k_f_0_2;
/*006a0774*/ extern undefined4 g_course_arctic_6a0774;
/*006a077c*/ extern float g_course_arctic_6a077c;
/*008c5740*/ extern undefined4 g_course_arctic_8c5740;
extern double sin(double);
/* course_arctic_cb1 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void course_arctic_cb1(undefined4 param_1)

{
  float fbits1;
  RwCamera *camera;
  double fVar1;
  
  course_arctic_frames_sway_update(param_1);
  course_arctic_frames_update(param_1);
  camera = (RwCamera *)get_viewport_camera(0);
  rain_update_render(camera);
  fVar1 = (double)g_course_arctic_6a077c;
  g_course_arctic_6a077c = (float)(fVar1 + (double)k_f_0_2);
  fVar1 = (double)sin(fVar1 + (double)k_f_0_2);
  fx_slot_set_field_b8
            ((&g_course_arctic_8c5740)[g_course_arctic_6a0774],
             (fbits1 = (float)((fVar1 + (double)(1.0)) * (double)(0.15000000596046448)), *(int *)&fbits1));
  return;
}


