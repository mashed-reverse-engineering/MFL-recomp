#include "mfl_tier2.h"
#include <math.h>

/* One contact point of a hit (0x28 bytes): position, penetration depth, and the feature ids of the two shapes. */
typedef struct MflHitPt {
  float p[3];
  float depth;   /* 0xc */
  int pad10[4];
  int id0;   /* 0x20 */
  int id1;   /* 0x24 */
} MflHitPt;
/* A hit: a contact normal and up to 4 points (0xe0 bytes).  `next` and `rep` are scratch links of the reduction. */
typedef struct MflHit {
  float n[3];
  MflHitPt pt[4];   /* 0xc */
  unsigned int count;   /* 0xac */
  unsigned int keep_b0;   /* 0xb0: not part of the data, kept when a hit is copied into the output */
  unsigned char pad_b4[0x18];
  unsigned char kind0;   /* 0xcc */
  unsigned char kind1;   /* 0xcd */
  unsigned char pad_ce[2];
  unsigned char pad_d0[8];
  unsigned int rep;   /* 0xd8: output hits, index of the group's first input hit */
  struct MflHit *next;   /* 0xdc */
} MflHit;

extern float rwp_cylinder_0056fd20(float *out, float *in);
extern int rwp_generalprimitive_0058a1f0(MflHit *plane, MflHit *list, float tol);

/* Copies a hit into an output slot without touching the slot's field 0xb0. */
#define HIT_COPY(dst, src) do { unsigned int keep_ = (dst)->keep_b0; *(dst) = *(src); (dst)->keep_b0 = keep_; } while (0)

/* Culls the hits p and q of one bucket: a hit of kind 2 on the one side removes a hit of a lower kind on the other side when both share
   the feature id, the kind 2 hit is the shallower one and its plane covers the other hit's first point (0058a1f0).  KIND / ID choose the
   field pair (pass 1: kind1 / id0, pass 2: kind0 / id1). */
#define HIT_CULL(KIND, ID)                                                                                   \
  do {                                                                                                         \
    if (p->KIND == 2) {                                                                                        \
      if (q->KIND < 2 && p->pt[0].ID == q->pt[0].ID && p->pt[0].depth < q->pt[0].depth                        \
          && rwp_generalprimitive_0058a1f0(p, q, tol))                                                         \
        q->count = 0;                                                                                          \
    } else if (q->KIND == 2 && p->KIND < 2 && p->pt[0].ID == q->pt[0].ID && p->pt[0].depth > q->pt[0].depth    \
               && rwp_generalprimitive_0058a1f0(q, p, tol)) {                                                  \
      p->count = 0;                                                                                            \
      pdone = 1;                                                                                               \
    }                                                                                                          \
  } while (0)

/* Appends the point `cand` to the output hit o unless an existing point is within tol * 0.001 (sum of the axis distances). */
#define HIT_ADD_PT(cand)                                                                                       \
  do {                                                                                                         \
    unsigned int cnt_ = o->count, kk_;                                                                         \
    for (kk_ = 0; kk_ < cnt_; kk_++) {                                                                         \
      float thr_ = tol * 0.001f;                                                                               \
      float dz_ = (float)fabs((cand)->p[2] - o->pt[kk_].p[2]);                                                 \
      float dy_ = (float)fabs((cand)->p[1] - o->pt[kk_].p[1]);                                                 \
      float dx_ = (float)fabs((cand)->p[0] - o->pt[kk_].p[0]);                                                 \
      float s_ = dz_ + dy_;                                                                                    \
      s_ = s_ + dx_;                                                                                           \
      if (s_ < thr_) break;                                                                                    \
    }                                                                                                          \
    if (kk_ == cnt_) {                                                                                         \
      o->pt[cnt_] = *(cand);                                                                                   \
      o->count++;                                                                                              \
    }                                                                                                          \
  } while (0)

/* Reduces n hits to at most cap output hits.  With `merge` set, hits that a hit of kind 2 covers are culled first (two passes over buckets
   keyed by the feature ids, 16 buckets for n >= 5).  The remaining hits are grouped by normal (dot > 0.99, or a first point within tol * 0.01
   of the other plane); each group becomes one output hit: a single hit is copied, up to 4 points are merged (duplicates dropped, normal =
   normalised sum weighted by the point counts), more points are reduced to the extremes along the direction from the deepest point to the
   centroid and along the normal times it.  Returns the number of output hits. */
/* TIER2: unvalidated 2026-11-10 authority=static */
int rwp_generalprimitive_00589690(MflHit *recs, unsigned int n, int merge, MflHit *out, unsigned int cap, float tol)
{
  /* sumn is passed to 56fd20 by address; the 32 bytes behind it in the original frame hold these locals (esp+0x20..0x3c), so they are kept
     together in the same order.  x38 is the slot the original reuses for the last `fa` flag of the grouping and the list walk of the merge. */
  struct {
    float sumn[3];
    unsigned int nout;
    MflHitPt *pmin2, *pmax2;
    int x38;
    MflHitPt *pmin1;
  } fr;
  MflHit *heads[16];
  MflHit *p, *q, *r, *g, *rep, *o, **pp;
  MflHitPt *pbest = 0, *pt;
  unsigned int nb, live, i, j, b, sum, last, nout, total, key, cnt, k;
  float cv[3], u[3], w[3], mind, max1, max2, min1, min2, s1, s2;
  float d[3], dot, t1, t2, tol2, sa, sb, inv, wt, usum;
  int fa, fb, pdone;
#define sumn fr.sumn
#define pmin1 fr.pmin1
#define pmax2 fr.pmax2
#define pmin2 fr.pmin2

  pmin1 = 0; pmax2 = 0; pmin2 = 0;
  if (n == 0 || cap == 0) return 0;
  if (n == 1) {
    if (recs[0].count == 0) return 0;
    HIT_COPY(out, recs);
    return 1;
  }
  if (merge) {
    nb = 1;
    live = 0;
    if (n < 5) {
      heads[0] = 0;
      for (i = 0; i < n; i++)
        if (recs[i].count) {
          recs[i].next = heads[0];
          heads[0] = &recs[i];
          live++;
        }
    } else {
      nb = 16;
      for (i = 0; i < 16; i++) heads[i] = 0;
      for (i = 0; i < n; i++)
        if (recs[i].count) {
          key = recs[i].pt[0].id0 & 0xf;
          recs[i].next = heads[key];
          heads[key] = &recs[i];
        }
    }
    if (nb != 1 || live >= 2) {
      for (b = 0; b < nb; b++)
        for (p = heads[b]; p; p = p->next)
          for (q = p->next, pdone = 0; q && !pdone; q = q->next) HIT_CULL(kind1, id0);
      if (nb == 1) {
        pp = &heads[0];
        do {
          r = *pp;
          if (r->count == 0) *pp = r->next;
          else pp = &r->next;
        } while (*pp);
      } else {
        for (i = 0; i < 16; i++) heads[i] = 0;
        for (i = 0; i < n; i++)
          if (recs[i].count) {
            key = recs[i].pt[0].id1 & 0xf;
            recs[i].next = heads[key];
            heads[key] = &recs[i];
          }
      }
      for (b = 0; b < nb; b++)
        for (p = heads[b]; p; p = p->next)
          for (q = p->next, pdone = 0; q && !pdone; q = q->next) HIT_CULL(kind0, id1);
    }
  }
  sum = 0;
  last = 0;
  for (i = 0; i < n; i++)
    if (recs[i].count) {
      sum += recs[i].count;
      last = i;
    }
  if (sum == 0) return 0;
  if (sum == recs[last].count) {
    HIT_COPY(out, &recs[last]);
    return 1;
  }
  nout = 0;
  for (i = 0; i < n; i++) {
    r = &recs[i];
    if (!r->count) continue;
    for (j = 0; j < nout; j++) {
      g = &recs[out[j].rep];
      t1 = g->n[1] * r->n[1];
      t2 = r->n[0] * g->n[0];
      dot = t1 + t2;
      t1 = r->n[2] * g->n[2];
      dot = dot + t1;
      if (!(dot > 0.99f)) continue;
      d[0] = r->pt[0].p[0] - g->pt[0].p[0];
      d[1] = r->pt[0].p[1] - g->pt[0].p[1];
      d[2] = r->pt[0].p[2] - g->pt[0].p[2];
      tol2 = tol * 0.01f;
      sa = r->n[1] * d[1];
      t1 = r->n[0] * d[0];
      sa = sa + t1;
      t1 = r->n[2] * d[2];
      sa = sa + t1;
      fa = fabs(sa) < tol2;
      fr.x38 = fa;
      sb = g->n[1] * d[1];
      t1 = d[0] * g->n[0];
      sb = sb + t1;
      t1 = g->n[2] * d[2];
      sb = sb + t1;
      fb = fabs(sb) < tol2;
      if (fa || fb) {
        r->next = g->next;
        g->next = r;
        break;
      }
    }
    if (j == nout && nout < cap) {
      out[nout].rep = i;
      nout++;
      r->next = 0;
    }
  }
  fr.nout = nout;
  if (nout == 0) return 0;
  for (b = 0; b < nout; b++) {
    o = &out[b];
    rep = &recs[o->rep];
    if (rep->next == 0) {
      HIT_COPY(o, rep);
      continue;
    }
    total = 0;
    for (r = rep; r; r = r->next) total += r->count;
    cv[0] = cv[1] = cv[2] = 0.0f;
    sumn[0] = sumn[1] = sumn[2] = 0.0f;
    mind = 3.4028235e38f;
    for (r = rep; r; r = r->next) {
      wt = (float)r->count;
      sumn[0] = wt * r->n[0] + sumn[0];
      sumn[1] = r->n[1] * wt + sumn[1];
      sumn[2] = r->n[2] * wt + sumn[2];
      for (k = 0; k < r->count; k++) {
        pt = &r->pt[k];
        cv[0] += pt->p[0];
        cv[1] += pt->p[1];
        cv[2] += pt->p[2];
        if (mind > pt->depth) {
          mind = pt->depth;
          pbest = pt;
        }
      }
    }
    rwp_cylinder_0056fd20(sumn, sumn);
    inv = 1.0f / (float)total;
    cv[0] = cv[0] * inv;
    cv[1] = cv[1] * inv;
    cv[2] = inv * cv[2];
    if (total <= 4) {
      HIT_COPY(o, rep);
      for (r = rep->next; r; r = r->next) {
        for (cnt = r->count, pt = &r->pt[0]; cnt > 0; cnt--, pt++) HIT_ADD_PT(pt);
      }
      fr.x38 = 0;      /* the walk pointer the original keeps there ends at the list's null */
      o->n[0] = sumn[0];
      o->n[1] = sumn[1];
      o->n[2] = sumn[2];
      continue;
    }
    u[0] = cv[0] - pbest->p[0];
    u[1] = cv[1] - pbest->p[1];
    u[2] = cv[2] - pbest->p[2];
    usum = (float)fabs(u[2]) + (float)fabs(u[1]);
    usum = usum + (float)fabs(u[0]);
    if (usum < 1.1754944e-38f) u[0] = 1.0f;
    w[0] = u[2] * sumn[1] - u[1] * sumn[2];
    w[1] = sumn[2] * u[0] - u[2] * sumn[0];
    w[2] = u[1] * sumn[0] - u[0] * sumn[1];
    max2 = max1 = -3.4028235e38f;
    min2 = min1 = 3.4028235e38f;
    for (r = rep; r; r = r->next) {
      for (k = 0; k < r->count; k++) {
        pt = &r->pt[k];
        s1 = pt->p[1] * u[1];
        t1 = u[0] * pt->p[0];
        s1 = s1 + t1;
        t1 = pt->p[2] * u[2];
        s1 = s1 + t1;
        if (s1 > max1) {
          max1 = s1;
          pbest = pt;
        }
        if (s1 < min1) {
          min1 = s1;
          pmin1 = pt;
        }
        s2 = pt->p[1] * w[1];
        t1 = w[0] * pt->p[0];
        s2 = s2 + t1;
        t1 = pt->p[2] * w[2];
        s2 = s2 + t1;
        if (s2 > max2) {
          max2 = s2;
          pmax2 = pt;
        }
        if (s2 < min2) {
          min2 = s2;
          pmin2 = pt;
        }
      }
    }
    HIT_COPY(o, rep);
    o->n[0] = sumn[0];
    o->n[1] = sumn[1];
    o->n[2] = sumn[2];
    o->pt[0] = *pbest;
    o->count = 1;
    if (pmax2 != pbest) HIT_ADD_PT(pmax2);
    if (pmin1 != pbest && pmin1 != pmax2) HIT_ADD_PT(pmin1);
    if (pmin2 != pbest && pmin2 != pmax2 && pmin2 != pmin1) HIT_ADD_PT(pmin2);
  }
  return nout;
#undef sumn
#undef pmin1
#undef pmax2
#undef pmin2
}
