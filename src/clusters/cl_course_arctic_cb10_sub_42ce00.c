// cluster-entries: course_arctic_cb10_sub_42ce00
/* Installed cluster course_arctic_cb10_sub_42ce00 (tools/cluster_install.py from output/cluster_pilot_kc/course_arctic_cb10_sub_42ce00.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00430ab0*/ extern int screen_width();
/*00430ac0*/ extern int screen_height();
/*0045bdd0*/ extern void draw_textured_quad_uv(undefined4 texture, float x, float y, float width, float height, undefined4 flags, void *uv_rect);
/*005dd340*/ extern float k_f_0_5;
/*005dd624*/ extern float k_f_0_0020833334;
/*005de7c0*/ extern float k_f_0_0015625;
/* rounding point: the original spills these products to floats */
static float float_barrier(float value)
{
  return value;
}

/* course_arctic_cb10_sub_42ce00 */

/* single caller course_arctic_cb10 */

/* (inferred) draws a sprite given in 640x480 units (x, y, w, h scaled to the screen); anchor 0 / 1 = x,y is the top left corner, 2 = x,y is the centre; texture_ref may be NULL (texture 0), full 0..1 uv rect */
void course_arctic_cb10_sub_42ce00(undefined4 *texture_ref, float x, float y, float w, float h, int anchor, undefined4 colour)

{
  float y_scale;
  short screen_dim;
  undefined4 texture;
  float x_scale;
  float width;
  float height;
  struct {
    float pos_x;
    float pos_y;
    undefined4 uv_u0;
    undefined4 uv_v0;
    undefined4 uv_u1;
    undefined4 uv_v1;
  } frame;
  
  frame.pos_x = 0.0;
  frame.pos_y = 0.0;
  screen_dim = screen_width();
  x_scale = float_barrier((float)(int)screen_dim * k_f_0_0015625);
  screen_dim = screen_height(*(int *)&x_scale);
  texture = 0;
  frame.uv_u0 = 0;
  frame.uv_v0 = 0;
  y_scale = (float)(int)screen_dim * k_f_0_0020833334;
  frame.uv_u1 = 0x3f800000;
  frame.uv_v1 = 0x3f800000;
  width = float_barrier(x_scale * w);
  height = float_barrier(h * y_scale);
  if ((anchor == 0) || (anchor == 1)) {
    frame.pos_x = x_scale * x;
    frame.pos_y = y_scale * y;
  }
  else if (anchor == 2) {
    frame.pos_x = x_scale * x - width * k_f_0_5;
    frame.pos_y = y_scale * y - height * k_f_0_5;
  }
  if (texture_ref != (undefined4 *)0x0) {
    texture = *texture_ref;
  }
  draw_textured_quad_uv(texture,frame.pos_x,frame.pos_y,width,height,colour,&frame.uv_u0);
  return;
}

