/* compile: /G5 */
#include <math.h>
/* Index of the component of v with the largest magnitude (0 = x, 1 = y, 2 = z). */
int rwp_math_0056f020(float *v)
{
  float ax;
  float ay;
  float az[1];

  ax = (float)fabs(v[0]);
  ay = (float)fabs(v[1]);
  az[0] = (float)fabs(v[2]);
  if (ax > ay) {
    if (!(ax > az[0])) {
      return 2;
    }
    return 0;
  }
  return ay > az[0] ? 1 : 2;
}
