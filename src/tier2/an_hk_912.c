#include "mfl_tier2.h"
/*004824a0*/ extern int draw_rect(float x, float y, float w, float h, uint colour);
/*005dd33c*/ extern float k_f_4;
/*005dd634*/ extern float k_f_2;
/*005de9a4*/ extern float k_f_48;
/* menu_draw_selection_frame */

/* "draws a framed rectangular frame of 5 draw_rect calls with alpha param_2" */

void menu_draw_selection_frame(int param_1,byte param_2)

{
  float fVar1;
  float fVar2;
  float fVar3;
  uchar fill[4];
  uchar edge[4];
  
  fVar1 = (float)(param_1 + -0xd);
  fill[0] = 0xf0;
  fill[1] = 0x6e;
  fill[2] = 0x14;
  fill[3] = param_2 >> 1;
  draw_rect(48.0f,fVar1,534.0f,26.0f,*(uint *)fill);
  edge[0] = 0xb4;
  edge[1] = 0x50;
  edge[2] = 0x10;
  edge[3] = param_2;
  draw_rect(46.0f,fVar1,2.0f,26.0f,*(uint *)edge);
  fVar2 = k_f_4 + 534.0f;
  draw_rect(46.0f,fVar1,fVar2,2.0f,*(uint *)edge);
  fVar3 = 26.0f - k_f_2;
  fVar1 = fVar3 + fVar1;
  draw_rect(46.0f,fVar1,fVar2,2.0f,*(uint *)edge);
  draw_rect(k_f_48 + 534.0f,fVar1 - fVar3,2.0f,26.0f,*(uint *)edge);
  return;
}
