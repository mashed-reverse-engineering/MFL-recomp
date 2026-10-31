#include "mfl_tier2.h"

/*00583f40*/ extern int rwp_convexhullmassprops_00583f40(void *body, float *a, float *m, float *b);
/*0056ef10*/ extern int rwp_obj_0056ef10(float *out, float *m);
/*00557340*/ extern int RtQuatTransformVectors(float *out, float *in, int count, float *quat);
/*00575670*/ extern float rwp_capsule_00575670(void *body, float *dir, float lo, float hi, float *extra);

/* Extent of a capsule-like body along the axis of its smallest principal inertia: the inertia matrix m of the body (583f40) is decomposed
   (56ef10 writes the three principal values to body+0x68), the axis with the smallest value is picked from the order of the negated values
   (the table 0x21312300 maps the three compare bits to 0, 1, 2), rotated into the world by the quaternion at body+0x74, and the support
   extent in both directions (575670, +dir and -dir) is added into body+0x58.  Marks body+0x88 |= 2 and returns the new flags. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_capsule_005754f0(unsigned char *body)
{
  float m[9], axis[3], dir[3], extra;
  float *pv = (float *)(body + 0x68);
  double x;
  float y, z, r1, r2;
  int c1, c2, c3, a, flags;

  rwp_convexhullmassprops_00583f40(body, (float *)(body + 0x5c), m, (float *)(body + 0x54));
  rwp_obj_0056ef10(pv, m);
  x = pv[0] * -1.0f;
  y = pv[1] * -1.0f;
  z = pv[2] * -1.0f;
  c1 = x > y;
  c2 = x > z;
  c3 = y > z;
  a = (0x21312300 >> ((((c1 * 2) | c2) * 2 | c3) << 2)) & 3;
  axis[0] = (float)(a == 0);
  axis[1] = (float)(a == 1);
  axis[2] = (float)(a == 2);
  RtQuatTransformVectors(dir, axis, 1, (float *)(body + 0x74));
  r1 = rwp_capsule_00575670(body, dir, 0.0f, 3.4028235e38f, &extra);
  dir[0] = axis[0] * -1.0f;
  dir[1] = axis[1] * -1.0f;
  dir[2] = axis[2] * -1.0f;
  r2 = rwp_capsule_00575670(body, dir, 0.0f, 3.4028235e38f, &extra);
  flags = *(int *)(body + 0x88) | 2;
  *(int *)(body + 0x88) = flags;
  *(float *)(body + 0x58) = r2 + r1;
  return flags;
}
