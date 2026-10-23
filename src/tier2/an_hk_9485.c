#include "mfl_tier2.h"
/*0049b890*/ extern int particle_batch_add(int p0, void *p1, void *p2);
/*0049b910*/ extern int particle_batch_set_uv_a(int p0, void *p1);
/*0049b940*/ extern int particle_batch_set_size(int p0, int p1);
/*0049bd50*/ extern int fx_system_flush(int p0);
/*005dd330*/ extern float float_1_0;
/*005dd328*/ extern float k_f_0_6;
/*005dd370*/ extern float k_f_0_3;
/* fx_ribbon_add_point */

/* "appends a point (pos, optional vel, colour) to a ribbon ring and fades the older points" */

void fx_ribbon_add_point(int *param_1,undefined4 *param_2,undefined4 *param_3,undefined4 *param_4)

{
  float *tmp_f;
  int *ribbon;
  int ring_size;
  undefined4 *point_ptr;
  int age_index;
  double width;
  double fade;
  uint alpha;
  struct {
    float uv_a_x;
    float uv_a_y;
    undefined4 point_x;
    undefined4 point_y;
    undefined4 point_z;
  } frame;
  
  ribbon = param_1;
  point_ptr = (undefined4 *)(*param_1 + param_1[9] * 0xc);
  *point_ptr = *param_2;
  point_ptr[1] = param_2[1];
  point_ptr[2] = param_2[2];
  if (param_3 == (undefined4 *)0x0) {
    *(undefined4 *)(param_1[1] + param_1[9] * 0xc) = 0;
    *(undefined4 *)(param_1[1] + 4 + param_1[9] * 0xc) = 0;
    *(undefined4 *)(param_1[1] + 8 + param_1[9] * 0xc) = 0;
  }
  else {
    point_ptr = (undefined4 *)(param_1[1] + param_1[9] * 0xc);
    *point_ptr = *param_3;
    point_ptr[1] = param_3[1];
    point_ptr[2] = param_3[2];
  }
  if (param_4 == (undefined4 *)0x0) {
    *(undefined1 *)(param_1[2] + param_1[9] * 4) = 0xff;
    *(undefined1 *)(param_1[2] + 1 + param_1[9] * 4) = 0xff;
    *(undefined1 *)(param_1[2] + 2 + param_1[9] * 4) = 0xff;
    *(undefined1 *)(param_1[2] + 3 + param_1[9] * 4) = 0xff;
  }
  else {
    *(undefined4 *)(param_1[2] + param_1[9] * 4) = *param_4;
  }
  age_index = param_1[9];
  ring_size = param_1[3];
  param_1[9] = age_index + 1;
  if (ring_size <= age_index + 1) {
    param_1[9] = 0;
  }
  age_index = ring_size - param_1[4];
  if (age_index < ring_size) {
    do {
      ring_size = ribbon[9] + age_index;
      fade = float_1_0 - (double)age_index / ribbon[3];
      if (ribbon[3] <= ring_size) {
        ring_size = (ribbon[9] - ribbon[3]) + age_index;
      }
      *(uint *)&param_2 = *(uint *)(ribbon[2] + ring_size * 4);
      alpha = *(uint *)&param_2 >> 0x18;
      *((char *)&param_2 + 3) = (char)(int)((double)(int)alpha - (double)(int)(alpha - 10) * fade);
      ring_size = ring_size * 0xc;
      point_ptr = (undefined4 *)(*ribbon + ring_size);
      frame.point_x = *point_ptr;
      width = ((*(float *)&ribbon[7] - *(float *)&ribbon[6]) * fade + (double)*(float *)&ribbon[6]) * (double)k_f_0_3;
      frame.point_y = point_ptr[1];
      frame.point_z = point_ptr[2];
      frame.uv_a_y = (float)width;
      frame.uv_a_x = (float)(width * (double)k_f_0_6);
      *(float *)&param_3 = (float)(fade * (double)*(float *)&ribbon[8]);
      particle_batch_add(ribbon[10],&frame.point_x,&param_2);
      particle_batch_set_uv_a(ribbon[10],&frame.uv_a_x);
      particle_batch_set_size(ribbon[10],*(int *)&param_3);
      age_index = age_index + 1;
      *(float *)(*ribbon + ring_size) = *(float *)(ring_size + ribbon[1]) + *(float *)(*ribbon + ring_size);
      tmp_f = (float *)(*ribbon + 4 + ring_size);
      *tmp_f = *(float *)(ring_size + 4 + ribbon[1]) + *tmp_f;
      tmp_f = (float *)(*ribbon + 8 + ring_size);
      *tmp_f = *(float *)(ring_size + 8 + ribbon[1]) + *tmp_f;
    } while (age_index < ribbon[3]);
    fx_system_flush(ribbon[10]);
  }
  return;
}
