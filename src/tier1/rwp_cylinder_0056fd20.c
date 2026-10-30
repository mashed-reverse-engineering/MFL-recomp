/* compile: /G5 */
#include <math.h>
typedef struct { float x, y, z; } V3;
/* Normalises in into out and returns the length; a vector with squared length <= FLT_MIN gives (0, 0, 1) and returns the squared length. */
float rwp_cylinder_0056fd20(V3 *out, V3 *in)
{
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
  if (len2 > 1.1754944e-38f) {
    inv = 1.0f / (float)sqrt(len2);
    out->x = (float)inv * in->x;
    out->y = in->y * inv;
    out->z = in->z * inv;
    return len2 * inv;
  }
  out->x = 0;
  out->y = 0;
  out->z = 1.0f;
  return len2;
}
