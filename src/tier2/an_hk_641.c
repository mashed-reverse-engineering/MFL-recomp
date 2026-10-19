#include "mfl_tier2.h"
/*004dc920*/ extern void * RwIm3DTransform(RwIm3DVertex *p0, RwUInt32 p1, RwMatrix *p2, RwUInt32 p3);
/*004dc9f0*/ extern RwBool RwIm3DEnd();
/*004dca20*/ extern RwBool __cdecl RwIm3DRenderIndexedPrimitive(enum RwPrimitiveType p0, RwImVertexIndex *p1, RwInt32 p2);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
#define BOX_CORNER_COUNT (8)
#define BOX_EDGE_INDEX_COUNT (24) /* 12 edges, 2 indices each */
/* debug_box_draw_with_culling_b */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "same AABB edge drawing without a frame (world space)" */

/* Draws one axis-aligned box (AABB) as 12 wireframe edges in world space (no frame matrix), in colour rgb, over the scene (depth test and write off).
 * unused_arg: not read.
 * box_bounds: two opposite corners, min x,y,z in [0..2] and max x,y,z in [3..5] (inferred from the corner table below).
 * rgb: 3 bytes, red, green, blue; the colour is stored opaque (alpha 0xff).
 * RwIm3DTransform returns NULL when the corners cannot be transformed; nothing is drawn then (inferred: this is the culling, as the name says).
 * Corner numbering, bit set = min on that axis (bit0 = x, bit1 = y, bit2 = z):
 *   corner0 (max,max,max)  corner1 (min,max,max)  corner2 (max,min,max)  corner3 (min,min,max)
 *   corner4 (max,max,min)  corner5 (min,max,min)  corner6 (max,min,min)  corner7 (min,min,min)
 */
void debug_box_draw_with_culling_b(char *unused_arg,undefined4 *box_bounds,byte *rgb)

{
  void *transform_result;
  RwImVertexIndex edge_indices [BOX_EDGE_INDEX_COUNT];
  RwIm3DVertex corner_vertices; /* corner0 position and colour; corners 1..7 follow in memory */
  /* corner1..7 x/y/z and colour scalars: inferred to be the fields of corner_vertices[1..7] (stack offsets) */
  undefined4 corner1_x;
  undefined4 corner1_y;
  undefined4 corner1_z;
  uint corner1_color;
  undefined4 corner2_x;
  undefined4 corner2_y;
  undefined4 corner2_z;
  uint corner2_color;
  undefined4 corner3_x;
  undefined4 corner3_y;
  undefined4 corner3_z;
  uint corner3_color;
  undefined4 corner4_x;
  undefined4 corner4_y;
  undefined4 corner4_z;
  uint corner4_color;
  undefined4 corner5_x;
  undefined4 corner5_y;
  undefined4 corner5_z;
  uint corner5_color;
  undefined4 corner6_x;
  undefined4 corner6_y;
  undefined4 corner6_z;
  uint corner6_color;
  undefined4 corner7_x;
  undefined4 corner7_y;
  undefined4 corner7_z;
  uint corner7_color;

  /* edge list: 12 cube edges as index pairs (written in two halves): 0-1 1-3 3-2 2-0 | 4-5 5-7 7-6 6-4 | 0-4 1-5 2-6 3-7 */
  edge_indices[5] = 2;
  edge_indices[6] = 2;
  edge_indices[8] = 4;
  edge_indices[0xf] = 4;
  edge_indices[0x11] = 4;
  edge_indices[9] = 5;
  edge_indices[10] = 5;
  edge_indices[0x13] = 5;
  edge_indices[3] = 3;
  edge_indices[4] = 3;
  edge_indices[0x16] = 3;
  /* pack rgb into one colour word (opaque) and fill corner0 */
  corner_vertices.color = ((*rgb | 0xffffff00) << 8 | (uint)rgb[1]) << 8 | (uint)rgb[2];
  corner_vertices.objVertex.x = *(float *)&box_bounds[3];
  corner_vertices.objVertex.y = *(float *)&box_bounds[4];
  corner_vertices.objVertex.z = *(float *)&box_bounds[5];
  corner1_x = *box_bounds;
  corner1_y = box_bounds[4];
  corner1_z = box_bounds[5];
  corner2_x = box_bounds[3];
  corner2_y = box_bounds[1];
  corner2_z = box_bounds[5];
  corner3_x = *box_bounds;
  corner3_y = box_bounds[1];
  corner3_z = box_bounds[5];
  corner4_x = box_bounds[3];
  corner4_y = box_bounds[4];
  corner4_z = box_bounds[2];
  corner5_x = *box_bounds;
  corner5_y = box_bounds[4];
  corner5_z = box_bounds[2];
  edge_indices[0] = 0;
  edge_indices[1] = 1;
  edge_indices[2] = 1;
  edge_indices[7] = 0;
  edge_indices[0xb] = 7;
  edge_indices[0xc] = 7;
  edge_indices[0xd] = 6;
  edge_indices[0xe] = 6;
  edge_indices[0x10] = 0;
  edge_indices[0x12] = 1;
  edge_indices[0x14] = 2;
  edge_indices[0x15] = 6;
  edge_indices[0x17] = 7;
  corner6_x = box_bounds[3];
  corner6_y = box_bounds[1];
  corner6_z = box_bounds[2];
  corner7_x = *box_bounds;
  corner7_y = box_bounds[1];
  corner7_z = box_bounds[2];
  corner1_color = corner_vertices.color;
  corner2_color = corner_vertices.color;
  corner3_color = corner_vertices.color;
  corner4_color = corner_vertices.color;
  corner5_color = corner_vertices.color;
  corner6_color = corner_vertices.color;
  corner7_color = corner_vertices.color;
  RwRenderStateSet(rwRENDERSTATESHADEMODE,(void *)rwSHADEMODEFLAT);
  /* no texture; depth test and depth write off so the box draws over the scene */
  RwRenderStateSet(rwRENDERSTATETEXTURERASTER,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
  /* NULL matrix: corners stay in world space; NULL result means nothing to draw */
  transform_result = RwIm3DTransform(&corner_vertices,BOX_CORNER_COUNT,(RwMatrix *)0x0,rwIM3D_ALLOPAQUE);
  if (transform_result != (void *)0x0) {
    RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST,edge_indices,BOX_EDGE_INDEX_COUNT);
    RwIm3DEnd();
  }
  /* restore depth write/test and shade mode for the rest of the frame (gouraud restore is inferred) */
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATESHADEMODE,(void *)rwSHADEMODEGOURAUD);
  return;
}
