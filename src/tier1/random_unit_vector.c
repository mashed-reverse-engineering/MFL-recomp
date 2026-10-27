#include "mfl_tier2.h"
/*004c45e0*/ extern float rp_random_float_range(undefined4 p0, undefined4 p1);
/*004d5410*/ extern RwReal _rwSqrt(RwReal p0);
/*005dd330*/ extern float float_1_0;
extern double cos(double);
extern double sin(double);
/* float bit patterns (IEEE 754 single) passed to rp_random_float_range */
#define UNIT_Z_MIN_BITS (0xbf800000)   /* -1.0f */
#define UNIT_Z_MAX_BITS (0x3f800000)   /* 1.0f */
#define ANGLE_MAX_BITS (0x40c90fdb)    /* 2*pi, about 6.2831855f */
/* random_unit_vector */
/* "z in [-1, 1], angle in [0, 2 pi], x/y from sqrt(1 - z^2)" */
/* Writes a uniformly random unit vector to out[0..2] (x, y, z).
 * z is uniform in [-1, 1]; angle is uniform in [0, 2*pi).
 * The ring at height z has radius sqrt(1 - z*z), so x/y are scaled by it.
 * Returns nothing. */
void random_unit_vector(float *out)
{
  float angle;
  double xy_radius;

  out[2] = (float)rp_random_float_range(UNIT_Z_MIN_BITS,UNIT_Z_MAX_BITS);
  angle = (float)rp_random_float_range(0,ANGLE_MAX_BITS);
  xy_radius = _rwSqrt(float_1_0 - out[2] * out[2]);
  *out = (float)(cos(angle) * xy_radius);
  out[1] = (float)(sin(angle) * xy_radius);
  return;
}
