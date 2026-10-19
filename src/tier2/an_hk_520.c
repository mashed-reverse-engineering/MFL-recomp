#include "mfl_tier2.h"
/*00475730*/ extern int get_viewport_camera();
/*0049c5f0*/ extern undefined __cdecl fx_snow_ptank_update(int p0, struct RwCamera *p1, undefined1 *p2, float p3, float p4);
/*008bf084*/ extern undefined4 g_course_training_8bf084;

#define SNOW_COLOUR (0xff6c6c6c)
#define SNOW_SPEED (0.05)
#define SNOW_WIND (0.025)

/* course_training_cb1 (inferred: per-frame draw callback of the training course)
 * Draws the snow particle effect for the current viewport camera.
 * param_1: game state pointer (inferred); field at +0xc134 is passed to func_0x00484960.
 *          Overwritten with SNOW_COLOUR midway, so it holds the colour after that point.
 * param_2: passed to func_0x00484b20 with the course global (inferred).
 * Returns nothing.
 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void course_training_cb1(int param_1,undefined4 param_2)

{
  struct {
    undefined4 x;
    undefined4 y;
    undefined4 z;
  } origin;

  func_0x0049e1b0(get_viewport_camera(0),0,0x40140000);
  func_0x00484960(*(undefined4 *)(param_1 + 0xc134),func_0x00484b20(g_course_training_8bf084,param_2));
  ((RwRGBA *)&param_1)->red = 0x6c;
  ((RwRGBA *)&param_1)->green = 0x6c;
  ((RwRGBA *)&param_1)->blue = 0x6c;
  ((RwRGBA *)&param_1)->alpha = 0xff;
  origin.x = 0;
  origin.y = 0;
  origin.z = 0;
  func_0x0049c180(&origin.x);
  fx_snow_ptank_update(0,(RwCamera *)get_viewport_camera(0),(undefined1 *)&param_1,SNOW_SPEED,SNOW_WIND);
  return;
}


