#include "mfl_tier2.h"
/*00475730*/ extern char *get_viewport_camera();
/*0049c5f0*/ extern undefined __cdecl fx_snow_ptank_update(int p0, struct RwCamera *p1, undefined1 *p2, float p3, float p4);
/*008bf084*/ extern undefined4 g_course_training_8bf084;
#define SNOW_GREY (0x6c) /* rgb of the snow tint */
#define SNOW_SPEED (0.05)
#define SNOW_WIND (0.025)
/* course_training_cb1 (inferred: per-frame draw callback of the training course)
 * Draws the snow particle effect for the current viewport camera.
 * course: game state pointer (inferred); field at +0xc134 is passed to func_0x00484960.
 *          Overwritten with SNOW_COLOUR midway, so it holds the colour after that point.
 * clump_arg: passed to func_0x00484b20 with the course global (inferred).
 * Returns nothing.
 */
void course_training_cb1(char *course,undefined4 clump_arg)
{
  struct {
    undefined4 x;
    undefined4 y;
    undefined4 z;
  } origin;

  func_0x0049e1b0(get_viewport_camera(0),0,0x40140000);
  func_0x00484960(((CourseWorld *)course)->clumps[9],func_0x00484b20(g_course_training_8bf084,clump_arg));
  ((RwRGBA *)&course)->red = SNOW_GREY;
  ((RwRGBA *)&course)->green = SNOW_GREY;
  ((RwRGBA *)&course)->blue = SNOW_GREY;
  ((RwRGBA *)&course)->alpha = 0xff;
  origin.x = 0;
  origin.y = 0;
  origin.z = 0;
  func_0x0049c180(&origin.x);
  fx_snow_ptank_update(0,(RwCamera *)get_viewport_camera(0),(undefined1 *)&course,SNOW_SPEED,SNOW_WIND);
  return;
}
