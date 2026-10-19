#include "mfl_tier2.h"
/*00430ab0*/ extern int screen_width();
/*00430ac0*/ extern int screen_height();
/*005dd624*/ extern float k_f_0_0020833334;
/*005de7c0*/ extern float k_f_0_0015625;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008c8000*/ extern RwIm2DVertex g_im2d_vertices[4];
/*008c807c*/ extern char g_car_8c807c[];

#define RECT_VERTEX_COUNT (4)
#define VERTEX_STRIDE_FLOATS (7) /* RwIm2DVertex is 28 bytes = 7 RwReal */

/* gfx_draw_rect_2d_gradient_rgb */

/* "640x480 rect with the colour split into per vertex channels" */

/*
 * Draws a filled rectangle as a 4 vertex triangle strip, with a colour gradient across it.
 * x, y, width, height: rectangle in 640x480 reference units, scaled to the real screen size.
 * rgba: packed colour, bytes R, G, B, A from the lowest byte up (inferred).
 *       Left vertices (0, 2) take rgba as is; right vertices (1, 3) take RGB with alpha 0,
 *       so the rectangle fades out towards the right under SRCALPHA / INVSRCALPHA blending (inferred).
 * Sets the render states it needs (no culling, no texture, vertex alpha on, alpha blending)
 * and returns nothing.
 */
/* TIER2: validated 2026-10-19 authority=diffemu */
void gfx_draw_rect_2d_gradient_rgb(float x,float y,float width,float height,uint rgba)
{
  volatile float height_px;
  float left_px;
  float top_px;
  float width_px;
  RwGlobalsRelease *engine;
  short screen_size;
  RwReal *vertex_rhw;
  uint green;

  /* culling off (rwCULLMODECULLNONE) */
  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLNONE);
  /* 640x480 units to screen pixels: multiply by screen size / 640 (x) and / 480 (y) */
  screen_size = screen_width();
  left_px = (float)(int)screen_size * x * k_f_0_0015625;
  screen_size = screen_height();
  top_px = (float)(int)screen_size * y * k_f_0_0020833334;
  screen_size = screen_width();
  width_px = (float)(int)screen_size * width * k_f_0_0015625;
  screen_size = screen_height();
  engine = RwEngineInstance;
  green = rgba >> 8 & 0xff;
  height_px = (float)(int)screen_size * height * k_f_0_0020833334;
  /* right edge and bottom edge: the size is added to the origin */
  g_im2d_vertices[1].x = width_px + left_px;
  g_im2d_vertices[2].y = height_px + top_px;
  /* left vertices (0, 2) colour: bytes R, G, B, A as ARGB (A from byte 3) */
  g_im2d_vertices[0].emissiveColor =
       (((uint)((*(ushort *)((char *)&rgba + 2)) >> 8) << 8 | rgba & 0xff) << 8 | green) << 8 | (*(ushort *)((char *)&rgba + 2)) & 0xff
  ;
  /* right vertices (1, 3) colour: RGB, alpha 0 */
  g_im2d_vertices[1].emissiveColor = ((rgba & 0xff) << 8 | green) << 8 | (*(ushort *)((char *)&rgba + 2)) & 0xff;
  vertex_rhw = &g_im2d_vertices[0].rhw;
  g_im2d_vertices[0].x = left_px;
  g_im2d_vertices[0].y = top_px;
  g_im2d_vertices[1].y = top_px;
  g_im2d_vertices[2].x = left_px;
  g_im2d_vertices[2].emissiveColor = g_im2d_vertices[0].emissiveColor;
  g_im2d_vertices[3].x = g_im2d_vertices[1].x;
  g_im2d_vertices[3].y = g_im2d_vertices[2].y;
  g_im2d_vertices[3].emissiveColor = g_im2d_vertices[1].emissiveColor;
  /* per vertex: z = device near plane, rhw = 1.0 (no perspective divide).
     Walks the rhw field of the 4 vertices; the bound is the address g_car_8c807c (inferred, 12 bytes past the array) */
  do {
    vertex_rhw[-1] = RwGlobalsRelease_dOpenDevice_zBufferNear(engine);
    *vertex_rhw = 1.0;
    vertex_rhw = vertex_rhw + VERTEX_STRIDE_FLOATS;
  } while ((int)vertex_rhw < ((int)&g_car_8c807c));
  /* plain coloured triangles: no texture, vertex colour with alpha blending */
  (*RwGlobalsRelease_dOpenDevice_fpRenderStateSet(engine))(rwRENDERSTATETEXTURERASTER,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDINVSRCALPHA);
  RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP,g_im2d_vertices,RECT_VERTEX_COUNT);
  return;
}
