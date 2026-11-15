/* rwp_segment_segment_closest (0x00588560): closest points of two segments */
#include "mfl_tier2.h"

/* Closest points of two segments origin_a + s*dir_a (s in [smin, smax]) and origin_b + t*dir_b (t in [tmin, tmax]), the parameters are returned in *s_out / *t_out.
   Returns 1 when the lines are not parallel and the unconstrained solution is used (possibly clamped on one side), 0 otherwise;
   *overlap is set to 0 or, for the parallel case, to the length of the parameter interval that stays within the range. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_segment_segment_closest(float *origin_a, float *dir_a, float smin, float smax, float *origin_b, float *dir_b, float tmin, float tmax, float *s_out, float *t_out, float *overlap)
{
  double acc0;
  float len1, len2, w[3], dot, det, wd1, wd2, num, inv, q, r, near_t, par_lo, par_hi, g, num2;
  double prod, inv_l2, inv_det, far_t;

  acc0 = dir_a[0] * dir_a[0];
  acc0 = acc0 + dir_a[1] * dir_a[1];
  acc0 = acc0 + dir_a[2] * dir_a[2];
  len1 = acc0;
  acc0 = dir_b[0] * dir_b[0];
  acc0 = acc0 + dir_b[1] * dir_b[1];
  acc0 = acc0 + dir_b[2] * dir_b[2];
  len2 = acc0;
  w[0] = origin_b[0] - origin_a[0];
  w[1] = origin_b[1] - origin_a[1];
  w[2] = origin_b[2] - origin_a[2];
  if (len1 == 0.0f) {
    *s_out = smin;
    *overlap = 0.0f;
    if (len2 == 0.0f) {
      *t_out = tmin;
      return 0;
    }
    acc0 = dir_b[1] * w[1];
    acc0 = acc0 + w[0] * dir_b[0];
    acc0 = acc0 + dir_b[2] * w[2];
    g = -acc0;
    if (len2 * tmin >= g) {
      *t_out = tmin;
      return 0;
    }
    if (len2 * tmax <= g) {
      *t_out = tmax;
      return 0;
    }
    *t_out = g / len2;
    return 0;
  }
  if (len2 == 0.0f) {
    *t_out = tmin;
    *overlap = 0.0f;
    acc0 = dir_a[1] * w[1];
    acc0 = acc0 + w[0] * dir_a[0];
    acc0 = acc0 + dir_a[2] * w[2];
    num2 = acc0;
    if (len1 * smin >= num2) {
      *s_out = smin;
      return 0;
    }
    if (len1 * smax <= num2) {
      *s_out = smax;
      return 0;
    }
    *s_out = num2 / len1;
    return 0;
  }
  acc0 = dir_b[1] * dir_a[1];
  acc0 = acc0 + dir_b[0] * dir_a[0];
  acc0 = acc0 + dir_a[2] * dir_b[2];
  dot = acc0;
  prod = len2 * len1;
  det = prod - dot * dot;
  acc0 = dir_a[1] * w[1];
  acc0 = acc0 + w[0] * dir_a[0];
  acc0 = acc0 + dir_a[2] * w[2];
  wd1 = acc0;
  acc0 = dir_b[1] * w[1];
  acc0 = acc0 + w[0] * dir_b[0];
  acc0 = acc0 + dir_b[2] * w[2];
  wd2 = acc0;
  if (0.0001f * prod < det) {
    num = wd1 * len2 - wd2 * dot;
    if (det * smin > num) {
      *s_out = smin;
    } else {
      if (!(det * smax < num)) goto L_5888fb;
      *s_out = smax;
    }
    num2 = dot * *s_out - wd2;
    if (len2 * tmin > num2) {
      *t_out = tmin;
    } else {
      if (!(len2 * tmax < num2)) goto L_5888da;
      *t_out = tmax;
    }
    num2 = dot * *t_out + wd1;
    if (len1 * smin > num2) {
      *s_out = smin;
      *overlap = 0.0f;
      return 1;
    }
    if (len1 * smax < num2) {
      *s_out = smax;
      *overlap = 0.0f;
      return 1;
    }
    *s_out = num2 / len1;
    *overlap = 0.0f;
    return 1;
  L_5888da:;
    *t_out = num2 / len2;
    *overlap = 0.0f;
    return 1;
  L_5888fb:;
    q = wd1 * dot - wd2 * len1;
    if (det * tmin > q) {
      *t_out = tmin;
    } else {
      if (!(det * tmax < q)) goto L_5889cb;
      *t_out = tmax;
    }
    num2 = dot * *t_out + wd1;
    if (len1 * smin > num2) {
      *s_out = smin;
      *overlap = 0.0f;
      return 1;
    }
    if (len1 * smax < num2) {
      *s_out = smax;
    } else {
      *s_out = num2 / len1;
    }
    *overlap = 0.0f;
    return 1;
  L_5889cb:;
    inv_det = 1.0f / det;
    *s_out = num * inv_det;
    *t_out = inv_det * q;
    *overlap = 0.0f;
    return 1;
  }
  inv = 1.0f / prod;
  inv_l2 = inv * len2;
  q = dot * inv_l2;
  r = inv_l2 * wd1;
  if (q > 0.0f) {
    far_t = tmax;
    near_t = tmin;
  } else {
    far_t = tmin;
    near_t = tmax;
  }
  par_lo = near_t * q + r;
  par_hi = q * far_t + r;
  if (par_hi <= smin) {
    *s_out = smin;
    *overlap = 0.0f;
    *t_out = far_t;
    return 0;
  }
  if (par_lo >= smax) {
    *s_out = smax;
    *overlap = 0.0f;
    *t_out = near_t;
    return 0;
  }
  *s_out = smin > par_lo ? smin : par_lo;
  *overlap = (smax < par_hi ? smax : par_hi) - *s_out;
  *t_out = (dot * *s_out - wd2) * (inv * len1);
  return 0;
}
