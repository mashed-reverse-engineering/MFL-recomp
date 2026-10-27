#include "mfl_tier2.h"
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*00843ea0*/ extern undefined4 g_collision_triangles;
/*00843ea4*/ extern undefined4 g_trail_843ea4;
/*00843ea8*/ extern undefined4 g_camera_843ea8;
/*00843ebc*/ extern undefined4 g_trail_843ebc;
/*00843ec0*/ extern undefined4 g_trail_843ec0;
/*00843ed8*/ extern undefined4 g_trail_843ed8;
/*00843edc*/ extern undefined4 g_trail_843edc;
/*00843efc*/ extern char g_trail_843efc[];

/* RwD3D9Vertex is 7 words (x, y, z, rhw, emissiveColor, u, v) */
#define IM2D_VERTEX_WORDS (7)
/* vertices and indices per triangle, passed to fpIm2DRenderIndexedPrimitive */
#define TRI_VERTEX_COUNT (3)
#define TRI_INDEX_COUNT (3)
/* collision_debug_draw */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "draws collision triangles / trails (debug lines)" */

/* Draws one debug triangle with depth test and depth write off, then restores both.
 * x0,y0 / x1,y1 / x2,y2: the three vertex positions (screen space, inferred); they are stored
 * into the vertex array at g_collision_triangles (x, y word of each vertex).
 * colour: packed 32 bit colour; the low three bytes are reversed, R and B swapped (inferred).
 * Each vertex also gets z = zBufferNear and rhw = 1.0 (inferred). Returns nothing. */
void collision_debug_draw(undefined4 x0,undefined4 y0,undefined4 x1,undefined4 y1,
               undefined4 x2,undefined4 y2,uint colour)
{
  RwGlobalsRelease *rw_globals;
  RwReal *vertex;
  RwImVertexIndex indices [4];

  rw_globals = RwEngineInstance;
  g_trail_843ea4 = y0;
  g_collision_triangles = x0;
  g_trail_843ed8 = x2;
  g_trail_843ebc = x1;
  g_trail_843ec0 = y1;
  g_trail_843edc = y2;
  indices[0] = 0;
  indices[1] = 1;
  indices[2] = 2;
  vertex = (RwReal *)&g_camera_843ea8;
  /* per vertex, from the z word (g_camera_843ea8) up to g_trail_843efc: z, rhw, colour */
  do {
    *(uint *)&vertex[2] = (((colour >> 0x18) << 8 | colour & 0xff) << 8 | colour >> 8 & 0xff) << 8
                        | colour >> 0x10 & 0xff;
    *vertex = RwGlobalsRelease_dOpenDevice_zBufferNear(rw_globals);
    vertex[1] = 1.0;
    vertex = vertex + IM2D_VERTEX_WORDS;
  } while ((char *)vertex < ((char *)&g_trail_843efc));
  /* no texture, Gouraud shade mode, depth test and depth write off for the draw */
  (*RwGlobalsRelease_dOpenDevice_fpRenderStateSet(rw_globals))(rwRENDERSTATETEXTURERASTER,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATESHADEMODE,(void *)rwSHADEMODEGOURAUD);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  RwIm2DRenderIndexedPrimitive
            (rwPRIMTYPETRILIST,(RwIm2DVertex *)&g_collision_triangles,TRI_VERTEX_COUNT,indices,TRI_INDEX_COUNT);
  /* restore depth test and depth write */
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  return;
}


