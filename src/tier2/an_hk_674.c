#include "mfl_tier2.h"
/*00485ff0*/ extern int random_unit_vector();
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005de9c4*/ extern float k_f_0_017453292;
extern double sin(double);
/* random_vector_in_cone */

/* "(out, axis, angle degrees): random direction inside a cone" (was: "(out, ?, angle degrees)") */

/* Writes to out a random unit direction inside a cone around axis (inferred):
 * out = normalize(axis + sin(cone_angle_deg) * random_unit_vector()).
 * out: receives the result. axis: float[3] cone axis (inferred). cone_angle_deg: cone angle in degrees (inferred). Returns nothing. */

void random_vector_in_cone(RwV3d *out,float *axis,float cone_angle_deg)

{
  float cone_scale;
  float offset_x;
  float offset_y;
  double sin_result;

  /* sin of the angle: degrees to radians via k_f_0_017453292 (pi/180). cone_scale is reused below for the z component. */
  sin_result = (double)sin((double)cone_angle_deg * (double)k_f_0_017453292);
  cone_scale = (float)sin_result;
  /* random unit vector in out, scaled by sin(angle) per component */
  random_unit_vector(out);
  offset_x = cone_scale * out->x;
  out->x = offset_x;
  offset_y = cone_scale * out->y;
  out->y = offset_y;
  cone_scale = cone_scale * out->z;
  out->z = cone_scale;
  /* add the axis to the scaled offset */
  out->x = offset_x + *axis;
  out->y = axis[1] + offset_y;
  out->z = axis[2] + cone_scale;
  /* normalize to a unit direction */
  RwV3dNormalize(out,out);
  return;
}
