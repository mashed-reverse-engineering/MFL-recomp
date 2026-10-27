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
  RwImVertexIndex line_indices [RIBBON_INDEX_COUNT] = {0,1,1,2,2,3,4,5};
  RwIm3DVertex v[RIBBON_VERTEX_COUNT];

  polygon_index = 0;
  if (polygon_index < course->ai_polygon_count) {
    polygon_floats = (RwReal *)((char *)course + AI_POLYGON_DATA_OFFSET);
    /* one pass per AI polygon record; polygon_floats starts at AI_POLYGON_DATA_OFFSET and steps by one record.
       Points: [-4..-2] -> pt1_*, sum_* = [-7..-5] + [-4..-2], [-1..1] -> im3d_verts position,
       [2..4] -> pt3_*, [5..7] -> pt4_*, [8..10] -> pt5_*. The point roles are (inferred). */
    do {
      v[5].objVertex.x = polygon_floats[-7] + polygon_floats[-4];
      v[0].objVertex.x = polygon_floats[-1];
      v[0].objVertex.y = polygon_floats[0];
      v[0].objVertex.z = polygon_floats[1];
      v[1].objVertex.x = polygon_floats[2];
      v[5].objVertex.y = polygon_floats[-6] + polygon_floats[-3];
      v[1].objVertex.y = polygon_floats[3];
      v[1].objVertex.z = polygon_floats[4];
      v[2].objVertex.x = polygon_floats[5];
      v[5].objVertex.z = polygon_floats[-5] + polygon_floats[-2];
      v[2].objVertex.y = polygon_floats[6];
      v[2].objVertex.z = polygon_floats[7];
      v[3].objVertex.x = polygon_floats[8];
      v[3].objVertex.y = polygon_floats[9];
      v[3].objVertex.z = polygon_floats[10];
      v[4].objVertex.x = polygon_floats[-4];
      v[4].objVertex.y = polygon_floats[-3];
      v[4].objVertex.z = polygon_floats[-2];
      v[0].color = 0xff40ff40;
      v[1].color = 0xff4040ff;
      v[2].color = 0xffffff40;
      v[3].color = 0xff40ffff;
      v[4].color = 0xff000000;
      v[5].color = 0xffffffff;
      RwRenderStateSet(rwRENDERSTATETEXTURERASTER,(void *)0x0);
      transformed = RwIm3DTransform(v,RIBBON_VERTEX_COUNT,(RwMatrix *)0x0,rwIM3D_ALLOPAQUE);
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
