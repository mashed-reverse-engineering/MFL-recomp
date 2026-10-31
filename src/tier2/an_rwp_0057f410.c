#include "mfl_tier2.h"
#define SUM3(r, a, b, c) do { (r) = (a); (r) = (r) + (b); (r) = (r) + (c); } while (0)
/* 3x3 rotation product: p = lhs * rhs, dst = p * rhs^T (all three rows; translation row and flags of dst are untouched).
   Each dot product is summed in the order written (see SUM3). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_batch_0057f410(RwMatrix *dst, RwMatrix *lhs, RwMatrix *rhs)
{
  float p[9];
  double acc;

  SUM3(acc, lhs->up.x * rhs->up.x, lhs->right.x * rhs->right.x, rhs->at.x * lhs->at.x);
  p[0] = acc;
  SUM3(acc, lhs->up.y * rhs->up.x, lhs->at.y * rhs->at.x, lhs->right.y * rhs->right.x);
  p[1] = acc;
  SUM3(acc, lhs->at.z * rhs->at.x, lhs->right.z * rhs->right.x, lhs->up.z * rhs->up.x);
  p[2] = acc;
  SUM3(acc, lhs->up.x * rhs->up.y, rhs->right.y * lhs->right.x, lhs->at.x * rhs->at.y);
  p[3] = acc;
  SUM3(acc, lhs->at.y * rhs->at.y, lhs->up.y * rhs->up.y, lhs->right.y * rhs->right.y);
  p[4] = acc;
  SUM3(acc, lhs->at.z * rhs->at.y, lhs->up.z * rhs->up.y, lhs->right.z * rhs->right.y);
  p[5] = acc;
  SUM3(acc, rhs->at.z * lhs->at.x, lhs->up.x * rhs->up.z, lhs->right.x * rhs->right.z);
  p[6] = acc;
  SUM3(acc, rhs->at.z * lhs->at.y, rhs->up.z * lhs->up.y, lhs->right.y * rhs->right.z);
  p[7] = acc;
  SUM3(acc, lhs->right.z * rhs->right.z, rhs->at.z * lhs->at.z, rhs->up.z * lhs->up.z);
  p[8] = acc;
  SUM3(acc, p[2] * rhs->at.x, p[0] * rhs->right.x, p[1] * rhs->up.x);
  dst->right.x = acc;
  SUM3(acc, p[0] * rhs->right.y, p[2] * rhs->at.y, p[1] * rhs->up.y);
  dst->right.y = acc;
  SUM3(acc, p[2] * rhs->at.z, p[1] * rhs->up.z, p[0] * rhs->right.z);
  dst->right.z = acc;
  SUM3(acc, p[5] * rhs->at.x, p[3] * rhs->right.x, p[4] * rhs->up.x);
  dst->up.x = acc;
  SUM3(acc, p[3] * rhs->right.y, p[5] * rhs->at.y, p[4] * rhs->up.y);
  dst->up.y = acc;
  SUM3(acc, p[5] * rhs->at.z, p[4] * rhs->up.z, p[3] * rhs->right.z);
  dst->up.z = acc;
  SUM3(acc, p[8] * rhs->at.x, p[6] * rhs->right.x, p[7] * rhs->up.x);
  dst->at.x = acc;
  SUM3(acc, p[6] * rhs->right.y, p[8] * rhs->at.y, p[7] * rhs->up.y);
  dst->at.y = acc;
  SUM3(acc, p[8] * rhs->at.z, p[7] * rhs->up.z, p[6] * rhs->right.z);
  dst->at.z = acc;
}
