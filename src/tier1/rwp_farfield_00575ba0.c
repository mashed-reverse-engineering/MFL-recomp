/* compile: /G5 */
/* Interpolates two corner points of the box a2 (0.4 / 0.6 weights of its two corners), compares them with the corners of the box a1 into two 3 bit codes (one bit per axis) and returns 1 when together the codes cover all three axes. (Rwp library) */
int rwp_farfield_00575ba0(float *a1, float *a2)
{
  struct { float lo[4]; float hi[4]; } bx;      /* lo = max', hi = min' (the shrunk corners) */
  int r1, r2, r3, r4, r5, r6;
  bx.hi[0] = a2[4] * 0.6f;
  bx.hi[1] = a2[5] * 0.6f;
  bx.hi[2] = a2[6] * 0.6f;
  bx.hi[0] = a2[0] * 0.39999998f + bx.hi[0];
  bx.hi[1] = a2[1] * 0.39999998f + bx.hi[1];
  bx.hi[2] = a2[2] * 0.39999998f + bx.hi[2];
  bx.lo[0] = a2[0] * 0.6f;
  bx.lo[1] = a2[1] * 0.6f;
  bx.lo[2] = a2[2] * 0.6f;
  bx.lo[0] = bx.lo[0] + a2[4] * 0.39999998f;
  bx.lo[1] = a2[5] * 0.39999998f + bx.lo[1];
  bx.lo[2] = a2[6] * 0.39999998f + bx.lo[2];
  if (a1[4] > bx.hi[0]) r6 = 1; else r6 = 0;
  if (a1[5] > bx.hi[1]) r4 = 1; else r4 = 0;
  if (a1[6] > bx.hi[2]) r5 = 1; else r5 = 0;
  if (bx.lo[0] > a1[0]) r3 = 1; else r3 = 0;
  if (bx.lo[1] > a1[1]) r1 = 1; else r1 = 0;
  if (bx.lo[2] > a1[2]) r2 = 1; else r2 = 0;
  return (r3 + (r1 + r2 * 2) * 2 | r6 + (r4 + r5 * 2) * 2) == 7;
}
