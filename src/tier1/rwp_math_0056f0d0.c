/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpSphere { RwV3d center; float radius; } RwpSphere;
extern float RwV3dLength();
/* Grows the sphere in so that it contains the point p (result in out; out may be in). (Rwp library) */
RwpSphere *rwp_math_0056f0d0(RwpSphere *out, RwpSphere *in, RwV3d *p)
{
  RwV3d d;
  float len;

  RwV3dSubMacro(&d, p, &in->center);
  len = RwV3dLength(&d);
  if (len > in->radius) {
    out->center.x = (len - in->radius) / (len + len) * d.x + in->center.x;
    out->center.y = (len - in->radius) / (len + len) * d.y + in->center.y;
    out->center.z = (len - in->radius) / (len + len) * d.z + in->center.z;
    out->radius = (len + in->radius) * 0.5f;
    return out;
  }
  if (out != in) {
    out->center.x = in->center.x;
    out->center.y = in->center.y;
    out->center.z = in->center.z;
    out->radius = in->radius;
  }
  return out;
}
