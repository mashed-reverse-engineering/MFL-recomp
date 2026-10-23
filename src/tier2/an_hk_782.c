#include "mfl_tier2.h"
/*00430ab0*/ extern int screen_width();
/*00430ac0*/ extern int screen_height();
/*005dd624*/ extern float k_f_0_0020833334;
/*005de7c0*/ extern float k_f_0_0015625;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008c8000*/ extern RwIm2DVertex g_im2d_vertices[4];
/*008c8078*/ extern char g_training_lesson[];
#define RECT_VERTEX_COUNT (4)
#define EMISSIVE_OPAQUE_BLACK (0xff000000)
/* gfx_draw_rect_2d_flat_b */

/* "640x480 rect with one colour, emissive vertex 2/3 cleared" */

/* Draws a flat, untextured, alpha-blended quad immediately with the RenderWare im2d primitive call.
   x, y: top-left corner; width, height: size. All four are in 640x480 reference units and are
   scaled to the real screen size. colour: packed 32-bit colour (channels rotated before storing,
   inferred). Returns nothing. Vertex order is top-left, top-right, bottom-left, bottom-right. */
void gfx_draw_rect_2d_flat_b(float x,float y,float width,float height,uint colour)

{
  volatile float height_px;
  float left_px;
  float top_px;
  float width_px;
  RwGlobalsRelease *engine;
  short screen_dim;
  RwReal *vertex_z;

  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLNONE);
  screen_dim = screen_width();
  left_px = (float)(int)screen_dim * x * k_f_0_0015625;
  screen_dim = screen_height();
  top_px = (float)(int)screen_dim * y * k_f_0_0020833334;
  screen_dim = screen_width();
  width_px = (float)(int)screen_dim * width * k_f_0_0015625;
  screen_dim = screen_height();
  engine = RwEngineInstance;
  height_px = (float)(int)screen_dim * height * k_f_0_0020833334;
  g_im2d_vertices[1].x = width_px + left_px;
  g_im2d_vertices[2].y = height_px + top_px;
  vertex_z = &g_im2d_vertices[0].z;
  g_im2d_vertices[0].x = left_px;
  g_im2d_vertices[0].y = top_px;
  g_im2d_vertices[1].y = top_px;
  g_im2d_vertices[2].x = left_px;
  g_im2d_vertices[3].x = g_im2d_vertices[1].x;
  g_im2d_vertices[3].y = g_im2d_vertices[2].y;
  /* Write z, rhw and colour for all 4 vertices (the loop walks the 28-byte vertices up to g_training_lesson) */
  do {
    *(uint *)&vertex_z[2] = (((uint)((*(ushort *)((char *)&colour + 2)) >> 8) << 8 | colour & 0xff) << 8 |
                         colour >> 8 & 0xff) << 8 | (*(ushort *)((char *)&colour + 2)) & 0xff;
    *vertex_z = RwGlobalsRelease_dOpenDevice_zBufferNear(engine);
    vertex_z[1] = 1.0;
    vertex_z = vertex_z + 7;
  } while ((char *)vertex_z < ((char *)&g_training_lesson));
  /* Vertices 2 and 3 are overwritten with opaque black emissive colour */
  g_im2d_vertices[2].emissiveColor = EMISSIVE_OPAQUE_BLACK;
  g_im2d_vertices[3].emissiveColor = EMISSIVE_OPAQUE_BLACK;
  /* Render state: no texture, vertex alpha on, src-alpha / inverse-src-alpha blending */
  (*RwGlobalsRelease_dOpenDevice_fpRenderStateSet(engine))(rwRENDERSTATETEXTURERASTER,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDINVSRCALPHA);
  RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP,g_im2d_vertices,RECT_VERTEX_COUNT);
  return;
}


