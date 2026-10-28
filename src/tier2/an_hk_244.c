#include "mfl_tier2.h"
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*004dc920*/ extern void * RwIm3DTransform(RwIm3DVertex *p0, RwUInt32 p1, RwMatrix *p2, RwUInt32 p3);
/*004dc9f0*/ extern RwBool RwIm3DEnd();
/*004dcb80*/ extern RwBool RwIm3DRenderPrimitive(enum RwPrimitiveType p0);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

#define QUAD_VERTEX_COUNT (4)
#define OBJ_POINTS_OFFSET (0x74)
#define OBJ_MATRIX_OFFSET (0x34)
#define OBJ_TEXTURE_PTR_OFFSET (0xa4)
#define OBJ_TEXTURED_FLAG_OFFSET (0x118)
#define UV_TABLE_HALF_BYTES (0x10)

/* course_quad_render_a */

/* "transforms 4 points and draws a textured Im3D triangle strip with the object's texture"
 *
 * Draws one quad for a course object. The object's 4 corner points (obj + OBJ_POINTS_OFFSET) are transformed by the
 * object matrix (obj + OBJ_MATRIX_OFFSET) into camera space, copied into 4 Im3D vertices together with a texture
 * coordinate per corner, and submitted as a triangle strip.
 *
 * obj: course object pointer. The offsets used on it are inferred from the call sites, not from a struct.
 * Returns nothing. Nothing is drawn if RwIm3DTransform fails (returns NULL).
 */

void course_quad_render_a(char *obj)

{
  undefined4 uv_component;
  RwReal coord;
  RwReal *vertex_out;
  void *transform_result;
  RwReal *point_in;
  int byte_offset;
  undefined4 quad_uv_table [8];
  RwV3d transformed_points [QUAD_VERTEX_COUNT];
  RwIm3DVertex im3d_verts [QUAD_VERTEX_COUNT];

  /* quad_uv_table: entries 0-3 are the U of each corner, entries 4-7 the V (inferred).
     Corners in strip order: (1,0) (1,1) (0,0) (0,1) (inferred from the values). */
  quad_uv_table[4] = 0;
  quad_uv_table[5] = FLOAT_ONE_BITS;
  quad_uv_table[6] = 0;
  quad_uv_table[7] = FLOAT_ONE_BITS;
  quad_uv_table[0] = FLOAT_ONE_BITS;
  quad_uv_table[1] = FLOAT_ONE_BITS;
  quad_uv_table[2] = 0;
  quad_uv_table[3] = 0;
  RwV3dTransformPoints(transformed_points,(RwV3d *)(obj + OBJ_POINTS_OFFSET),QUAD_VERTEX_COUNT,(RwMatrix *)(obj + OBJ_MATRIX_OFFSET));
  vertex_out = &im3d_verts[0].objVertex.z;
  point_in = &transformed_points[0].y;
  byte_offset = 0;
  /* one pass per corner: position from transformed_points, then the U/V pair from quad_uv_table.
     The constant colour word is written as a float literal by the decompiler; its value is not identified. */
  do {
    uv_component = *(undefined4 *)((LONG_PTR)quad_uv_table + byte_offset + UV_TABLE_HALF_BYTES);
    ((RwV3d *)(vertex_out + -2))->x = ((RwV3d *)(point_in + -1))->x;
    coord = *point_in;
    vertex_out[5] = (RwReal)uv_component;
    uv_component = *(undefined4 *)((LONG_PTR)quad_uv_table + byte_offset);
    vertex_out[-1] = coord;
    coord = point_in[1];
    vertex_out[6] = (RwReal)uv_component;
    *vertex_out = coord;
    vertex_out[4] = 4.152326e+19;
    byte_offset = byte_offset + 4;
    vertex_out = vertex_out + 9;
    point_in = point_in + 3;
  } while (byte_offset < UV_TABLE_HALF_BYTES);
  /* bind the object's texture if it has one (flag inferred), otherwise bind no raster */
  if (*(int *)(obj + OBJ_TEXTURED_FLAG_OFFSET) == 0) {
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,(void *)0x0);
  }
  else {
    RwRenderStateSet
              (rwRENDERSTATETEXTURERASTER,**(void ***)(obj + OBJ_TEXTURE_PTR_OFFSET));
  }
  /* enable vertex alpha for the quad */
  RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x1);
  transform_result = RwIm3DTransform(im3d_verts,QUAD_VERTEX_COUNT,(RwMatrix *)0x0,1);
  /* draw the strip only if the Im3D transform succeeded */
  if (transform_result != (void *)0x0) {
    RwIm3DRenderPrimitive(rwPRIMTYPETRISTRIP);
    RwIm3DEnd();
    return;
  }
  return;
}


