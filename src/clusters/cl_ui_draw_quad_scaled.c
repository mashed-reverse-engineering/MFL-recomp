// cluster-entries: ui_draw_quad_scaled
/* Installed cluster ui_draw_quad_scaled (tools/cluster_install.py from output/cluster_pilot_zy/ui_draw_quad_scaled.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00430ab0*/ extern short screen_width(void);
/*00430ac0*/ extern short screen_height(void);
/*005de7c0*/ extern float k_f_0_0015625;
/*005dd624*/ extern float k_f_0_0020833334;
/*00482330*/ extern void im2d_draw_quad(int mode, float x0, float y0, float x1, float y1, unsigned int colour, float x2, float y2, float x3, float y3);

/* Draws a screen quad whose corners (x0,y0) and (x1,y1) are given in 640x480 units; scaled to the current resolution (1/640, 1/480). */
void ui_draw_quad_scaled(float x0, float y0, float x1, float y1, unsigned int colour)
{
  double sx = (double)(int)screen_width() * k_f_0_0015625;
  double sy = (double)(int)screen_height() * k_f_0_0020833334;

  im2d_draw_quad(0, sx * x0, y0 * sy, sx * x1, sy * y1, colour, sx * x1, sy * y1, sx * x0, y0 * sy);
}

