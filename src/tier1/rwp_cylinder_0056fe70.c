/* compile: /G5 */
#include <math.h>
/* Cylinder bounding radius: sqrt(a^2 + b^2) of two half-extents plus a margin. */
void rwp_cylinder_0056fe70(float *cyl, int out)
{
  float f1, f2;

  *(int *)(out + 0x8) = 0;
  *(int *)(out + 0x4) = 0;
  *(int *)(out) = 0;
  f1 = cyl[17];
  f2 = cyl[16];
  *(float *)(out + 0xc) = (float)sqrt(f2 * f2 + f1 * f1) + cyl[19];
}
