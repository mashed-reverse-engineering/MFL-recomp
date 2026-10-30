/* compile: /G5 */
#include <math.h>
/* Rotates two interleaved float triples (rows a2*4 and a3*4 of the table a1, stride 0xc) by the pair (a4, a5): row_a = a4*row_a - a5*row_b, row_b = a4*row_b + a5*row_a. */
void rwp_obj_0056ee50(float *a1, int a2, int a3, float a4, float a5)
{
  float tmp[6];
  float head[2];

  head[0] = a4 * a1[a2] - a5 * a1[a3];
  head[1] = a4 * a1[a2 + 3] - a5 * a1[a3 + 3];
  tmp[5] = a4 * a1[a2 + 6] - a5 * a1[a3 + 6];
  tmp[0] = a4 * a1[a3] + a5 * a1[a2];
  tmp[1] = a4 * a1[a3 + 3] + a5 * a1[a2 + 3];
  tmp[2] = a5 * a1[a2 + 6] + a4 * a1[a3 + 6];
  a1[a2] = head[0];
  a1[a2 + 3] = head[1];
  a1[a2 + 6] = tmp[5];
  a1[a3] = tmp[0];
  a1[a3 + 3] = tmp[1];
  a1[a3 + 6] = tmp[2];
}
