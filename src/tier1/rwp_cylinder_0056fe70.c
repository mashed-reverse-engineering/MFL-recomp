/* compile: /G5 */
#include <math.h>
/* Cylinder bounding sphere: zero centre, radius sqrt(a^2 + b^2) of two half-extents plus a margin, and returns it.  Shape vtable
   slot +0x8: the caller (0x56c0b0) tests the result and reads the sphere through it, so the original's EAX = out is the return
   value; as `void` the v143 build returned what sqrt left in EAX (ret_scan.py FPTR row, agents/det_w17/notes.md). */
float *rwp_cylinder_0056fe70(float *cyl, float *out)
{
  float f1, f2;

  ((int *)out)[2] = 0;
  ((int *)out)[1] = 0;
  ((int *)out)[0] = 0;
  f1 = cyl[17];
  f2 = cyl[16];
  out[3] = (float)sqrt(f2 * f2 + f1 * f1) + cyl[19];
  return out;
}
