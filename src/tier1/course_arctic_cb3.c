#include "mfl_tier2.h"
/*0042b230*/ extern int course_sky_clumps_update();
/*0042b2a0*/ extern int course_sky_clumps_render();
/*004524e0*/ extern undefined __cdecl course_arctic_cb3_object_init(void *p0);
/*004525b0*/ extern int course_arctic_snow_quad_draw();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*00481fd0*/ extern float random_range();
/*005dda24*/ extern float k_f_0_75;
/*005ddb20*/ extern float k_f_0_9;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008c58c0*/ extern undefined4 g_course_arctic_8c58c0;
/* course_arctic_cb3
 * Per-frame update and render for the arctic course: sky clumps plus a falling-snow effect.
 * Takes no parameters, returns nothing.
 * The snow state is g_course_arctic_8c58c0: 0 = no snow active, otherwise the snow object
 * (its address is passed to course_arctic_cb3_object_init and course_arctic_snow_quad_draw).
 * Random rolls are in [0, 1) (inferred from FLOAT_ONE_BITS): snow starts when roll > 0.75 (about 25% per frame)
 * and stops when roll > 0.9 (about 10% per frame) (inferred).
 */
void course_arctic_cb3(void)
{
  course_sky_clumps_update(get_viewport_camera_matrix(0));
  course_sky_clumps_render();
  RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDONE);
  if (g_course_arctic_8c58c0 != 0) {
    course_arctic_snow_quad_draw(&g_course_arctic_8c58c0,get_viewport_camera_matrix(0));
    if (random_range(0,FLOAT_ONE_BITS) > k_f_0_9) {
      g_course_arctic_8c58c0 = 0;
    }
  }
  else {
    if (random_range(0,FLOAT_ONE_BITS) > k_f_0_75) {
      course_arctic_cb3_object_init(&g_course_arctic_8c58c0);
    }
  }
  RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDINVSRCALPHA);
  return;
}
