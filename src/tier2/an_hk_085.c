#include "mfl_tier2.h"
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/* float-index slots in shadow_plane that hold an RwV3d * to each of the two edge vertices (inferred) */
#define SHADOW_PLANE_VERT_A_SLOT (7)
#define SHADOW_PLANE_VERT_B_SLOT (8)
/* shadow_project_to_ground */

/* "builds the projection from the plane normal (cross product of two edges) with optional
   translation" */
/* Builds matrix as a shadow-projection basis onto the ground (inferred):
   right = normalized edge (vert_a - vert_b), at = shadow direction (shadow_plane[0..2]),
   up = at x edge, normalized. Then translates by `translation` (NULL means no offset). */

void shadow_project_to_ground(RwMatrix *matrix,float *shadow_plane,RwV3d *translation)

{
  RwV3d *vert_a;
  RwV3d *vert_b;
  RwV3d edge;
  RwV3d zero_offset;
  RwV3d up_vec;

  zero_offset.x = 0.0;
  zero_offset.y = 0.0;
  zero_offset.z = 0.0;
  /* no translation given: use the zero vector */
  if (translation == (RwV3d *)0x0) {
    translation = &zero_offset;
  }
  /* edge between the two vertices the plane record points to */
  vert_a = *(RwV3d **)&shadow_plane[SHADOW_PLANE_VERT_A_SLOT];
  vert_b = *(RwV3d **)&shadow_plane[SHADOW_PLANE_VERT_B_SLOT];
  edge.x = vert_a->x - vert_b->x;
  edge.y = vert_a->y - vert_b->y;
  edge.z = vert_a->z - vert_b->z;
  /* up = shadow direction x edge (cross product) */
  up_vec.x = edge.z * shadow_plane[1] - edge.y * shadow_plane[2];
  up_vec.y = edge.x * shadow_plane[2] - edge.z * *shadow_plane;
  up_vec.z = edge.y * *shadow_plane - edge.x * shadow_plane[1];
  RwV3dNormalize(&matrix->right,&edge);
  RwV3dNormalize(&matrix->up,&up_vec);
  /* at = shadow direction */
  (matrix->at).x = *shadow_plane;
  (matrix->at).y = shadow_plane[1];
  (matrix->at).z = shadow_plane[2];
  RwMatrixUpdate(matrix);
  RwMatrixTranslate(matrix,translation,rwCOMBINEPOSTCONCAT);
  return;
}


