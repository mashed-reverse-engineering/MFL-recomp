#include "mfl_tier2.h"
/*0044c5c0*/ extern int collision_debug_draw(int p0, int p1, int p2, int p3, int p4, int p5, int p6);
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd634*/ extern float k_f_2;
/*005dd814*/ extern float k_f_6;
/*005ddb14*/ extern float float_7_0;
/* ai_debug_draw_marker_box */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "draws a coloured box around a point, colour by colour_id = 1,2,3" */

void __cdecl ai_debug_draw_marker_box(float x,float y,float colour_id)

{
  float y_lo;
  float x_lo6;
  float y_hi;
  float x_hi;
  float x_lo;
  uchar col[4];
  
  col[1] = 0;
  col[2] = 0;
  col[0] = 0xff;
  if (colour_id == float_1_0) {
    col[1] = 0x80;
  }
  if (colour_id == k_f_2) {
    col[1] = 0xff;
  }
  if (colour_id == k_f_3) {
    col[0] = 0x80;
    col[1] = 0xff;
  }
  y_lo = y - float_7_0;
  col[3] = 0xff;
  x_lo6 = x - k_f_6;
  y_hi = y + float_7_0;
  x_hi = x + float_7_0;
  x_lo = x - float_7_0;
  collision_debug_draw(*(int *)&x_lo,*(int *)&y_lo,*(int *)&x_hi,*(int *)&y_hi,*(int *)&x_lo6,*(int *)&y_lo,*(int *)col);
  collision_debug_draw(*(int *)&x_lo,*(int *)&y_hi,*(int *)&x_hi,*(int *)&y_lo,*(int *)&x_lo6,*(int *)&y_hi,*(int *)col);
  return;
}


