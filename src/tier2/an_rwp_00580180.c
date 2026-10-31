#include "mfl_tier2.h"

/* Fills the rows of a two-body constraint for the direction n: the linear row of body A is n, its angular row is r_a x n; body B
   gets -n and n x r_b.  Each half is only written when its flag is set.  Returns n. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float *rwp_constraints_00580180(float *row, float *n, float *r_a, float *r_b, int use_a, int use_b)
{
  if (use_a) {
    *(int *)&row[0] = *(int *)&n[0];
    *(int *)&row[1] = *(int *)&n[1];
    *(int *)&row[2] = *(int *)&n[2];
    row[4] = n[2] * r_a[1] - n[1] * r_a[2];
    row[5] = r_a[2] * n[0] - n[2] * r_a[0];
    row[6] = n[1] * r_a[0] - r_a[1] * n[0];
  }
  if (use_b) {
    row[8] = n[0] * -1.0f;
    row[9] = n[1] * -1.0f;
    row[10] = n[2] * -1.0f;
    row[12] = n[1] * r_b[2] - r_b[1] * n[2];
    row[13] = n[2] * r_b[0] - r_b[2] * n[0];
    row[14] = r_b[1] * n[0] - n[1] * r_b[0];
  }
  return n;
}
