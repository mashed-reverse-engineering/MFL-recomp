#include "mfl_tier2.h"
/*00430ab0*/ extern int screen_width();
/*00430ac0*/ extern int screen_height();
/*005dd624*/ extern float k_f_0_0020833334;
/*005de7c0*/ extern float k_f_0_0015625;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008c8000*/ extern RwIm2DVertex g_im2d_vertices[4];
/*008c8078*/ extern char g_training_lesson[];
#define RS_CULL_MODE_NONE ((void *)0x1) /* rwCULLMODECULLNONE */
#define RS_VERTEX_ALPHA_ON ((void *)0x1)
#define RS_BLEND_SRC_ALPHA ((void *)0x5) /* rwBLENDSRCALPHA */
#define RS_BLEND_INV_SRC_ALPHA ((void *)0x6) /* rwBLENDINVSRCALPHA */
#define IM2D_TRISTRIP_VERTEX_COUNT (4)
#define EMISSIVE_OPAQUE_BLACK (0xff000000) /* (inferred) opaque black */
/* gfx_draw_rect_2d_flat_a */

/* "640x480 rect with one colour, emissive vertex 0/1 cleared" */

/* Draws a flat-coloured 2D rectangle in screen space as a 4-vertex triangle strip (inferred: HUD/overlay quad).
 * rect_x, rect_y, rect_w, rect_h: left, top, width, height in 640x480 reference pixels,
 *   scaled to the real screen size with screen_width()/640 and screen_height()/480.
 * colour_argb: 0xAARRGGBB (inferred).
 * Vertices: 0 = top-left, 1 = top-right, 2 = bottom-left, 3 = bottom-right.
 * Leaves the render states changed: cull mode none, no texture, vertex alpha on, src-alpha blending.
 */
void gfx_draw_rect_2d_flat_a(float rect_x,float rect_y,float rect_w,float rect_h,uint colour_argb)

{
  volatile float height_px;
  float left_px;
  float top_px;
  float width_px;
  RwGlobalsRelease *engine;
  short screen_px;
  RwReal *vertex_cursor;

  RwRenderStateSet(rwRENDERSTATECULLMODE,RS_CULL_MODE_NONE);
  screen_px = screen_width();
  left_px = (float)(int)screen_px * rect_x;
  left_px = left_px * k_f_0_0015625;
  screen_px = screen_height();
  top_px = (float)(int)screen_px * rect_y;
  top_px = top_px * k_f_0_0020833334;
  screen_px = screen_width();
  width_px = (float)(int)screen_px * rect_w;
  width_px = width_px * k_f_0_0015625;
  screen_px = screen_height();
  engine = RwEngineInstance;
  height_px = (float)(int)screen_px * rect_h;
  height_px = height_px * k_f_0_0020833334;
  g_im2d_vertices[1].x = width_px + left_px;
  g_im2d_vertices[2].y = height_px + top_px;
  vertex_cursor = &g_im2d_vertices[0].z;
  g_im2d_vertices[0].x = left_px;
  g_im2d_vertices[0].y = top_px;
  g_im2d_vertices[1].y = top_px;
  g_im2d_vertices[2].x = left_px;
  g_im2d_vertices[3].x = g_im2d_vertices[1].x;
  g_im2d_vertices[3].y = g_im2d_vertices[2].y;
  /* Per vertex: z = near depth, rhw = 1.0 (2D, no perspective), colour word = colour_argb with
   * red and blue swapped (inferred: RwRGBA byte order in memory). Cursor steps by 7 words
   * (one RwIm2DVertex, 28 bytes). Stops at g_training_lesson, 8 bytes past the end of
   * g_im2d_vertices (0x8c8078 vs 0x8c8070), so it runs for exactly 4 vertices. */
  do {
    *(uint *)&vertex_cursor[2] = (((uint)((*(ushort *)((char *)&colour_argb + 2)) >> 8) << 8 | colour_argb & 0xff) << 8 |
                         colour_argb >> 8 & 0xff) << 8 | (*(ushort *)((char *)&colour_argb + 2)) & 0xff;
    *vertex_cursor = RwGlobalsRelease_dOpenDevice_zBufferNear(engine);
    vertex_cursor[1] = 1.0;
    vertex_cursor = vertex_cursor + 7;
  } while ((char *)vertex_cursor < ((char *)&g_training_lesson));
  /* Top edge (vertices 0 and 1) gets opaque black emissive instead of the colour. */
  g_im2d_vertices[0].emissiveColor = EMISSIVE_OPAQUE_BLACK;
  g_im2d_vertices[1].emissiveColor = EMISSIVE_OPAQUE_BLACK;
  /* No texture; alpha blend src-alpha over destination. */
  (*RwGlobalsRelease_dOpenDevice_fpRenderStateSet(engine))(rwRENDERSTATETEXTURERASTER,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,RS_VERTEX_ALPHA_ON);
  RwRenderStateSet(rwRENDERSTATESRCBLEND,RS_BLEND_SRC_ALPHA);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,RS_BLEND_INV_SRC_ALPHA);
  /* Draw the quad as a triangle strip. */
  RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP,g_im2d_vertices,IM2D_TRISTRIP_VERTEX_COUNT);
  return;
}


