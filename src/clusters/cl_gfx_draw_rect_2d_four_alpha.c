// cluster-entries: gfx_draw_rect_2d_four_alpha
/* Installed cluster gfx_draw_rect_2d_four_alpha (tools/cluster_install.py from output/cluster_pilot_zy/gfx_draw_rect_2d_four_alpha.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008c8000*/ extern RwIm2DVertex g_im2d_vertices[4];
/*00430ab0*/ extern short screen_width(void);
/*00430ac0*/ extern short screen_height(void);
/*005de7c0*/ extern float k_f_0_0015625;
/*005dd624*/ extern float k_f_0_0020833334;

/* Draws an untextured screen rectangle (x, y, w, h in 640x480 units) with the colour `colour` (bytes 0xRRGGBB, packed to the vertex order) and a separate alpha for each of the 4 corners. */
void gfx_draw_rect_2d_four_alpha(float x, float y, float w, float h, unsigned int colour,
                                 unsigned char alpha0, unsigned char alpha1, unsigned char alpha2, unsigned char alpha3)
{
  unsigned int c_b, c_g, c_r;
  int i;
  volatile float px, py, pw, ph;  /* ponytail: the original keeps the scaled values as 32-bit floats in memory */

  RwRenderStateSet(rwRENDERSTATECULLMODE, (void *)0x1);
  px = (float)(int)screen_width() * x * k_f_0_0015625;
  py = (float)(int)screen_height() * y * k_f_0_0020833334;
  pw = (float)(int)screen_width() * w * k_f_0_0015625;
  ph = (float)(int)screen_height() * h * k_f_0_0020833334;
  c_r = colour >> 0x10 & 0xff;
  c_b = colour & 0xff;
  c_g = colour >> 8 & 0xff;
  g_im2d_vertices[0].emissiveColor = (((unsigned int)alpha0 << 8 | c_b) << 8 | c_g) << 8 | c_r;
  g_im2d_vertices[1].x = pw + px;
  g_im2d_vertices[2].y = ph + py;
  g_im2d_vertices[1].emissiveColor = (((unsigned int)alpha1 << 8 | c_b) << 8 | c_g) << 8 | c_r;
  g_im2d_vertices[2].emissiveColor = (((unsigned int)alpha2 << 8 | c_b) << 8 | c_g) << 8 | c_r;
  g_im2d_vertices[3].emissiveColor = (((unsigned int)alpha3 << 8 | c_b) << 8 | c_g) << 8 | c_r;
  g_im2d_vertices[0].x = px;
  g_im2d_vertices[0].y = py;
  g_im2d_vertices[1].y = py;
  g_im2d_vertices[2].x = px;
  g_im2d_vertices[3].x = g_im2d_vertices[1].x;
  g_im2d_vertices[3].y = g_im2d_vertices[2].y;
  for (i = 0; i < 4; i++) {
    g_im2d_vertices[i].z = RwIm2DGetNearScreenZ();
    g_im2d_vertices[i].rhw = 1.0f;
  }
  RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)0x0);
  RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)0x1);
  RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)0x5);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)0x6);
  RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, g_im2d_vertices, 4);
}

