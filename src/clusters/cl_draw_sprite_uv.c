// cluster-entries: draw_sprite_uv
/* Installed cluster draw_sprite_uv (tools/cluster_install.py from output/cluster_pilot_zy/draw_sprite_uv.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008c8000*/ extern RwIm2DVertex g_im2d_vertices[4];
/*00430ab0*/ extern short screen_width(void);
/*00430ac0*/ extern short screen_height(void);
/*005de7c0*/ extern float k_f_0_0015625;
/*005dd624*/ extern float k_f_0_0020833334;
/*005dfdfc*/ extern float k_f_0_002232143;

/* Draws a textured screen quad (x, y, w, h) with the texture window u0..u1 / v0..v1 and the colour 0xAARRGGBB.  scale_mode 0: pixels, 1: 640x480 units, 2: only y and h are scaled (by height / 448).
   texture NULL draws untextured; linear selects the texture filter. */
void draw_sprite_uv(int *texture, float x, float y, float w, float h, unsigned int colour,
                    float u0, float u1, float v0, float v1, int scale_mode, int linear)
{
  volatile float px, py, pw, ph;  /* ponytail: the original keeps the scaled values as 32-bit floats in its parameter slots */
  int i;

  RwRenderStateSet(rwRENDERSTATECULLMODE, (void *)0x1);
  px = x;
  py = y;
  pw = w;
  ph = h;
  if (scale_mode != 0) {
    if (scale_mode == 2) {
      py = (float)(int)screen_height() * y * k_f_0_002232143;
      ph = (float)(int)screen_height() * h * k_f_0_002232143;
    } else {
      px = (float)(int)screen_width() * x * k_f_0_0015625;
      py = (float)(int)screen_height() * y * k_f_0_0020833334;
      pw = (float)(int)screen_width() * w * k_f_0_0015625;
      ph = (float)(int)screen_height() * h * k_f_0_0020833334;
    }
  }
  g_im2d_vertices[1].x = pw + px;
  g_im2d_vertices[0].x = px;
  g_im2d_vertices[0].u = u0;
  g_im2d_vertices[2].y = ph + py;
  g_im2d_vertices[0].v = v0;
  g_im2d_vertices[1].v = v0;
  g_im2d_vertices[1].u = u1;
  g_im2d_vertices[0].y = py;
  g_im2d_vertices[1].y = py;
  g_im2d_vertices[2].u = u0;
  g_im2d_vertices[2].x = px;
  g_im2d_vertices[2].v = v1;
  g_im2d_vertices[3].v = v1;
  g_im2d_vertices[3].u = u1;
  g_im2d_vertices[3].x = g_im2d_vertices[1].x;
  g_im2d_vertices[3].y = g_im2d_vertices[2].y;
  for (i = 0; i < 4; i++) {
    g_im2d_vertices[i].emissiveColor = (((colour >> 0x18) << 8 | (colour & 0xff)) << 8 | (colour >> 8 & 0xff)) << 8 | (colour >> 0x10 & 0xff);
    g_im2d_vertices[i].z = RwIm2DGetNearScreenZ();
    g_im2d_vertices[i].rhw = 1.0f;
  }
  RwRenderStateSet(rwRENDERSTATETEXTURERASTER, texture == 0 ? (void *)0x0 : (void *)*texture);
  if (texture != 0) {
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void *)(linear == 0 ? 0x1 : 0x2));
  }
  RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)0x1);
  RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)0x5);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)0x6);
  RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, g_im2d_vertices, 4);
}

