#include "mfl_tier2.h"
/*00434a20*/ extern char *get_second_camera();
/*00475730*/ extern char *get_viewport_camera();
/*004a1860*/ extern int game_set_option_flags(unsigned char a, unsigned char b, unsigned char c);
/*004d1bd0*/ extern struct RwCamera * RwCameraSetNearClipPlane(struct RwCamera *p0, RwReal p1);
/*004d1ca0*/ extern struct RwCamera * RwCameraSetFarClipPlane(struct RwCamera *p0, RwReal p1);
/*008c8840*/ extern float g_course_8c8840;
/*008c8844*/ extern undefined4 g_course_8c8844;
/*008c8848*/ extern undefined4 g_course_8c8848;
/*008c8850*/ extern undefined1 g_course_8c8850;
/*008c8854*/ extern undefined1 g_course_8c8854;
/*008c8858*/ extern undefined1 g_course_8c8858;
/* course_apply_clip_planes */
/* Applies the course's clip distances to the viewport camera and the second camera (inferred: split-screen).
 * Near plane = g_course_8c8840 (float), far plane = g_course_8c8844 (its bits are copied through clip_distance),
 * fog plane = g_course_8c8848 (written to RwCamera::fogPlane at +0x88, inferred from the offset).
 * Then calls game_set_option_flags with g_course_8c8850/54/58 (meaning unknown).
 * Takes no parameters, returns nothing.
 * (was: "near / far clip plane of the main camera from the course (0x8C8840 / 0x8C8844)") */
void course_apply_clip_planes(void)
{
  RwCameraSetNearClipPlane((RwCamera *)get_viewport_camera(0),*(float *)&g_course_8c8840);
  RwCameraSetFarClipPlane((RwCamera *)get_viewport_camera(0),*(float *)&g_course_8c8844);
  *(undefined4 *)&RwCamera_fogPlane(((RwCamera *)get_viewport_camera(0))) = g_course_8c8848;
  get_viewport_camera(0);
  RwCameraSetNearClipPlane((RwCamera *)get_second_camera(),*(float *)&g_course_8c8840);
  RwCameraSetFarClipPlane((RwCamera *)get_second_camera(),*(float *)&g_course_8c8844);
  *(undefined4 *)&RwCamera_fogPlane(((RwCamera *)get_second_camera())) = g_course_8c8848;
  get_second_camera();
  game_set_option_flags(g_course_8c8850,g_course_8c8854,g_course_8c8858);
  return;
}
