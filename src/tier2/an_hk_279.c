#include "mfl_tier2.h"
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/* vector_normalize_copy */

/* "(out, point, in_dir): copies xyz and normalises" */

/* Builds a plane (inferred) from a point and a direction.
 * out[0] receives the unit normal of in_dir; out[1].x receives the plane
 * offset d = -dot(normal, point), so that dot(normal, p) + d = 0 for points on the plane.
 * point: 3 floats (inferred: a point on the plane), in_dir: 3 floats, need not be unit length.
 * Returns nothing. */
void __cdecl vector_normalize_copy(RwV3d *out,float *point,RwReal *in_dir)

{
  out->x = *in_dir;
  out->y = in_dir[1];
  out->z = in_dir[2];
  RwV3dNormalize(out,out);
  out[1].x = -((out->y * point[1] + out->x * *point) + out->z * point[2]);
  return;
}


