/* compile: /G5 */
#include <math.h>
/* Bounding sphere of an axis-aligned box {lo xyz,w; hi xyz,w}: centre = (lo + hi) / 2, radius = half the diagonal. */
void rwp_math_0056f180(float *sphere, float *box)
{
  float dx, dy, dz;
  float *lo = box, *hi = box + 4;

  sphere[0] = hi[0] + lo[0];
  sphere[1] = hi[1] + lo[1];
  sphere[2] = hi[2] + lo[2];
  sphere[0] = sphere[0] * 0.5f;
  sphere[1] = sphere[1] * 0.5f;
  sphere[2] = sphere[2] * 0.5f;
  dz = hi[2] - lo[2];
  dy = hi[1] - lo[1];
  dx = hi[0] - lo[0];
  sphere[3] = (float)sqrt(dx * dx + dy * dy + dz * dz) * 0.5f;
}
