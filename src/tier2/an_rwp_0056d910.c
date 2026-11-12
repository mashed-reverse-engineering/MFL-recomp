/* rwp_box_sweep_sphere (0x0056d910): RwpBox vtable slot (5f6508): first contact of a sphere moving along a segment with a box */
#include "mfl_tier2.h"

/* The box shape: half extents at +0x40, the sweep radius (margin) at +0x4c. */
typedef struct MflBoxShape {
  unsigned char pad00[0x40];
  float ext[3];   /* 0x40 */
  float margin;   /* 0x4c */
} MflBoxShape;

/* The hit record: shape, contact point, contact normal, accumulated segment fraction. */
typedef struct MflSweepHit {
  void *shape;
  float p[3];   /* 0x4 */
  float n[3];   /* 0x10 */
  float t;   /* 0x1c */
} MflSweepHit;

/* The walk's working set, in the original's frame order (the callees get pointers into it). */
typedef struct MflBoxSweep {
  float p[3];   /* current start point */
  float frac[2];   /* best hit so far: {numerator, denominator} of the fraction along p -> q */
  float n[3];   /* contact normal */
  float d[3];   /* q - p */
  float margin;
  float ef[2];   /* edge region: best bounding plane hit */
  union { float neg; int rq; } w48;   /* -ext of the axis being classified; later the edge cylinder result */
  union { float pi; int axis; } w4c;   /* p of the axis being classified; later the axis of the nearest face */
  float c[3];   /* the feature (corner / edge point) of the current region */
  float side[3];   /* -1 / 0 / +1: p below / inside / above the slab of each axis */
  float e[3];   /* half extents */
  float inside_sign;   /* side of the nearest face when p is inside */
  int budget;
  float q[3];   /* end point */
  float cand_e[2], cand_v[2], cand_f[2];   /* bounding plane hits of the edge / corner / face regions */
  float sdir[3];   /* sign of d per axis */
} MflBoxSweep;

/*005f6524*/ extern float g_rwp_unit_axes[3][3];   /* rows of the identity matrix */
extern int rwp_ray_sphere_test(float *frac, float *p, float *d, float *centre, float radius);
extern int rwp_line_quadric_intersect(float *frac, float *p, float *d, float *base, float *axis, float radius, int a7, int a8);
extern int rwp_grid_005772c0(float *frac, float *p, float *d, int axis, float side, float plane);
extern float rwp_grid_005771f0(float *out, float *in, float *axis, float fallback);

#define NEXT_AXIS(a) ((1 << (a)) & 3)      /* 0 -> 1 -> 2 -> 0 */
#define COPY(dst, src) (*(int *)&(dst) = *(int *)&(src))      /* a float moved as its bit pattern */

/* Region walk: the start point's region against the box (`nout` axes outside the slab: 0 = beside a corner, 1 = beside an edge, 2 = in
   front of a face, 3 = inside) picks the primitive the sphere can hit first (corner sphere, edge cylinder, face plane, all grown by the
   margin); a hit on a bounding plane of that region's slab moves the start to the plane and continues in the neighbouring region, at most
   6 times.  Fills hit (point, normal, hit->t += fraction along start -> end) and returns 1, or returns 0 when there is no contact. */
/* TIER2: unvalidated 2026-11-10 authority=static */
int rwp_box_sweep_sphere(MflBoxShape *box, float *start, float *end, MflSweepHit *hit)
{
  MflBoxSweep f;
  float best, sgn, t, r;
  int nout, edge, i, j, k, rv;

  COPY(f.p[0], start[0]);
  COPY(f.p[1], start[1]);
  COPY(f.p[2], start[2]);
  COPY(f.q[0], end[0]);
  f.d[0] = f.q[0] - f.p[0];
  COPY(f.q[1], end[1]);
  f.d[1] = f.q[1] - f.p[1];
  COPY(f.q[2], end[2]);
  f.d[2] = f.q[2] - f.p[2];
  COPY(f.e[0], box->ext[0]);
  COPY(f.e[1], box->ext[1]);
  COPY(f.e[2], box->ext[2]);
  COPY(f.margin, box->margin);
  f.w4c.axis = 0;
  f.inside_sign = 1.0f;
  f.budget = 6;
  nout = 0;
  edge = 0;
  rv = 0;
  best = -3.4028235e38f;
  for (i = 0; i < 3; i++) {
    f.w4c.pi = f.p[i];
    f.sdir[i] = f.d[i] < 0.0f ? -1.0f : f.d[i] > 0.0f ? 1.0f : 0.0f;
    f.w48.neg = -f.e[i];
    f.frac[0] = f.w48.neg - f.w4c.pi;
    if (f.frac[0] > best) {
      best = f.frac[0];
      rv = i;
      f.inside_sign = -1.0f;
    }
    f.frac[0] = f.w4c.pi - f.e[i];
    if (f.frac[0] > best) {
      best = f.frac[0];
      rv = i;
      f.inside_sign = 1.0f;
    }
    if (f.p[i] < f.w48.neg) {
      f.side[i] = -1.0f;
    } else if (f.p[i] > f.e[i]) {
      f.side[i] = 1.0f;
    } else {
      nout++;
      if (nout == 1)
        edge = i;
      else if (nout == 2)
        edge = (3 - i) - edge;
      *(int *)&f.side[i] = 0;
    }
  }
  f.w4c.axis = rv;
  hit->shape = box;
  *(int *)&hit->t = 0;

  for (;;) {
    if (nout & 2) {
      if (nout != 2) {
        /* inside: the normal is the axis of the nearest face */
        *(int *)&f.n[2] = 0;
        *(int *)&f.n[1] = 0;
        *(int *)&f.n[0] = 0;
        f.n[f.w4c.axis] = f.inside_sign;
        break;
      }
      /* in front of face `edge`: its plane, or a bounding plane of the face slab */
      k = edge;
      rv = rwp_grid_005772c0(f.frac, f.p, f.d, edge, f.side[edge], f.margin + f.e[edge]);
      if (rv < 0)
        return 0;
      if (rv > 0 && f.frac[0] == 0.0f) {
        *(int *)&f.n[2] = 0;
        *(int *)&f.n[1] = 0;
        *(int *)&f.n[0] = 0;
        COPY(f.n[edge], f.side[edge]);
        break;
      }
      for (j = NEXT_AXIS(edge); j != edge; j = NEXT_AXIS(j)) {
        if (!(f.sdir[j] == 0.0f)) {
          i = rwp_grid_005772c0(f.cand_f, f.p, f.d, j, -f.sdir[j], -f.e[j]);
          if (i > 0 && f.cand_f[1] * f.frac[0] > f.cand_f[0] * f.frac[1]) {
            COPY(f.frac[0], f.cand_f[0]);
            COPY(f.frac[1], f.cand_f[1]);
            k = j;
            rv = i;
          }
        }
      }
      if (rv <= 0)
        return 0;
      if (k == edge) {
        t = f.frac[0] / f.frac[1];
        *(int *)&f.n[2] = 0;
        *(int *)&f.n[1] = 0;
        *(int *)&f.n[0] = 0;
        COPY(f.n[edge], f.side[edge]);
        hit->t = t + hit->t;
        f.p[0] = f.d[0] * t + f.p[0];
        f.p[1] = f.d[1] * t + f.p[1];
        f.p[2] = t * f.d[2] + f.p[2];
        break;
      }
      COPY(f.side[k], f.sdir[k]);
      edge = 3 - k - edge;
      nout = 1;
    } else {
      f.c[0] = f.side[0] * f.e[0];
      f.c[1] = f.side[1] * f.e[1];
      f.c[2] = f.side[2] * f.e[2];
      if (nout == 0) {
        /* beside a corner: the corner sphere, or a bounding plane of the corner region */
        rv = rwp_ray_sphere_test(f.frac, f.p, f.d, f.c, f.margin);
        if (rv < 0)
          return 0;
        if (f.margin <= 0.0f) {
          rv = 0;
        } else if (rv > 0 && f.frac[0] == 0.0f) {
          f.n[0] = f.p[0] - f.c[0];
          f.n[1] = f.p[1] - f.c[1];
          f.n[2] = f.p[2] - f.c[2];
          rwp_grid_005771f0(f.n, f.n, f.c, 1.0f);
          break;
        }
        k = -1;
        for (i = 0; i < 3; i++) {
          if (f.sdir[i] * f.side[i] < 0.0f) {
            j = rwp_grid_005772c0(f.cand_v, f.p, f.d, i, f.side[i], f.e[i]);
            if (j > 0 && (rv <= 0 || !(f.frac[1] * f.cand_v[0] > f.cand_v[1] * f.frac[0]))) {
              COPY(f.frac[0], f.cand_v[0]);
              COPY(f.frac[1], f.cand_v[1]);
              k = i;
              rv = j;
            }
          }
        }
        if (rv <= 0)
          return 0;
        if (k < 0) {
          /* the corner sphere: normal = (contact - corner) / margin */
          t = f.frac[0] / f.frac[1];
          hit->t = t + hit->t;
          f.p[0] = f.d[0] * t + f.p[0];
          f.p[1] = f.d[1] * t + f.p[1];
          f.p[2] = t * f.d[2] + f.p[2];
          /* the original reads p[2] and n[2] back from the frame and rounds 1 / margin to float for n[1] and n[2] (differs for
             denormal / overflowing values): p[2] is taken as stored, the other two through volatile frame reads */
          COPY(f.n[2], f.p[2]);
          *(volatile float *)&f.n[2] = f.n[2] - f.c[2];
          r = 1.0f / f.margin;
          *(volatile float *)&f.frac[0] = r;
          f.n[0] = r * (f.p[0] - f.c[0]);
          f.n[1] = *(volatile float *)&f.frac[0] * (f.p[1] - f.c[1]);
          f.n[2] = *(volatile float *)&f.frac[0] * *(volatile float *)&f.n[2];
          break;
        }
        edge = k;
        *(int *)&f.side[k] = 0;
        nout = 1;
      } else {
        /* beside edge `edge`: the edge cylinder, or a bounding plane of the edge region */
        k = edge;
        f.w48.rq = rwp_line_quadric_intersect(f.frac, f.p, f.d, f.c, g_rwp_unit_axes[edge], f.margin, 0, 0);
        if (f.w48.rq < 0)
          return 0;
        if (f.margin <= 0.0f) {
          f.w48.rq = 0;
        } else if (f.w48.rq > 0 && f.frac[0] == 0.0f) {
          f.n[0] = f.p[0] - f.c[0];
          f.n[1] = f.p[1] - f.c[1];
          f.n[2] = f.p[2] - f.c[2];
          *(int *)&f.n[edge] = 0;
          rwp_grid_005771f0(f.n, f.n, g_rwp_unit_axes[edge], 0.0f);
          break;
        }
        sgn = f.d[edge] > 0.0f ? 1.0f : -1.0f;
        f.ef[0] = f.e[edge] - f.p[edge] * sgn;
        f.ef[1] = f.d[edge] * sgn;
        rv = f.ef[0] <= f.ef[1];
        for (j = NEXT_AXIS(edge); j != edge; j = NEXT_AXIS(j)) {
          if (f.sdir[j] * f.side[j] < 0.0f) {
            i = rwp_grid_005772c0(f.cand_e, f.p, f.d, j, f.side[j], f.e[j]);
            if (i > 0 && (rv <= 0 || !(f.cand_e[0] * f.ef[1] > f.cand_e[1] * f.ef[0]))) {
              COPY(f.ef[0], f.cand_e[0]);
              COPY(f.ef[1], f.cand_e[1]);
              k = j;
              rv = i;
            }
          }
        }
        if (f.w48.rq > 0 && (rv <= 0 || !(f.ef[1] * f.frac[0] > f.ef[0] * f.frac[1]))) {
          /* the edge cylinder: normal = (contact - edge) / margin, without the edge axis */
          t = f.frac[0] / f.frac[1];
          hit->t = t + hit->t;
          f.p[0] = f.d[0] * t + f.p[0];
          f.p[1] = f.d[1] * t + f.p[1];
          f.p[2] = t * f.d[2] + f.p[2];
          f.n[0] = f.p[0] - f.c[0];
          f.n[1] = f.p[1] - f.c[1];
          f.n[2] = f.p[2] - f.c[2];
          r = 1.0f / f.margin;
          *(int *)&f.n[edge] = 0;
          f.n[0] = f.n[0] * r;
          f.n[1] = f.n[1] * r;
          f.n[2] = r * f.n[2];
          break;
        }
        if (rv <= 0)
          return 0;
        if (k == edge) {
          COPY(f.side[k], f.sdir[k]);
          nout = 0;
        } else {
          edge = 3 - k - edge;
          *(int *)&f.side[k] = 0;
          nout = 2;
        }
        COPY(f.frac[0], f.ef[0]);
        COPY(f.frac[1], f.ef[1]);
      }
    }
    /* move to the bounding plane that was hit and go on from the neighbouring region */
    t = f.frac[0] / f.frac[1];
    hit->t = t + hit->t;
    f.p[0] = f.d[0] * t + f.p[0];
    f.budget--;
    f.p[1] = f.d[1] * t + f.p[1];
    f.p[2] = t * f.d[2] + f.p[2];
    f.d[0] = f.q[0] - f.p[0];
    f.d[1] = f.q[1] - f.p[1];
    f.d[2] = f.q[2] - f.p[2];
    if (f.budget == 0)
      return 0;
  }
  COPY(hit->p[0], f.p[0]);
  COPY(hit->p[1], f.p[1]);
  COPY(hit->p[2], f.p[2]);
  COPY(hit->n[0], f.n[0]);
  COPY(hit->n[1], f.n[1]);
  COPY(hit->n[2], f.n[2]);
  return 1;
}
