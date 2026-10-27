#include "mfl_tier2.h"
/*0042c630*/ extern undefined __cdecl ui_text_draw_rotated(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6);
/*0042c8e0*/ extern float text_width();
/*00430ab0*/ extern int screen_width();
/*00430ac0*/ extern int screen_height();
extern double cos(double);
extern double sin(double);
/* menu_player_select_draw_rotated */

/* "menu_draw_player_select_sub_42c630 with sin/cos of the angle" */

/* (inferred names) angle_bits is the float angle in degrees passed as raw bits; the text is shifted so that it rotates around its centre */
void menu_player_select_draw_rotated(undefined4 text_id,float pos_x,float pos_y,undefined4 colour,undefined4 scale
               ,undefined4 angle_bits)
{
  double half_width;
  double angle;
  double offset_y;
  short screen_h;
  short screen_w;
  float x;
  float y;
  
  half_width = text_width(text_id,scale) * 0.5f;
  screen_h = screen_height();
  angle = *(float *)&angle_bits * 0.017453292f;      /* degrees to radians */
  offset_y = (double)sin(angle) * (double)(int)screen_h * (half_width * 0.8f);
  screen_w = screen_width();
  y = (float)(offset_y / (double)(int)screen_w + (double)pos_y);
  x = (float)((double)pos_x - (double)cos(angle) * half_width);
  ui_text_draw_rotated(text_id,*(int *)&x,*(int *)&y,colour,scale,0,angle_bits);
  return;
}
