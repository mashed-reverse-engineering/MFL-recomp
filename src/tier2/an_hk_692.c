#include "mfl_tier2.h"
/*00430ab0*/ extern int screen_width();
/*00430ac0*/ extern int screen_height();
/*005dd624*/ extern float k_f_0_0020833334;
/*005de7c0*/ extern float k_f_0_0015625;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008c8000*/ extern RwIm2DVertex g_im2d_vertices[4];
/*008c8078*/ extern char g_training_lesson[];

#define RECT_VERTEX_COUNT (4)

/* gfx_draw_rect_2d_gradient_b */

/* "640x480 rect with two colours (second on vertices 0,2)" */

/* TIER2: unvalidated 2026-10-17 authority=diffemu */
/*
 * Draws a filled 2D rectangle as a 4-vertex triangle strip (inferred), in
 * g_im2d_vertices, with a two-colour gradient.
 *
 * x, y, width, height: rectangle in 640x480 reference units, scaled to the
 *   current screen size (screen_width / screen_height).
 * colour_primary: colour of the right-hand vertices (1 and 3) and of the
 *   default; colour_secondary: colour of the left-hand vertices (0 and 2).
 *   Both are packed as 0xAABBGGRR (inferred from the byte shuffle) and are
 *   stored as 0xAARRGGBB.
 *
 * Sets render states (cull off, no texture, vertex alpha on, src alpha /
 * inverse src alpha blend) and leaves them set. Returns nothing.
 */
/* TIER2: validated 2026-10-19 authority=shadow */
void gfx_draw_rect_2d_gradient_b(float x,float y,float width,float height,uint colour_primary,uint colour_secondary)
{
  volatile float scaled_height;
  float left;
  float top;
  float scaled_width;
  RwGlobalsRelease *engine;
  short screen_extent;
  RwReal *vertex_z;

  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLNONE);
  screen_extent = screen_width();
  left = (float)(int)screen_extent * x * k_f_0_0015625;
  screen_extent = screen_height();
  top = (float)(int)screen_extent * y * k_f_0_0020833334;
  screen_extent = screen_width();
  scaled_width = (float)(int)screen_extent * width * k_f_0_0015625;
  screen_extent = screen_height();
  engine = RwEngineInstance;
  scaled_height = (float)(int)screen_extent * height * k_f_0_0020833334;
  g_im2d_vertices[1].x = scaled_width + left;
  g_im2d_vertices[2].y = scaled_height + top;
  vertex_z = &g_im2d_vertices[0].z;
  g_im2d_vertices[0].x = left;
  g_im2d_vertices[0].y = top;
  g_im2d_vertices[1].y = top;
  g_im2d_vertices[2].x = left;
  g_im2d_vertices[3].x = g_im2d_vertices[1].x;
  g_im2d_vertices[3].y = g_im2d_vertices[2].y;
  /* Initialise all 4 vertices: colour_primary (converted to ARGB), z = near
     plane, rhw = 1 (no perspective). Each RwD3D9Vertex is 7 words; the loop
     ends at g_training_lesson's address, just past vertex 3 (inferred). */
  do {
    *(uint *)&vertex_z[2] = (((uint)((*(ushort *)((char *)&colour_primary + 2)) >> 8) << 8 | colour_primary & 0xff) << 8 |
                         colour_primary >> 8 & 0xff) << 8 | (*(ushort *)((char *)&colour_primary + 2)) & 0xff;
    *vertex_z = RwGlobalsRelease_dOpenDevice_zBufferNear(engine);
    vertex_z[1] = 1.0;
    vertex_z = vertex_z + 7;
  } while ((int)vertex_z < ((int)&g_training_lesson));
  /* Second colour on the left-hand vertices 0 and 2, converted to ARGB the same way. */
  g_im2d_vertices[0].emissiveColor =
       (((uint)((*(ushort *)((char *)&colour_secondary + 2)) >> 8) << 8 | colour_secondary & 0xff) << 8 | colour_secondary >> 8 & 0xff) << 8 |
       (*(ushort *)((char *)&colour_secondary + 2)) & 0xff;
  g_im2d_vertices[2].emissiveColor = g_im2d_vertices[0].emissiveColor;
  /* Untextured, alpha-blended draw. */
  (*RwGlobalsRelease_dOpenDevice_fpRenderStateSet(engine))(rwRENDERSTATETEXTURERASTER,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDINVSRCALPHA);
  RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP,g_im2d_vertices,RECT_VERTEX_COUNT);
  return;
}


