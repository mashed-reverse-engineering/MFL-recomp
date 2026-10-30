/* compile: /G5 */
#include <math.h>
/* rwp_math_0056f080: lifted from the original code (tools/x87/lift.py) */
int rwp_math_0056f080(float *a1)
{
  if ((a1[0] + 1.0f) >= a1[4]) {
    if ((a1[1] + 1.0f) >= a1[5]) {
      if ((a1[2] + 1.0f) >= a1[6]) {
        return 1;
      }
    }
  }
  return 0;
}
