#include "mfl_tier2.h"
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005ddc28*/ extern float k_f_0;
#define PLANE_CLASS_GREATER (0)
#define PLANE_CLASS_EQUAL (1)
#define PLANE_CLASS_LESS (2)
/* plane_classify_point */

/*
 * Classifies a point against a plane given as (normal.x, normal.y, normal.z, d).
 * Signed distance = dot(normal, point) + d.
 * plane: 4 floats; point: xyz; out_distance: optional (NULL to skip), receives
 * the distance from point to its projection onto the plane (|d| for a unit normal, inferred).
 * Returns PLANE_CLASS_GREATER if the signed distance is above k_f_0, PLANE_CLASS_EQUAL if equal,
 * PLANE_CLASS_LESS if below. The sign convention of the normal is not known here (inferred).
 * (was: "(plane, point, out dist): distance to the plane and 0 behind / 1 on / 2 in front")
 */

undefined4 plane_classify_point(float *plane,float *point,RwReal *out_distance)

{
  undefined4 result;
  float10 proj_x_ext;
  struct {
    RwV3d proj;
    RwV3d diff;
    float d;
  } frame;

  frame.d = plane[0] * point[0] + plane[1] * point[1] + plane[2] * point[2] + plane[3];
  /* Project point onto the plane (point - d * normal) and measure the offset.
     The x component is kept in extended precision (float10) for the difference. */
  if (out_distance != (RwReal *)0x0) {
    frame.proj = *(RwV3d *)point;
    proj_x_ext = (float10)(-frame.d) * plane[0] + frame.proj.x;
    frame.proj.y = (-frame.d) * plane[1] + frame.proj.y;
    frame.proj.z = (-frame.d) * plane[2] + frame.proj.z;
    frame.diff.x = (float)((float10)point[0] - proj_x_ext);
    frame.diff.y = point[1] - frame.proj.y;
    frame.diff.z = point[2] - frame.proj.z;
    *out_distance = RwV3dLength(&frame.diff);
  }
  /* Compare the signed distance with k_f_0: above, equal, or below. */
  if (k_f_0 < frame.d) {
    return PLANE_CLASS_GREATER;
  }
  result = PLANE_CLASS_LESS;
  if (k_f_0 <= frame.d) {
    result = PLANE_CLASS_EQUAL;
  }
  return result;
}
