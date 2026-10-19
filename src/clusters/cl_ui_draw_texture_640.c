// cluster-entries: ui_draw_texture_640
/* Installed cluster ui_draw_texture_640 (tools/cluster_install.py from output/cluster_pilot_zy/ui_draw_texture_640.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00430ab0*/ extern short screen_width(void);
/*00430ac0*/ extern short screen_height(void);
/*005de7c0*/ extern float k_f_0_0015625;
/*005dd624*/ extern float k_f_0_0020833334;
/*005e64a4*/ extern float k_f_0;
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*0045bdd0*/ extern void draw_textured_quad_uv(int texture, float x, float y, float w, float h, unsigned int colour, float *uv);

/* Draws a texture quad given in 640x480 units; anchor 0/1 = (x,y) is the top-left, 2 = (x,y) is the centre; flip 1 = mirror u, 2 = mirror v, 3 = both. */
void ui_draw_texture_640(int *texture, float x, float y, float w, float h, int anchor, int flip)
{
  float uv[4];
  volatile float sx, sy, pw, ph;  /* ponytail: stored scale factors (the original keeps them as 32-bit floats in memory) */
  float px = 0.0f;
  float py = 0.0f;
  int tex_handle = 0;

  sx = (float)(int)screen_width() * k_f_0_0015625;
  sy = (float)(int)screen_height() * k_f_0_0020833334;
  uv[0] = k_f_0;
  uv[1] = k_f_0;
  uv[2] = float_1_0;
  uv[3] = float_1_0;
  pw = sx * w;
  ph = sy * h;
  if (anchor == 0 || anchor == 1) {
    px = sx * x;
    py = sy * y;
  } else if (anchor == 2) {
    px = sx * x - pw * k_f_0_5;
    py = sy * y - ph * k_f_0_5;
  }
  if (flip == 1) {
    uv[0] = float_1_0;
    uv[2] = k_f_0;
  } else {
    if (flip != 2) {
      if (flip != 3) {
        goto draw;
      }
      uv[0] = float_1_0;
      uv[2] = k_f_0;
    }
    uv[1] = float_1_0;
    uv[3] = k_f_0;
  }
draw:
  if (texture != 0) {
    tex_handle = *texture;
  }
  draw_textured_quad_uv(tex_handle, px, py, pw, ph, 0xffffffff, uv);
}

