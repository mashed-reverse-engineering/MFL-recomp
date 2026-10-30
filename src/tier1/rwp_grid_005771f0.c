/* compile: /G5 */
#include <math.h>
typedef struct { float x, y, z; } V3;
/*0056fd20*/ extern float rwp_cylinder_0056fd20(V3 *out, V3 *in);
/*0056fd90*/ extern int rwp_cylinder_0056fd90(V3 *out, V3 *in);
/* Unit vector of in written to out (returns its length); a vector shorter than 1e-6 is replaced by the normalised fallback alt (scaled by scale when scale is not 0) and 0 is returned. */
float rwp_grid_005771f0(V3 *out, V3 *in, V3 *alt, float scale)
{
  float t;
  float y;
  float x;
  float z;
  float xy;
  float len2;
  double inv;

  y = in->y;
  x = in->x;
  z = in->z;
  xy = x * x + y * y;
  len2 = xy + z * z;
  if (len2 > 1e-12f) {
    inv = 1.0f / (float)sqrt(len2);
    out->x = (float)inv * in->x;
    out->y = in->y * inv;
    out->z = in->z * inv;
    return len2 * inv;
  }
  if (scale == 0.0f) {
    rwp_cylinder_0056fd90(out, alt);
    rwp_cylinder_0056fd20(out, out);
    return 0.0f;
  }
  out->x = scale * alt->x;
  t = alt->y;
  out->y = t * scale;
  t = alt->z;
  out->z = t * scale;
  rwp_cylinder_0056fd20(out, out);
  return 0.0f;
}
