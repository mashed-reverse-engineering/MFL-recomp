// cluster-entries: gfx_draw_movie_rect_2d
/* Installed cluster gfx_draw_movie_rect_2d (tools/cluster_install.py from output/cluster_pilot_uc3/gfx_draw_movie_rect_2d.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008c8000*/ extern RwIm2DVertex g_im2d_vertices[4];
/*00430ab0*/ extern short screen_width(void);
/*00430ac0*/ extern short screen_height(void);
/*004a4030*/ extern int __cdecl app_get_app_76e890_4030(int index);
/*004a4020*/ extern int stub_return_one_004a4020(int index);
/*004a34b0*/ extern void app_get_two_size_pairs(float *first, float *second);
/*0050b5e0*/ extern void nop_stub(int arg);
extern int g_app_76e8e0;
extern int g_app_76e8dc;

/* Draws the movie frame as a quad: (x, y, w, h) in 640x480 units, the texture window taken from the two size pairs of the movie surface (u = first[0] / second[0],
   v = first[1] / second[1]), vertex colour white with alpha `alpha`.  `texture` 0 / 1 select the movie surfaces g_app_76e8dc / 76e8e0 (the first one when
   the movie code is stubbed: black), any other value is used as the raster.  unused0..3 are not read. */
void gfx_draw_movie_rect_2d(float x, float y, float w, float h, int unused0, int unused1, int unused2, int unused3, void *texture, unsigned int alpha)
{
  float sizes[4];      /* two (width, height) pairs: surface size and texture size */
  float u, v;
  int i;

  RwRenderStateSet(rwRENDERSTATECULLMODE, (void *)1);
  if (app_get_app_76e890_4030(0) == 0) {
    return;
  }
  x = (float)(int)screen_width() * x * 0.0015625f;
  y = (float)(int)screen_height() * y * 0.0020833334f;
  w = (float)(int)screen_width() * w * 0.0015625f;
  h = (float)(int)screen_height() * h * 0.0020833334f;
  app_get_two_size_pairs(&sizes[0], &sizes[2]);
  u = sizes[0] / sizes[2];
  v = sizes[1] / sizes[3];
  g_im2d_vertices[1].x = w + x;
  g_im2d_vertices[0].x = x;
  g_im2d_vertices[0].y = y;
  g_im2d_vertices[2].y = h + y;
  g_im2d_vertices[1].y = y;
  g_im2d_vertices[0].u = 0.0f;
  g_im2d_vertices[2].u = 0.0f;
  g_im2d_vertices[2].v = 0.0f;
  g_im2d_vertices[3].v = 0.0f;
  g_im2d_vertices[0].v = v;
  g_im2d_vertices[1].u = u;
  g_im2d_vertices[1].v = v;
  g_im2d_vertices[2].x = x;
  g_im2d_vertices[3].x = g_im2d_vertices[1].x;
  g_im2d_vertices[3].y = g_im2d_vertices[2].y;
  g_im2d_vertices[3].u = u;
  for (i = 0; i < 4; i++) {
    g_im2d_vertices[i].emissiveColor = ((alpha & 0xff) << 0x18) | 0xffffff;
    g_im2d_vertices[i].z = RwIm2DGetNearScreenZ();
    g_im2d_vertices[i].rhw = 1.0f;
  }
  RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void *)2);
  RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)1);
  RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)5);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)6);
  if (texture != (void *)0 && texture != (void *)1) {
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, texture);
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void *)3);
    RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, g_im2d_vertices, 4);
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void *)1);
    return;
  }
  if (stub_return_one_004a4020(0) == 0) {
    for (i = 0; i < 4; i++) {
      g_im2d_vertices[i].emissiveColor = 0xff000000;
    }
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)0);
    nop_stub(0);
  } else {
    if (texture == (void *)1) {
      RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)g_app_76e8e0);
    } else {
      RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)g_app_76e8dc);
    }
    nop_stub(1);
  }
  RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void *)3);
  RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, g_im2d_vertices, 4);
  RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void *)1);
  nop_stub(0);
}

