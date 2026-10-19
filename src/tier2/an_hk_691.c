#include "mfl_tier2.h"
/*00430ab0*/ extern int screen_width();
/*00430ac0*/ extern int screen_height();
/*005dd624*/ extern float k_f_0_0020833334;
/*005de7c0*/ extern float k_f_0_0015625;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008c8000*/ extern RwIm2DVertex g_im2d_vertices[4];
/*008c8078*/ extern char g_training_lesson[];
#define IM2D_QUAD_VERTICES (4)
#define IM2D_VERTEX_WORDS (7)
/* gfx_draw_rect_2d_gradient_a */

/* "640x480 rect with two colours (second on vertices 1,3)" */

/* TIER2: unvalidated 2026-10-17 authority=diffemu */
/* Draws an untextured, alpha-blended quad with a left-to-right two-colour gradient.
 *  left, top, width, height: rectangle in 640x480 reference units, scaled to the current
 *    screen size (screen_width() / 640, screen_height() / 480; k_f_0_0015625 and
 *    k_f_0_0020833334 are 1/640 and 1/480).
 *  colour_left, colour_right: packed colours. The shift/mask expression below swaps bytes 0
 *    and 2 (red and blue for 0xAARRGGBB input) (inferred).
 *  Left edge (vertices 0, 2) uses colour_left, right edge (vertices 1, 3) uses colour_right.
 *  Fills g_im2d_vertices and submits it as a triangle strip. */
/* TIER2: validated 2026-10-19 authority=diffemu */
void gfx_draw_rect_2d_gradient_a(float left,float top,float width,float height,uint colour_left,uint colour_right)
{
  volatile float height_px;
  float left_px;
  float top_px;
  float width_px;
  RwGlobalsRelease *engine;
  short screen_px;
  RwReal *vertex_word;

  /* Render state: cull mode none first; the rest of the state is set before the draw below. */
  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLNONE);
  /* Scale from reference units to screen pixels: x and width by screen_width() / 640, y and height by screen_height() / 480. */
  screen_px = screen_width();
  left_px = (float)(int)screen_px * left * k_f_0_0015625;
  screen_px = screen_height();
  top_px = (float)(int)screen_px * top * k_f_0_0020833334;
  screen_px = screen_width();
  width_px = (float)(int)screen_px * width * k_f_0_0015625;
  screen_px = screen_height();
  engine = RwEngineInstance;
  height_px = (float)(int)screen_px * height * k_f_0_0020833334;
  /* Vertex layout (triangle strip order): 0 = top-left, 1 = top-right, 2 = bottom-left, 3 = bottom-right. */
  g_im2d_vertices[1].x = width_px + left_px;
  g_im2d_vertices[2].y = height_px + top_px;
  vertex_word = &g_im2d_vertices[0].z;
  g_im2d_vertices[0].x = left_px;
  g_im2d_vertices[0].y = top_px;
  g_im2d_vertices[1].y = top_px;
  g_im2d_vertices[2].x = left_px;
  g_im2d_vertices[3].x = g_im2d_vertices[1].x;
  g_im2d_vertices[3].y = g_im2d_vertices[2].y;
  /* Loop over the four vertices: set z (device near plane), rhw = 1 and the left colour.
     Vertices 1 and 3 get their colour overwritten below. */
  do {
    *(uint *)&vertex_word[2] = (((uint)((*(ushort *)((char *)&colour_left + 2)) >> 8) << 8 | colour_left & 0xff) << 8 | colour_left >> 8 & 0xff) << 8 |
                         (*(ushort *)((char *)&colour_left + 2)) & 0xff;
    *vertex_word = RwGlobalsRelease_dOpenDevice_zBufferNear(engine);
    vertex_word[1] = 1.0;
    vertex_word = vertex_word + IM2D_VERTEX_WORDS;
  } while ((int)vertex_word < ((int)&g_training_lesson));
  /* Right edge: vertices 1 and 3 get colour_right; vertex 3 copies vertex 1. */
  g_im2d_vertices[1].emissiveColor =
       (((uint)((*(ushort *)((char *)&colour_right + 2)) >> 8) << 8 | colour_right & 0xff) << 8 | colour_right >> 8 & 0xff) << 8 |
       (*(ushort *)((char *)&colour_right + 2)) & 0xff;
  g_im2d_vertices[3].emissiveColor = g_im2d_vertices[1].emissiveColor;
  /* No texture; vertex alpha on, blend with source alpha and inverse source alpha. */
  (*RwGlobalsRelease_dOpenDevice_fpRenderStateSet(engine))(rwRENDERSTATETEXTURERASTER,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)TRUE);
  RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDINVSRCALPHA);
  /* Submit the four vertices as a triangle strip. */
  RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP,g_im2d_vertices,IM2D_QUAD_VERTICES);
  return;
}


