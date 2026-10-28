#include "mfl_tier2.h"
/*004d18d0*/ extern struct RwBBox * RwBBoxCalculate(struct RwBBox *p0, struct RwV3d *p1, RwInt32 p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d4e70*/ extern RwMatrix * RwMatrixCreate();
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*005de25c*/ extern float k_f_0_005;
/*0072dfc4*/ extern RwMatrix *g_course_create_72dfc4;
/*008885c0*/ extern float g_course_create_8885c0;
/*008885c4*/ extern float g_course_create_8885c4;
/*008885c8*/ extern float g_course_create_8885c8;
/*008885cc*/ extern float g_particles_8885cc;
/*008885d0*/ extern float g_course_create_8885d0;
/*008885d4*/ extern float g_course_create_8885d4;
/*008885e0*/ extern undefined4 g_course_create_8885e0;
#define RWBBOX_FLOAT_COUNT (6)
#define BBOX_CORNER_COUNT (8)
#define MATRIX_ELEMENT_COUNT (0x10)
#define OBJ_LOCAL_BBOX_OFFSET (0x438)
#define OBJ_WORLD_BBOX_OFFSET (0x450)
#define OBJ_MATRIX_PTR_OFFSET (0x478)
/* smplfzx_object_bounds_update */

/* "computes the world AABB of an object from the local box and matrix plus margin 2*param+0.005"
 *
 * obj:    object base pointer; local bbox at +0x438 (RwBBox), world bbox written to +0x450 (RwBBox),
 *         transform matrix pointer at +0x478, 8 output vec4 corners at +0x300..+0x37c.
 * radius: half-size parameter (inferred); the bbox is grown by margin = 2*radius + 0.005.
 *
 * Returns nothing. Writes the world bbox, the scratch matrix and the globals g_course_create_8885c0..d4 and g_particles_8885cc. */

/* collision object: the 8 corners of the local box as vec4 (xyz + radius), local and world bounding boxes, matrix */
typedef struct SmplfzxObject {
    char unknown_000[0x300];
    struct { float x, y, z, radius; } corner[8]; /* 0x300 */
    char unknown_380[0xb8];
    RwBBox local_bbox;                           /* 0x438 */
    RwBBox world_bbox;                           /* 0x450 */
    char unknown_468[0x10];
    RwMatrix *matrix;                            /* 0x478 */
} SmplfzxObject;

void smplfzx_object_bounds_update(SmplfzxObject *obj,float radius)

{
  RwBBox *world_bbox;
  int i;
  RwReal *float_cursor;
  RwMatrix *mtx;
  RwReal *frame_cursor;
  struct {
    RwReal bbox_f [4];
    undefined4 bbox_f4;
    undefined4 bbox_f5;
    RwV3d corners_local;
    RwReal corner1_x;
    RwReal corner1_y;
    RwReal corner1_z;
    RwReal corner2_x;
    RwReal corner2_y;
    undefined4 corner2_z;
    RwReal corner3_x;
    RwReal corner3_y;
    undefined4 corner3_z;
    RwReal corner4_x;
    undefined4 corner4_y;
    RwReal corner4_z;
    RwReal corner5_x;
    undefined4 corner5_y;
    RwReal corner5_z;
    RwReal corner6_x;
    undefined4 corner6_y;
    undefined4 corner6_z;
    RwReal corner7_x;
    undefined4 corner7_y;
    undefined4 corner7_z;
    RwV3d corners_world [8];
  } frame;
  
  /* copy the 6 floats of the local bbox (sup, inf) into the frame scratch */
  float_cursor = (RwReal *)&obj->local_bbox;
  frame_cursor = frame.bbox_f;
  for (i = RWBBOX_FLOAT_COUNT; i != 0; i = i + -1) {
    *frame_cursor = *float_cursor;
    float_cursor = float_cursor + 1;
    frame_cursor = frame_cursor + 1;
  }
  /* lazily create the shared scratch matrix, then reset it to identity (right = x, up = y, at = z, pos = 0) */
  if (g_course_create_72dfc4 == (RwMatrix *)0x0) {
    g_course_create_72dfc4 = RwMatrixCreate();
  }
  mtx = g_course_create_72dfc4;
  (g_course_create_72dfc4->at).z = 1.0;
  (mtx->up).y = 1.0;
  (mtx->right).x = 1.0;
  (mtx->up).x = 0.0;
  (mtx->right).z = 0.0;
  (mtx->right).y = 0.0;
  (mtx->at).y = 0.0;
  (mtx->at).x = 0.0;
  (mtx->up).z = 0.0;
  (mtx->pos).z = 0.0;
  (mtx->pos).y = 0.0;
  (mtx->pos).x = 0.0;
  mtx->flags = mtx->flags | 0x20003;
  frame.corner6_x = frame.bbox_f[3];
  frame.corner6_z = frame.bbox_f5;
  frame.corner7_y = frame.bbox_f4;
  frame.corner6_y = frame.bbox_f4;
  frame.corner3_x = frame.bbox_f[0];
  frame.corner7_x = frame.bbox_f[0];
  frame.corner3_z = frame.bbox_f5;
  frame.corner7_z = frame.bbox_f5;
  frame.corner2_y = frame.bbox_f[1];
  frame.corner3_y = frame.bbox_f[1];
  frame.corner4_x = frame.bbox_f[3];
  frame.corner2_x = frame.bbox_f[3];
  frame.corner4_z = frame.bbox_f[2];
  frame.corner2_z = frame.bbox_f5;
  frame.corner5_y = frame.bbox_f4;
  frame.corner4_y = frame.bbox_f4;
  frame.corner1_x = frame.bbox_f[0];
  frame.corner5_x = frame.bbox_f[0];
  frame.corner1_z = frame.bbox_f[2];
  frame.corner5_z = frame.bbox_f[2];
  frame.corners_local.y = frame.bbox_f[1];
  frame.corner1_y = frame.bbox_f[1];
  frame.corners_local.x = frame.bbox_f[3];
  frame.corners_local.z = frame.bbox_f[2];
  /* place the scratch matrix at the translation of the object's matrix (+0x30 of the matrix pointer) */
  /* frame.corners_local .. corner7_z: the 8 local bbox corners as 24 consecutive floats (corner0 = corners_local) */
  RwMatrixTranslate(mtx,&obj->matrix->pos,rwCOMBINEREPLACE);
  /* margin = 2*radius + 0.005, kept in the frame's bbox slot */
  frame.bbox_f[2] = radius + radius + k_f_0_005;
  /* transform the 8 local corners of the local bbox into world space */
  RwV3dTransformPoints(frame.corners_world,&frame.corners_local,BBOX_CORNER_COUNT,obj->matrix);
  /* world bbox = bound of the 8 world corners, then grow it by the margin (sup += margin, inf -= margin) */
  world_bbox = &obj->world_bbox;
  RwBBoxCalculate(world_bbox,frame.corners_world,BBOX_CORNER_COUNT);
  (world_bbox->sup).x = frame.bbox_f[2] + (world_bbox->sup).x;
  obj->world_bbox.sup.y = frame.bbox_f[2] + obj->world_bbox.sup.y;
  obj->world_bbox.sup.z = frame.bbox_f[2] + obj->world_bbox.sup.z;
  obj->world_bbox.inf.x = obj->world_bbox.inf.x - frame.bbox_f[2];
  obj->world_bbox.inf.y = obj->world_bbox.inf.y - frame.bbox_f[2];
  obj->world_bbox.inf.z = obj->world_bbox.inf.z - frame.bbox_f[2];
  /* publish the world bbox min/max relative to the scratch matrix position */
  g_particles_8885cc = obj->world_bbox.inf.x - (g_course_create_72dfc4->pos).x;
  g_course_create_8885d0 = obj->world_bbox.inf.y - (g_course_create_72dfc4->pos).y;
  g_course_create_8885d4 = obj->world_bbox.inf.z - (g_course_create_72dfc4->pos).z;
  g_course_create_8885c0 = (world_bbox->sup).x - (g_course_create_72dfc4->pos).x;
  g_course_create_8885c4 = obj->world_bbox.sup.y - (g_course_create_72dfc4->pos).y;
  g_course_create_8885c8 = obj->world_bbox.sup.z - (g_course_create_72dfc4->pos).z;
  /* copy the 16 floats of the scratch matrix (right, up, at, pos) into g_course_create_8885e0 */
  mtx = g_course_create_72dfc4;
  float_cursor = (RwReal *)&g_course_create_8885e0;
  for (i = MATRIX_ELEMENT_COUNT; i != 0; i = i + -1) {
    *float_cursor = (mtx->right).x;
    mtx = (RwMatrix *)&(mtx->right).y;
    float_cursor = float_cursor + 1;
  }
  /* transform the frame's corner points in place (inferred) */
  RwV3dTransformPoints(&frame.corners_local,&frame.corners_local,BBOX_CORNER_COUNT,obj->matrix);
  /* write the 8 corners as vec4 entries (xyz + radius) at obj+0x300..0x37c, 16 bytes each (inferred) */
  obj->corner[6].x = frame.corner6_x;
  *(undefined4 *)&obj->corner[6].y = frame.corner6_y;
  *(undefined4 *)&obj->corner[6].z = frame.corner6_z;
  obj->corner[6].radius = radius;
  obj->corner[7].x = frame.corner7_x;
  *(undefined4 *)&obj->corner[7].y = frame.corner7_y;
  *(undefined4 *)&obj->corner[7].z = frame.corner7_z;
  obj->corner[7].radius = radius;
  obj->corner[3].x = frame.corner3_x;
  obj->corner[3].y = frame.corner3_y;
  *(undefined4 *)&obj->corner[3].z = frame.corner3_z;
  obj->corner[3].radius = radius;
  obj->corner[2].x = frame.corner2_x;
  obj->corner[2].y = frame.corner2_y;
  *(undefined4 *)&obj->corner[2].z = frame.corner2_z;
  obj->corner[2].radius = radius;
  obj->corner[4].x = frame.corner4_x;
  *(undefined4 *)&obj->corner[4].y = frame.corner4_y;
  obj->corner[4].z = frame.corner4_z;
  obj->corner[4].radius = radius;
  obj->corner[5].x = frame.corner5_x;
  *(undefined4 *)&obj->corner[5].y = frame.corner5_y;
  obj->corner[5].z = frame.corner5_z;
  obj->corner[5].radius = radius;
  obj->corner[1].x = frame.corner1_x;
  obj->corner[1].y = frame.corner1_y;
  obj->corner[1].z = frame.corner1_z;
  obj->corner[1].radius = radius;
  obj->corner[0].x = frame.corners_local.x;
  obj->corner[0].y = frame.corners_local.y;
  obj->corner[0].radius = radius;
  obj->corner[0].z = frame.corners_local.z;
  return;
}


