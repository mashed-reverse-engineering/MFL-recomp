#include "mfl_tier2.h"
/*004824a0*/ extern int draw_rect(float x, float y, float w, float h, uint colour);
/*005dd33c*/ extern float k_f_4;
/*005dd634*/ extern float k_f_2;
/*005de9a4*/ extern float k_f_48;
/* menu_draw_selection_frame */

/* "draws a framed rectangular frame of 5 draw_rect calls with alpha alpha" */

void menu_draw_selection_frame(int row_centre_y,byte alpha)

{
  float top;
  float frame_w;
  float frame_h;
  uchar fill[4];
  uchar edge[4];
  
  top = (float)(row_centre_y + -0xd);
  fill[0] = 0xf0;
  fill[1] = 0x6e;
  fill[2] = 0x14;
  fill[3] = alpha >> 1;
  draw_rect(48.0f,top,534.0f,26.0f,*(uint *)fill);
  edge[0] = 0xb4;
  edge[1] = 0x50;
  edge[2] = 0x10;
  edge[3] = alpha;
  draw_rect(46.0f,top,2.0f,26.0f,*(uint *)edge);
  frame_w = k_f_4 + 534.0f;
  draw_rect(46.0f,top,frame_w,2.0f,*(uint *)edge);
  frame_h = 26.0f - k_f_2;
  top = frame_h + top;
  draw_rect(46.0f,top,frame_w,2.0f,*(uint *)edge);
  draw_rect(k_f_48 + 534.0f,top - frame_h,2.0f,26.0f,*(uint *)edge);
  return;
}
