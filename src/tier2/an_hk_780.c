#include "mfl_tier2.h"
/*00430ab0*/ extern int screen_width();
/*00430ac0*/ extern int screen_height();
/*005dd624*/ extern float k_f_0_0020833334;
/*005de7c0*/ extern float k_f_0_0015625;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008c8000*/ extern RwIm2DVertex g_im2d_vertices[4];
/*008c8078*/ extern char g_training_lesson[];
/* draw_rect */

/* Draws a filled, alpha blended, untextured screen quad as a tristrip of g_im2d_vertices.
 * x, y, width, height: rectangle in 640x480 reference units, scaled to the current screen size
 *   (inferred from the 1/640 and 1/480 constants).
 * colour: 0xAARRGGBB (inferred), written into each vertex as bytes R, G, B, A.
 * Returns nothing; the quad is drawn immediately.
 * (was: "(x, y, w, h, colour) in screen fractions; alpha blended quad") */

void draw_rect(float x,float y,float width,float height,uint colour)

{
  volatile float height_px;
  float left_px;
  float top_px;
  float width_px;
  RwGlobalsRelease *rw_globals;
  short screen_dim;
  RwReal *vertex_cursor;

  /* No culling, so the quad is visible from either side. */
  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLNONE);
  /* Scale to pixels: x and width by screen_width / 640, y and height by screen_height / 480. */
  screen_dim = screen_width();
  left_px = (float)(int)screen_dim * x * k_f_0_0015625;
  screen_dim = screen_height();
  top_px = (float)(int)screen_dim * y * k_f_0_0020833334;
  screen_dim = screen_width();
  width_px = (float)(int)screen_dim * width * k_f_0_0015625;
  screen_dim = screen_height();
  rw_globals = RwEngineInstance;
  height_px = (float)(int)screen_dim * height * k_f_0_0020833334;
  /* Corners in tristrip order: 0 top-left, 1 top-right, 2 bottom-left, 3 bottom-right. */
  g_im2d_vertices[1].x = width_px + left_px;
  g_im2d_vertices[2].y = height_px + top_px;
  vertex_cursor = &g_im2d_vertices[0].z;
  g_im2d_vertices[0].x = left_px;
  g_im2d_vertices[0].y = top_px;
  g_im2d_vertices[1].y = top_px;
  g_im2d_vertices[2].x = left_px;
  g_im2d_vertices[3].x = g_im2d_vertices[1].x;
  g_im2d_vertices[3].y = g_im2d_vertices[2].y;
  /* Per-vertex setup over the four vertices (the cursor starts at each vertex's z, 7 floats apart;
     the loop ends at &g_training_lesson, the global after the array):
     emissive colour repacked from 0xAARRGGBB to bytes R, G, B, A; z = zBufferNear; rhw = 1.0 (inferred). */
  do {
    *(uint *)&vertex_cursor[2] = (((uint)((*(ushort *)((char *)&colour + 2)) >> 8) << 8 | colour & 0xff) << 8 |
                         colour >> 8 & 0xff) << 8 | (*(ushort *)((char *)&colour + 2)) & 0xff;
    *vertex_cursor = RwGlobalsRelease_dOpenDevice_zBufferNear(rw_globals);
    vertex_cursor[1] = 1.0;
    vertex_cursor = vertex_cursor + 7;
  } while ((char *)vertex_cursor < ((char *)&g_training_lesson));
  /* Untextured (raster 0), vertex alpha on (TRUE, inferred), standard src-alpha / inv-src-alpha blend. */
  (*RwGlobalsRelease_dOpenDevice_fpRenderStateSet(rw_globals))(rwRENDERSTATETEXTURERASTER,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDINVSRCALPHA);
  /* Draw the four vertices as one triangle strip (two triangles = one quad). */
  RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP,g_im2d_vertices,4);
  return;
}


