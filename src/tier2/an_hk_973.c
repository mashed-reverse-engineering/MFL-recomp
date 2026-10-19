#include "mfl_tier2.h"
/*0042c630*/ extern undefined __cdecl ui_text_draw_rotated(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6);
/*0042c8e0*/ extern float text_width();
/*00430ab0*/ extern int screen_width();
/*00430ac0*/ extern int screen_height();
extern double cos(double);
extern double sin(double);
/* menu_player_select_draw_rotated */

/* "menu_draw_player_select_sub_42c630 with sin/cos of the angle" */

void menu_player_select_draw_rotated(undefined4 param_1,float param_2,float param_3,undefined4 param_4,undefined4 param_5
               ,undefined4 param_6)
{
  float10 half_width;
  float10 angle;
  float10 offset_y;
  short screen_h;
  short screen_w;
  float x;
  float y;
  
  half_width = text_width(param_1,param_5) * 0.5f;
  screen_h = screen_height();
  angle = *(float *)&param_6 * 0.017453292f;      /* degrees to radians */
  offset_y = (float10)sin(angle) * (float10)(int)screen_h * (half_width * 0.8f);
  screen_w = screen_width();
  y = (float)(offset_y / (float10)(int)screen_w + (float10)param_3);
  x = (float)((float10)param_2 - (float10)cos(angle) * half_width);
  ui_text_draw_rotated(param_1,*(int *)&x,*(int *)&y,param_4,param_5,0,param_6);
  return;
}
