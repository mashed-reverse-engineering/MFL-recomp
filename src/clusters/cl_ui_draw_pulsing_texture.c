// cluster-entries: ui_draw_pulsing_texture
/* Installed cluster ui_draw_pulsing_texture (tools/cluster_install.py from output/cluster_pilot_zy/ui_draw_pulsing_texture.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00430ab0*/ extern short screen_width(void);
/*00430ac0*/ extern short screen_height(void);
/*005de7c0*/ extern float k_f_0_0015625;
/*005dd624*/ extern float k_f_0_0020833334;
/*005e64a4*/ extern float k_f_0;
/*005de88c*/ extern float k_f_512;
/*005de3e4*/ extern float k_f_40;
/*005de888*/ extern float k_f_72;
/*005dd7f4*/ extern float k_f_80;
/*005dd378*/ extern float k_f_0_1;
/*0069bb44*/ extern float g_course_69bb44;
/*0076e7ec*/ extern int *g_car_76e7ec;
/*0045bdd0*/ extern void draw_textured_quad_uv(int texture, float x, float y, float w, float h, unsigned int colour, float *uv);
extern double sin(double x);
#pragma intrinsic(sin)

/* Draws the pulsing (sine scaled) icon at (x, y) in 640x480 units, `scale` times its 80 unit size; the phase g_course_69bb44 advances by 0.1 per call. */
void ui_draw_pulsing_texture(int x, int y, float scale)
{
  float uv[4];
  float pulse;
  double scaled;
  float sx;
  double sy;
  volatile float pulse80;
  int texture = 0;

  pulse = (float)sin(g_course_69bb44);
  uv[0] = 0.0f;
  uv[1] = 0.0f;
  uv[2] = 0.5f;
  uv[3] = 1.0f;
  sx = (float)(int)screen_width() * k_f_0_0015625;
  sy = (double)(int)screen_height() * k_f_0_0020833334;
  scaled = pulse * scale;
  pulse80 = pulse * k_f_80;
  if (scaled < k_f_0) {
    uv[0] = 1.0f;
    uv[2] = 0.5f;
  }
  if (g_car_76e7ec != 0) {
    texture = *g_car_76e7ec;
  }
  draw_textured_quad_uv(texture, sx * ((k_f_512 - k_f_40 * scaled) + (float)x), sy * ((float)y + k_f_72),
                        sx * (pulse80 * scale), sy * (scale * k_f_80), 0xffffffff, uv);
  g_course_69bb44 = g_course_69bb44 + k_f_0_1;
}

