#include "mfl_tier2.h"
/*004dc920*/ extern void * RwIm3DTransform(RwIm3DVertex *p0, RwUInt32 p1, RwMatrix *p2, RwUInt32 p3);
/*004dc9f0*/ extern RwBool RwIm3DEnd();
/*004dca20*/ extern RwBool __cdecl RwIm3DRenderIndexedPrimitive(enum RwPrimitiveType p0, RwImVertexIndex *p1, RwInt32 p2);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
#define RIBBON_VERTEX_COUNT (6)
#define RIBBON_INDEX_COUNT (8)
#define AI_POLYGON_RECORD_FLOATS (0x13) /* 19 floats = 0x4c bytes per AI polygon record */
#define AI_POLYGON_DATA_OFFSET (0xc1dc) /* (inferred) first float read from CourseLap */
/* course_ribbons_render */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "builds Im3D ribbon vertices (colour 0xff40ff40) for each course ribbon entry" */

/*
 * Draws the course ribbons. For each AI polygon record in the course (course->ai_polygon_count
 * of them), reads six points from the record's floats, builds six Im3D vertices from them and,
 * if RwIm3DTransform accepts them, draws them as a line list (RwIm3DRenderIndexedPrimitive).
 * course: the CourseLap whose AI polygon records are drawn. Returns nothing.
 */
void __cdecl course_ribbons_render(CourseLap *course)

{
  void *transformed;
  RwReal *polygon_floats;
  int polygon_index;
  RwImVertexIndex line_indices [RIBBON_INDEX_COUNT];
  RwIm3DVertex im3d_verts;
  RwReal pt3_x;
  RwReal pt3_y;
  RwReal pt3_z;
  undefined4 colour_1;
  RwReal pt4_x;
  RwReal pt4_y;
  RwReal pt4_z;
  undefined4 colour_2;
  RwReal pt5_x;
  RwReal pt5_y;
  RwReal pt5_z;
  undefined4 colour_3;
  RwReal pt1_x;
  RwReal pt1_y;
  RwReal pt1_z;
  undefined4 colour_4;
  float sum_x;
  float sum_y;
  float sum_z;
  undefined4 colour_5;

  line_indices[1] = 1;
  line_indices[2] = 1;
  line_indices[3] = 2;
  line_indices[4] = 2;
  line_indices[0] = 0;
  line_indices[5] = 3;
  line_indices[6] = 4;
  line_indices[7] = 5;
  polygon_index = 0;
  if (0 < course->ai_polygon_count) {
    polygon_floats = (RwReal *)((char *)course + AI_POLYGON_DATA_OFFSET);
    /* one pass per AI polygon record; polygon_floats starts at AI_POLYGON_DATA_OFFSET and steps by one record.
       Points: [-4..-2] -> pt1_*, sum_* = [-7..-5] + [-4..-2], [-1..1] -> im3d_verts position,
       [2..4] -> pt3_*, [5..7] -> pt4_*, [8..10] -> pt5_*. The point roles are (inferred). */
    do {
      im3d_verts.objVertex.x = polygon_floats[-1];
      sum_x = polygon_floats[-7] + polygon_floats[-4];
      im3d_verts.objVertex.y = *polygon_floats;
      im3d_verts.objVertex.z = polygon_floats[1];
      pt3_x = polygon_floats[2];
      sum_y = polygon_floats[-6] + polygon_floats[-3];
      pt3_y = polygon_floats[3];
      pt3_z = polygon_floats[4];
      pt4_x = polygon_floats[5];
      sum_z = polygon_floats[-5] + polygon_floats[-2];
      pt4_y = polygon_floats[6];
      pt4_z = polygon_floats[7];
      pt5_x = polygon_floats[8];
      pt5_y = polygon_floats[9];
      pt5_z = polygon_floats[10];
      pt1_x = polygon_floats[-4];
      pt1_y = polygon_floats[-3];
      pt1_z = polygon_floats[-2];
      /* vertex colours (ARGB); im3d_verts.color is the first vertex, colour_1..colour_5 the others (inferred order) */
      im3d_verts.color = 0xff40ff40;
      colour_1 = 0xff4040ff;
      colour_2 = 0xffffff40;
      colour_3 = 0xff40ffff;
      colour_4 = 0xff000000;
      colour_5 = 0xffffffff;
      RwRenderStateSet(rwRENDERSTATETEXTURERASTER,(void *)0x0);
      transformed = RwIm3DTransform(&im3d_verts,RIBBON_VERTEX_COUNT,(RwMatrix *)0x0,rwIM3D_ALLOPAQUE);
      /* draw only when the transform returned a vertex buffer (inferred: NULL means the vertices were rejected) */
      if (transformed != (void *)0x0) {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST,line_indices,RIBBON_INDEX_COUNT);
        RwIm3DEnd();
      }
      polygon_index = polygon_index + 1;
      polygon_floats = polygon_floats + AI_POLYGON_RECORD_FLOATS;
    } while (polygon_index < course->ai_polygon_count);
  }
  return;
}


