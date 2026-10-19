#include "mfl_tier2.h"
/*0044c5c0*/ extern int collision_debug_draw(int p0, int p1, int p2, int p3, int p4, int p5, int p6);
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd634*/ extern float k_f_2;
/*005dd814*/ extern float k_f_6;
/*005ddb14*/ extern float float_7_0;
/* ai_debug_draw_marker_box */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "draws a coloured box around a point, colour by param_3 = 1,2,3" */

void __cdecl ai_debug_draw_marker_box(float param_1,float param_2,float param_3)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  uchar col[4];
  
  col[1] = 0;
  col[2] = 0;
  col[0] = 0xff;
  if (param_3 == float_1_0) {
    col[1] = 0x80;
  }
  if (param_3 == k_f_2) {
    col[1] = 0xff;
  }
  if (param_3 == k_f_3) {
    col[0] = 0x80;
    col[1] = 0xff;
  }
  fVar1 = param_2 - float_7_0;
  col[3] = 0xff;
  fVar2 = param_1 - k_f_6;
  fVar3 = param_2 + float_7_0;
  fVar4 = param_1 + float_7_0;
  fVar5 = param_1 - float_7_0;
  collision_debug_draw(*(int *)&fVar5,*(int *)&fVar1,*(int *)&fVar4,*(int *)&fVar3,*(int *)&fVar2,*(int *)&fVar1,*(int *)col);
  collision_debug_draw(*(int *)&fVar5,*(int *)&fVar3,*(int *)&fVar4,*(int *)&fVar1,*(int *)&fVar2,*(int *)&fVar3,*(int *)col);
  return;
}


