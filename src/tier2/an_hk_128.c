#include "mfl_tier2.h"
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd340*/ extern float k_f_0_5;
/* FUN_0056f0d0 */
/* Grows a bounding sphere just enough to enclose a point (inferred: bounding-volume merge).
   sphere: x y z radius of the existing sphere. point: x y z of the point to enclose.
   out_sphere: receives the resulting sphere (may be the same buffer as sphere).
   If the point is outside the sphere, the new sphere has radius (dist + radius) / 2 and its
   centre is moved from the old centre towards the point by (dist - radius) / 2.
   If the point is already inside, sphere is copied unchanged. Returns nothing. */

void __cdecl FUN_0056f0d0(float *out_sphere,float *sphere,float *point)

{
  float two_dist;
  RwReal dist;
  RwV3d center_to_point;

  /* offset from the sphere centre to the point */
  center_to_point.x = *point - *sphere;
  center_to_point.y = point[1] - sphere[1];
  center_to_point.z = point[2] - sphere[2];
  dist = RwV3dLength(&center_to_point);
  /* point is outside the sphere: grow it towards the point */
  if (sphere[3] < dist) {
    two_dist = dist + dist;
    *out_sphere = ((dist - sphere[3]) / two_dist) * center_to_point.x + *sphere;
    out_sphere[1] = ((dist - sphere[3]) / two_dist) * center_to_point.y + sphere[1];
    out_sphere[2] = ((dist - sphere[3]) / two_dist) * center_to_point.z + sphere[2];
    out_sphere[3] = (dist + sphere[3]) * k_f_0_5;
    return;
  }
  /* point already inside: pass the sphere through unchanged */
  if (out_sphere != sphere) {
    *out_sphere = *sphere;
    out_sphere[1] = sphere[1];
    out_sphere[2] = sphere[2];
    out_sphere[3] = sphere[3];
  }
  return;
}


