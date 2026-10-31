#include "mfl_tier2.h"
/* Layouts as in recomp/include/rwp_types.h (copied: a new header in this directory would change the object key of every cached tier 2 object; move to the shared header when that cost is accepted). */
/* Contact plane written by the shape query (rwp_aggregate_00586f20): unit normal and its distance. */
typedef struct MflPlane {
  float n[3];   /* 0x0 */
  float d;      /* 0xc */
} MflPlane;

/* One of the two shapes of a contact pair (0x80 bytes, only the fields used so far). */
typedef struct MflShape {
  MflPlane plane;   /* 0x0: filled by the shape query */
  unsigned char pad10[0x40];
  float *points;   /* 0x50: vertices, 16 bytes each (xyz first) */
  unsigned short count;   /* 0x54: 1 = point, 2 = edge, more = polygon */
  unsigned short mask;   /* 0x56 */
  int v58;   /* 0x58 */
  int v5c;   /* 0x5c */
  float normal[3];   /* 0x60 */
  int v6c;   /* 0x6c */
  float lo0;   /* 0x70 */
  float lo1;   /* 0x74 */
  float radius;   /* 0x78 */
  unsigned char flags;   /* 0x7c: bit 5 = no plane output, bit 6 = oriented by the filter word */
  unsigned char pad7d;
  unsigned short filter;   /* 0x7e */
} MflShape;

typedef struct MflShapePair {
  MflShape a;
  MflShape b;   /* 0x80 */
  float pos[3];   /* 0x100: query direction (unit) */
  int v10c;   /* 0x10c */
  float margin;   /* 0x110 */
} MflShapePair;

extern float rwp_cylinder_0056fd20(float *out, float *in);
extern int rwp_aggregate_00588280(float *origin, float *dir, float t_min, float t_max, float *point, float *out);
extern int rwp_aggregate_00588350(float *poly, unsigned n, float *nrm, float *p, float *out, unsigned *edge);
extern int rwp_aggregate_00588560(float *p1, float *d1, float smin, float smax, float *p2, float *d2, float tmin, float tmax, float *s_out, float *t_out, float *overlap);
extern int rwp_aggregate_00588b40(unsigned int *count, float *pts, float *p, float *d, float level);

#define EPS 1.0e-6f
#define NEXT(i, n) ((((int)((i) - (n) + 1)) >> 31) & ((i) + 1))     /* (i + 1) mod n */
#define SUB3(o, a, b) ((o)[0] = (a)[0] - (b)[0], (o)[1] = (a)[1] - (b)[1], (o)[2] = (a)[2] - (b)[2])
/* x = |v|^2, summed y, x, z like the original */
#define SQ3(x, v) do { double s_; s_ = (v)[1] * (v)[1]; s_ = s_ + (v)[0] * (v)[0]; s_ = s_ + (v)[2] * (v)[2]; (x) = s_; } while (0)
/* res = u . (v x w), summed term by term in the original order */
#define TRIPLE(res, u, v, w) do { \
    double t_; \
    t_ = ((v)[2] * (w)[0] - (v)[0] * (w)[2]) * (u)[1]; \
    t_ = t_ + ((v)[1] * (w)[2] - (v)[2] * (w)[1]) * (u)[0]; \
    t_ = t_ + ((v)[0] * (w)[1] - (v)[1] * (w)[0]) * (u)[2]; \
    (res) = t_; } while (0)

/* Closest features of two convex vertex rings A (a, na vertices: 1 point, 2 edge, more polygon) and B (b, nb) along the unit direction nrm.
   Walks both outlines in step (rotating calipers; the edge/edge closest points come from 588560) and keeps the closest pair found; the
   contact points of the overlap are appended to res (588b40, count = result size, cap = na + nb).  Without a gap the polygon lying on one
   side is projected onto the plane `off` instead.  On the closest pair: *margin = distance, nrm = unit direction between the points, off = the
   plane between them, and the planes of both shapes (pair->a.plane, pair->b.plane) are written from the edge directions;
   *kind_a / *kind_b are 0 for a vertex hit, 1 for an interior edge point, 2 / the 588350 result for a polygon.  Returns the number of points in res. */
/* TIER2: unvalidated 2026-11-01 authority=diffemu */
int rwp_aggregate_00586f20(float *a, unsigned na, float *b, unsigned nb, float diff, float off, float *nrm, float *margin, int *kind_a, int *kind_b,
                           MflShapePair *pair, float *res)
{
  unsigned count = 0, cap = na + nb;
  float pa[3], pb[3], d[3], da[3], db[3];
  float dist2, ov;
  double r, dot, nx, ny, nz;
  unsigned idx, pn, i;
  float *pv;
  double tp;

  if (na == 0) {
    pv = b;
    pn = nb;
    goto project;
  }
  if (nb == 0) {
    pv = a;
    pn = na;
    goto project;
  }
  if (na == 1) {
    *kind_a = 0;
    if (nb == 1) {
      pb[0] = b[0]; pb[1] = b[1]; pb[2] = b[2];
      *kind_b = 0;
    } else if (nb == 2) {
      db[0] = b[4] - b[0];
      db[1] = b[5] - b[1];
      db[2] = b[6] - b[2];
      *kind_b = rwp_aggregate_00588280(b, db, 0.0f, 1.0f, a, pb);
    } else {
      *kind_b = rwp_aggregate_00588350(b, nb, nrm, a, pb, &idx);
      if (*kind_b == 2) {
        if (count >= cap) return count;
        rwp_aggregate_00588b40(&count, res, pb, nrm, off);
        return count;
      }
      if (*kind_b == 1) {
        float *p0 = b + 4 * NEXT(idx, nb), *p1 = b + 4 * idx;
        SUB3(db, p0, p1);
      }
    }
    pa[0] = a[0]; pa[1] = a[1]; pa[2] = a[2];
    ov = 0.0f;
    SUB3(d, pa, pb);
    SQ3(dist2, d);
    goto done;
  }
  if (nb == 1) {
    *kind_b = 0;
    if (na == 2) {
      da[0] = a[4] - a[0];
      da[1] = a[5] - a[1];
      da[2] = a[6] - a[2];
      *kind_a = rwp_aggregate_00588280(a, da, 0.0f, 1.0f, b, pa);
    } else {
      *kind_a = rwp_aggregate_00588350(a, na, nrm, b, pa, &idx);
      if (*kind_a == 2) {
        if (count >= cap) return count;
        rwp_aggregate_00588b40(&count, res, pa, nrm, off);
        return count;
      }
      if (*kind_a == 1) {
        float *p0 = a + 4 * NEXT(idx, na), *p1 = a + 4 * idx;
        SUB3(da, p0, p1);
      }
    }
    SUB3(d, pa, b);
    pb[0] = b[0]; pb[1] = b[1]; pb[2] = b[2];
    ov = 0.0f;
    SQ3(dist2, d);
    goto done;
  }
  if (cap <= 4) {   /* edge against edge */
    float *ea = a, *eb = b;
    float sb, ta;
    db[0] = eb[4] - eb[0];
    db[1] = eb[5] - eb[1];
    db[2] = eb[6] - eb[2];
    da[0] = ea[4] - ea[0];
    da[1] = ea[5] - ea[1];
    da[2] = ea[6] - ea[2];
    rwp_aggregate_00588560(eb, db, 0.0f, 1.0f, ea, da, 0.0f, 1.0f, &sb, &ta, &ov);
    pa[0] = ta * da[0] + ea[0];
    pa[1] = ta * da[1] + ea[1];
    pa[2] = ta * da[2] + ea[2];
    pb[0] = sb * db[0] + eb[0];
    pb[1] = sb * db[1] + eb[1];
    pb[2] = sb * db[2] + eb[2];
    SUB3(d, pa, pb);
    *kind_a = (unsigned short)(ta * ta != ta);
    *kind_b = (unsigned short)(sb * sb != sb);
    SQ3(dist2, d);
    goto done;
  }
  {
    unsigned ct = 2 * cap + 1;
    unsigned ia = 0, ia1 = 1, ib = 0, ib1 = 1, adv = 0, mode = 0;
    unsigned best_ia = 0, best_ib = 0, ka = 0, kb = 0, flag_a = 0, flag_b = 0;
    float best_ta = 0.0f, best_sb = 0.0f, prev2 = 0.0f, prev3 = 0.0f;
    float ea[3], eb[3], pt[3];
    volatile float dd[3], w[3];   /* the original keeps these in frame slots (float rounding at the spill, visible at extended precision only); cheat_allow.txt */
    double q[3], acc;
    float c1, c2, c3, lam, mu, d2, ovc, sb, ta, cx, cy;
    double u[3], cz;
    float *pa_i, *pb_i, *pnext;

    dist2 = 3.4028235e38f;
    ov = 0.0f;
    while (ct != 0) {
      ct--;
      if (adv != 2) SUB3(ea, a + 4 * ia1, a + 4 * ia);
      if (adv != 1) SUB3(eb, b + 4 * ib1, b + 4 * ib);
      pa_i = a + 4 * ia;
      pb_i = b + 4 * ib;
      rwp_aggregate_00588560(pb_i, eb, 0.0f, 1.0f, pa_i, ea, 0.0f, 1.0f, &sb, &ta, &ovc);
      pa[0] = ta * ea[0] + pa_i[0];
      pa[1] = ta * ea[1] + pa_i[1];
      pa[2] = ta * ea[2] + pa_i[2];
      pb[0] = sb * eb[0] + pb_i[0];
      pb[1] = sb * eb[1] + pb_i[1];
      pb[2] = sb * eb[2] + pb_i[2];
      dd[0] = pa[0] - pb[0];
      dd[1] = pa[1] - pb[1];
      dd[2] = pa[2] - pb[2];
      SQ3(d2, dd);
      if (d2 < dist2 + EPS) {
        if (ov == 0.0f || ovc != 0.0f) {
          ka = (unsigned short)(ta * ta != ta);
          kb = (unsigned short)(sb * sb != sb);
          best_ib = ib;
          best_ta = ta;
          best_ia = ia;
          ov = ovc;
          best_sb = sb;
          d[0] = dd[0]; d[1] = dd[1]; d[2] = dd[2];
          dist2 = d2;
        }
      }
      w[0] = a[4 * ia1] - pb_i[0];
      w[1] = a[4 * ia1 + 1] - pb_i[1];
      w[2] = a[4 * ia1 + 2] - pb_i[2];
      TRIPLE(c1, ea, eb, nrm);
      TRIPLE(c3, eb, w, nrm);
      u[0] = b[4 * ib1] - pa_i[0];
      u[1] = b[4 * ib1 + 1] - pa_i[1];
      u[2] = b[4 * ib1 + 2] - pa_i[2];
      TRIPLE(c2, ea, u, nrm);
      if (adv != 0) {
        if (c2 * prev2 <= 0.0f || c3 * prev3 <= 0.0f) {
          if (fabs(c1) >= EPS) {
            r = 1.0f / c1;
            q[0] = pa_i[0] - pb_i[0];
            q[1] = pa_i[1] - pb_i[1];
            q[2] = pa_i[2] - pb_i[2];
            cx = q[1] * nrm[2] - q[2] * nrm[1];
            cy = q[2] * nrm[0] - q[0] * nrm[2];
            cz = q[0] * nrm[1] - q[1] * nrm[0];
            acc = cx * eb[0];
            acc = acc + cz * eb[2];
            acc = acc + cy * eb[1];
            lam = acc * r;
            acc = cy * ea[1];
            acc = acc + cx * ea[0];
            acc = acc + cz * ea[2];
            mu = acc * r;
            if (lam > -EPS && lam < 1.000001f && mu > -EPS && mu < 1.000001f) {
              pt[0] = lam * ea[0] + pa_i[0];
              pt[1] = lam * ea[1] + pa_i[1];
              pt[2] = lam * ea[2] + pa_i[2];
              if (count >= cap) return count;
              if (rwp_aggregate_00588b40(&count, res, pt, nrm, off)) return count;
              mode = c1 < 0.0f ? 1 : 2;
            }
          }
        }
      }
      if (mode == 1) {
        if (c3 < 0.0f && c2 > 0.0f) mode = 2;
        else if (c2 < 0.0f && c3 < 0.0f) mode = 0;
      } else if (mode == 2) {
        if (c2 < 0.0f) {
          if (c3 > 0.0f) mode = 1;
          else if (c3 < 0.0f) mode = 0;
        }
      }
      if (c1 > 0.0f) adv = c2 > 0.0f ? 1 : 2;
      else adv = c3 > 0.0f ? 2 : 1;
      if (c2 == 0.0f && c3 == 0.0f) adv = mode != 1 ? 1 : 2;
      if (adv == 1) {
        ia = ia1;
        ia1 = NEXT(ia, na);
        pnext = a + 4 * ia;
      } else {
        ib = ib1;
        ib1 = NEXT(ib, nb);
        pnext = b + 4 * ib;
      }
      if (mode == adv) {
        if (count >= cap) return count;
        if (rwp_aggregate_00588b40(&count, res, pnext, nrm, off)) return count;
      }
      prev3 = c3;
      prev2 = c2;
      flag_b |= c2 <= 0.0f;
      flag_a |= c3 <= 0.0f;
      if (count == 0 && ct < cap) {
        if (flag_a != 0 ? flag_b == 0 : flag_b != 0) {
          pv = flag_a != 0 ? b : a;
          pn = flag_a != 0 ? nb : na;
          goto project;
        }
        if (dist2 >= 3.4028235e38f) return 0;
        {
          float *p0 = a + 4 * NEXT(best_ia, na), *p1 = a + 4 * best_ia;
          float *q0 = b + 4 * NEXT(best_ib, nb), *q1 = b + 4 * best_ib;
          SUB3(da, p0, p1);
          SUB3(db, q0, q1);
          pa[0] = best_ta * da[0] + p1[0];
          pa[1] = best_ta * da[1] + p1[1];
          pa[2] = best_ta * da[2] + p1[2];
          pb[0] = best_sb * db[0] + q1[0];
          pb[1] = best_sb * db[1] + q1[1];
          pb[2] = best_sb * db[2] + q1[2];
          *kind_a = ka;
          *kind_b = kb;
          goto done;
        }
      }
    }
    if (count != 0) return count;
  }
done:
  if (*margin >= 0.0f && dist2 > 1.17549435e-38f && !(pair->a.flags & 0x20) && !(pair->b.flags & 0x20)) {
    r = 1.0f / sqrt(dist2);
    *margin = dist2 * r;
    nx = d[0] * r;
    nrm[0] = nx;
    ny = d[1] * r;
    nrm[1] = ny;
    nz = d[2] * r;
    nrm[2] = nz;
    dot = ny * pb[1];
    dot = dot + nx * pb[0];
    dot = dot + nz * pb[2];
    off = dot + (diff + *margin) * 0.5f;
  }
  if (count < cap) rwp_aggregate_00588b40(&count, res, pb, nrm, off);
  if (ov > 0.0f && count < cap) {
    pb[0] = ov * db[0] + pb[0];
    pb[1] = ov * db[1] + pb[1];
    pb[2] = ov * db[2] + pb[2];
    rwp_aggregate_00588b40(&count, res, pb, nrm, off);
    *kind_a = 1;
    *kind_b = 1;
  } else if (*kind_a == 0) {
    pair->a.plane.n[0] = nrm[0]; pair->a.plane.n[1] = nrm[1]; pair->a.plane.n[2] = nrm[2];
    goto plane_a_done;
  }
  {
    float cn[3];
    cn[0] = nrm[2] * da[1] - nrm[1] * da[2];
    cn[1] = da[2] * nrm[0] - nrm[2] * da[0];
    cn[2] = nrm[1] * da[0] - da[1] * nrm[0];
    pair->a.plane.n[0] = cn[1] * da[2] - cn[2] * da[1];
    pair->a.plane.n[1] = cn[2] * da[0] - cn[0] * da[2];
    pair->a.plane.n[2] = cn[0] * da[1] - cn[1] * da[0];
    rwp_cylinder_0056fd20(pair->a.plane.n, pair->a.plane.n);
  }
plane_a_done:
  pair->a.plane.n[0] *= -1.0f;
  pair->a.plane.n[1] *= -1.0f;
  pair->a.plane.n[2] *= -1.0f;
  dot = pair->a.plane.n[1] * pa[1];
  dot = dot + pair->a.plane.n[0] * pa[0];
  pair->a.plane.d = dot + pair->a.plane.n[2] * pa[2];
  if (*kind_b == 0) {
    pair->b.plane.n[0] = nrm[0]; pair->b.plane.n[1] = nrm[1]; pair->b.plane.n[2] = nrm[2];
  } else {
    float cn[3];
    cn[0] = nrm[2] * db[1] - nrm[1] * db[2];
    cn[1] = db[2] * nrm[0] - nrm[2] * db[0];
    cn[2] = nrm[1] * db[0] - db[1] * nrm[0];
    pair->b.plane.n[0] = cn[1] * db[2] - cn[2] * db[1];
    pair->b.plane.n[1] = cn[2] * db[0] - cn[0] * db[2];
    pair->b.plane.n[2] = cn[0] * db[1] - cn[1] * db[0];
    rwp_cylinder_0056fd20(pair->b.plane.n, pair->b.plane.n);
  }
  dot = pair->b.plane.n[1] * pb[1];
  dot = dot + pair->b.plane.n[0] * pb[0];
  pair->b.plane.d = dot + pair->b.plane.n[2] * pb[2];
  return count;
project:   /* every vertex of the polygon moved along nrm onto the plane nrm . x = off */
  for (i = 0; i < pn; i++, pv += 4, res += 4) {
    tp = pv[1] * nrm[1];
    tp = tp + pv[0] * nrm[0];
    tp = tp + nrm[2] * pv[2];
    tp = off - tp;
    res[0] = tp * nrm[0] + pv[0];
    res[1] = tp * nrm[1] + pv[1];
    res[2] = tp * nrm[2] + pv[2];
  }
  return pn;
}
