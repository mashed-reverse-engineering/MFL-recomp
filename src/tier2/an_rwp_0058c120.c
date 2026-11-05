/* rwp_triangle_segment_contact (0x0058c120): contact of a moving point (segment p -> q, triangle local space, the face in the plane z = 0) with the Rwp triangle shape */
#include "mfl_tier2.h"
#include <math.h>

/*0058c760*/ extern int rwp_null_0058c760(float *p, float *a, float *b, float *c);   /* p inside the triangle edge test (a: apex, b, c) */
/*0058c5c0*/ extern float rwp_segment_swept_sphere_test(float *out, float *p0, float *d, float *a, float *b, float radius);
/*0056fd20*/ extern float rwp_cylinder_0056fd20(float *out, float *v);   /* normalise v into out, returns the length */

typedef struct MflTriContact {
  float *tri;   /* 0x0 */
  float pt[3];   /* 0x4 */
  float n[3];   /* 0x10 */
  float t;   /* 0x1c: distance along the segment */
} MflTriContact;

#define COPY3(dst, src) do { ((int *)(dst))[0] = ((int *)(src))[0]; ((int *)(dst))[1] = ((int *)(src))[1]; ((int *)(dst))[2] = ((int *)(src))[2]; } while (0)

/* s: triangle (vertices at s, s + 4, s + 8, s[19] = margin).  Both ends further than the margin on the same side of the plane: no contact (0).
   A start point outside the margin slab is moved along the segment onto the slab face; if that point (or, inside the slab, the start point
   itself) lies inside all three edges the contact is on the face.  Otherwise the segment is swept against the three edges (capsules of the
   margin radius, rwp_segment_swept_sphere_test) and the earliest hit with parameter <= 1 wins.  Fills out and returns TRUE. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwp_triangle_segment_contact(float *s, float *p_in, float *q_in, MflTriContact *out)
{
  float p[3], q[3], d[3], n[3];
  float o[3][3];   /* closest points on the three edges */
  float r[3];
  float margin = s[19];
  float *v1 = s + 4, *v2 = s + 8;
  double tt, xx, xy;
  volatile float tv, zz;   /* the original keeps the hit parameter and the z of the hit point in frame floats */
  int sh, i;

  COPY3(p, p_in);
  COPY3(q, q_in);
  if (fabs(p[2]) > margin && fabs(q[2]) > margin && q[2] * p[2] > 0.0f) return 0;
  d[0] = q[0] - p[0];
  d[1] = q[1] - p[1];
  d[2] = q[2] - p[2];
  out->tri = s;
  if (fabs(p[2]) > margin) {
    tt = ((p[2] < 0.0f ? -1.0f : 1.0f) * margin - p[2]) / d[2];
    tv = tt;
    q[0] = tt * d[0] + p[0];
    q[1] = tv * d[1] + p[1];
    q[2] = tv * d[2] + p[2];
    if (rwp_null_0058c760(q, s, v1, v2) && rwp_null_0058c760(q, v1, s, v2) && rwp_null_0058c760(q, v2, s, v1)) {
      ((int *)n)[0] = 0;
      ((int *)n)[1] = 0;
      n[2] = p[2] < 0.0f ? -1.0f : 1.0f;
      COPY3(out->pt, q);
      out->t = sqrt(d[1] * d[1] + d[0] * d[0] + d[2] * d[2]) * tv;
      COPY3(out->n, n);
      return 1;
    }
  } else if (rwp_null_0058c760(p, s, v1, v2) && rwp_null_0058c760(p, v1, s, v2) && rwp_null_0058c760(p, v2, s, v1)) {
    rwp_cylinder_0056fd20(n, d);
    n[0] = -n[0];
    out->t = 0.0f;
    n[1] = -n[1];
    COPY3(out->pt, p);
    n[2] = -n[2];
    COPY3(out->n, n);
    return 1;
  }
  r[0] = rwp_segment_swept_sphere_test(o[0], p, d, s, v1, margin);
  r[1] = rwp_segment_swept_sphere_test(o[1], p, d, v1, v2, margin);
  r[2] = rwp_segment_swept_sphere_test(o[2], p, d, v2, s, margin);
  sh = ((((r[0] > r[1]) << 1 | (r[0] > r[2])) << 1) | (r[1] > r[2])) << 2;
  i = (0x21312300 >> sh) & 3;   /* the smallest parameter */
  if (!(r[i] <= 1.0f)) return 0;
  if (r[i] == 0.0f) {
    rwp_cylinder_0056fd20(n, d);
    n[0] = -n[0];
    out->t = 0.0f;
    n[1] = -n[1];
    COPY3(out->pt, p);
    n[2] = -n[2];
  } else {
    out->t = sqrt(d[1] * d[1] + d[0] * d[0] + d[2] * d[2]) * r[i];
    xx = r[i] * d[0] + p[0];
    out->pt[0] = xx;
    xy = r[i] * d[1] + p[1];
    out->pt[1] = xy;
    zz = r[i] * d[2] + p[2];
    out->pt[2] = zz;
    n[0] = xx - o[i][0];
    n[1] = xy - o[i][1];
    n[2] = zz - o[i][2];
    rwp_cylinder_0056fd20(n, n);
  }
  COPY3(out->n, n);
  return 1;
}
