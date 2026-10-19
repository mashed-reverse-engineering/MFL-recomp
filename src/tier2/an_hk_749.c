#include "mfl_tier2.h"
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d19d0*/ extern struct RwBBox * RwBBoxAddPoint(struct RwBBox *p0, struct RwV3d *p1);
/*004d3cd0*/ extern RwMatrix * RwMatrixMultiply(RwMatrix *p0, RwMatrix *p1, RwMatrix *p2);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

#define RWV3D_SIZE (0xc) /* sizeof(RwV3d): three RwReal floats */
/* geometry_transform_points */

/*
 * Transforms the vertices of an atomic's geometry into the space of the atomic's
 * parent frame and grows boundBox to contain them (inferred).
 *   atomic   - the atomic: +0x4 frame, +0x18 geometry, +0x3c parent clump (inferred)
 *   boundBox - bounding box that each transformed point is added to
 * Returns atomic.
 * (was: "allocates and transforms the vertices of a clump's geometry by its LTM (inferred)")
 */
int geometry_transform_points(int atomic,RwBBox *boundBox)

{
  RwV3d *transformed_points;
  RwMatrix *frame_ltm;
  RwMatrix *parent_ltm;
  int numPoints;
  RwV3d *vertex_cursor;
  RwMatrix parent_inverse_ltm;
  RwMatrix local_transform;

  numPoints = *(int *)(*(char **)(atomic + 0x18) + 0x14);
  vertex_cursor = *(RwV3d **)(*(char **)(*(int *)(atomic + 0x18) + 0x5c) + 0x14);
  transformed_points = RwMalloc(numPoints * RWV3D_SIZE,0);
  frame_ltm = RwFrameGetLTM(*(RwFrame **)(atomic + 4));
  parent_ltm = RwFrameGetLTM(*(RwFrame **)(*(char **)(atomic + 0x3c) + 4));
  RwMatrixInvert(&parent_inverse_ltm,parent_ltm);
  /* local = frame LTM * inverse(parent LTM): frame space -> parent space (inferred) */
  RwMatrixMultiply(&local_transform,frame_ltm,&parent_inverse_ltm);
  RwV3dTransformPoints(transformed_points,vertex_cursor,numPoints,&local_transform);
  vertex_cursor = transformed_points;
  /* add every transformed point to boundBox; vertex_cursor walks the output array */
  if (0 < numPoints) {
    do {
      RwBBoxAddPoint(boundBox,vertex_cursor);
      numPoints = numPoints + -1;
      vertex_cursor = vertex_cursor + 1;
    } while (numPoints != 0);
  }
  RwFree(transformed_points);
  return atomic;
}


