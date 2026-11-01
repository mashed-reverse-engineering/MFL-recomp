#include "mfl_tier2.h"
#include "rwp_common.h"
#include <math.h>

/* Swept contact test of two bodies (conservative advancement): finds the earliest time t (< limit) at which the geometry of body A and body B
   touch while they move with their velocities.  The bounding spheres of both bodies give a first t (ray against the sphere of the summed
   radii), then up to five rounds move both bodies to t, collect the contacts of their real shapes (585f20 + 586f20), and advance t by the
   largest step that cannot skip a contact.  *info_a / *info_b (when given) are lowered to the t found.  Returns 0 when the bodies do not touch
   within the limit, else 1. */

/* Body set: parallel arrays indexed by the body number (see 57c530). */
typedef struct MflBodyVel {
  float lin[3];
  int pad0c;
  float ang[3];   /* 0x10 */
  int pad1c;
} MflBodyVel;
typedef struct MflBodySet {
  RwMatrix *matrix;   /* 0x0 */
  int pad04;
  MflBodyVel *vel;   /* 0x8 */
  int pad0c;
  float (*quat)[4];   /* 0x10 */
} MflBodySet;
typedef struct MflBodyOwner {
  unsigned char pad00[0x10];
  MflBodySet *set;   /* 0x10 */
} MflBodyOwner;
typedef struct MflBodyNode {
  MflBodyOwner *owner;   /* 0x0 */
  int index;   /* 0x4 */
  float origin[3];   /* 0x8: offset of the shape origin in the body frame */
} MflBodyNode;
typedef struct MflBodyItem {
  void *shape;
  MflBodyNode *node;   /* 0x4 */
  void *xf;
} MflBodyItem;
typedef struct MflItemList {
  unsigned char pad00[0x10];
  MflBodyItem *items;   /* 0x10 */
} MflItemList;

struct MflShapeData;
typedef struct MflMaterial {
  unsigned char pad00[0x24];
  float (*measure)(struct MflShapeData *shape);   /* 0x24 */
  unsigned char pad28[0x18];
  unsigned flags;   /* 0x40: bit 1 = the shape frame is not applied to the transform */
} MflMaterial;
typedef struct MflShapeData {
  unsigned char pad00[0x4c];
  float margin;   /* 0x4c */
  unsigned char pad50[0xc];
  MflMaterial *material;   /* 0x5c */
} MflShapeData;
typedef struct MflBodyRef {
  MflItemList *list;   /* 0x0 */
  unsigned short slot;   /* 0x4 */
  unsigned short pad06;
  MflShapeData *shape;   /* 0x8 */
  int pad0c;
  RwMatrix *xf;   /* 0x10 */
} MflBodyRef;
typedef struct MflSweepPair {
  MflBodyRef *a;
  MflBodyRef *b;   /* 0x4 */
} MflSweepPair;

/* The collected contact of the two shapes (one record of the collector at world + 0x37c, see 585f20). */
typedef struct MflContact {
  float a_n[3];   /* 0x0: plane of shape A, written by 586f20 */
  float a_d;
  unsigned char pad10[0x40];
  float *a_pts;   /* 0x50: 16 byte points */
  unsigned short a_count;   /* 0x54 */
  unsigned char pad56[0x2a];
  float b_n[3];   /* 0x80 */
  float b_d;
  unsigned char pad90[0x40];
  float *b_pts;   /* 0xd0 */
  unsigned short b_count;   /* 0xd4 */
  unsigned char padd6[0x2a];
  float nrm[3];   /* 0x100: query direction */
  float off;   /* 0x10c */
  float margin;   /* 0x110 */
} MflContact;
typedef struct MflCollector {
  unsigned char pad00[0x2c];
  MflContact *contacts;   /* 0x2c */
  unsigned count;   /* 0x30 */
  unsigned char pad34[8];
  unsigned points_used;   /* 0x3c */
} MflCollector;

typedef struct MflSweepWorld {
  float f00;
  unsigned char pad04[0x20];
  float f24;   /* 0x24 */
  float f28;   /* 0x28 */
} MflSweepWorld;

extern void *rwp_pipeline_00572c80(MflItemList *list, int slot, float *sphere);
extern int rwp_sphere_0056c0b0(MflShapeData *shape, RwMatrix *xf, float *sphere);
extern float rwp_cylinder_0056fd20(float *out, float *in);
extern int rwp_batch_0057c530(MflBodySet *set, int index, float dt);
extern int rwp_aggregate_00585f20(MflCollector *cs, MflSweepWorld *w, MflSweepPair *pair, float limit);
extern int rwp_aggregate_00586f20(float *a, unsigned na, float *b, unsigned nb, float diff, float off, float *nrm, float *margin, int *kind_a,
                                  int *kind_b, MflContact *pair, float *res);
extern void rwp_convexhullmassprops_00583ca0(MflBodyVel *vel, float *centre, float *point, float *out);
extern RwMatrix *RwMatrixTranslate(RwMatrix *m, RwV3d *v, int op);
extern RwMatrix *RwMatrixTransform(RwMatrix *m, RwMatrix *tr, int op);

#define FMAX 3.4028235e+38f
#define CPY(dst, src) (*(int *)&(dst) = *(int *)&(src))

/* Moves one body to the pose at time t for the shape query: orientation and position of the body are advanced by 57c530, the frame matrix of the
   shape is rebuilt from the quaternion (R from q with s = 2 / |q|^2) and the shape offset, then the saved pose is put back.  centre = position at t. */
#define POSE(REF, NODE, CEN) do { \
    MflBodySet *set_ = (NODE)->owner->set; \
    int ix_ = (NODE)->index; \
    float *q_ = set_->quat[ix_]; \
    float *tr_ = &set_->matrix[ix_].pos.x; \
    int sq0_ = ((int *)q_)[0], sq1_ = ((int *)q_)[1], sq2_ = ((int *)q_)[2], sq3_ = ((int *)q_)[3]; \
    int st0_ = ((int *)tr_)[0], st1_ = ((int *)tr_)[1], st2_ = ((int *)tr_)[2]; \
    float s_, sx_, sy_, sz_, wx_, wy_, wz_, xx_, yy_, zz_, xy_, xz_, yz_; \
    RwV3d neg_; \
    RwMatrix *xf_; \
    if (t > 0.0f) rwp_batch_0057c530(set_, ix_, t); \
    CPY((CEN)[0], tr_[0]); CPY((CEN)[1], tr_[1]); CPY((CEN)[2], tr_[2]); \
    s_ = 2.0f / (((q_[0] * q_[0] + q_[1] * q_[1]) + q_[2] * q_[2]) + q_[3] * q_[3]); \
    sx_ = s_ * q_[0]; \
    sy_ = q_[1] * s_; \
    sz_ = q_[2] * s_; \
    wx_ = q_[3] * sx_; \
    wy_ = q_[3] * sy_; \
    wz_ = q_[3] * sz_; \
    xx_ = sx_ * q_[0]; \
    yy_ = sy_ * q_[1]; \
    zz_ = sz_ * q_[2]; \
    yz_ = sz_ * q_[1]; \
    xz_ = sx_ * q_[2]; \
    xy_ = sy_ * q_[0]; \
    xf_ = (REF)->xf; \
    xf_->flags = 3; \
    xf_->right.x = 1.0f - (zz_ + yy_); \
    xf_->right.y = wz_ + xy_; \
    xf_->right.z = xz_ - wy_; \
    xf_->up.x = xy_ - wz_; \
    xf_->up.y = 1.0f - (zz_ + xx_); \
    xf_->up.z = yz_ + wx_; \
    xf_->at.x = xz_ + wy_; \
    xf_->at.y = yz_ - wx_; \
    xf_->at.z = 1.0f - (yy_ + xx_); \
    CPY(xf_->pos.x, tr_[0]); CPY(xf_->pos.y, tr_[1]); CPY(xf_->pos.z, tr_[2]); \
    neg_.x = -(NODE)->origin[0]; \
    neg_.y = -(NODE)->origin[1]; \
    neg_.z = -(NODE)->origin[2]; \
    RwMatrixTranslate(xf_, &neg_, 1); \
    if (!((REF)->shape->material->flags & 2)) RwMatrixTransform(xf_, (RwMatrix *)(REF)->shape, 1); \
    ((int *)q_)[0] = sq0_; ((int *)q_)[1] = sq1_; ((int *)q_)[2] = sq2_; ((int *)q_)[3] = sq3_; \
    ((int *)tr_)[0] = st0_; ((int *)tr_)[1] = st1_; ((int *)tr_)[2] = st2_; \
  } while (0)

int rwp_constraints_00583050(MflSweepWorld *w, MflSweepPair *pair, float *info_a, float *info_b)
{
  float sphere_a[4], sphere_b[4], gap[3], dir[3], rel[3];
  float res[64];
  float t, best, limit, step, reach, twice, minstep, speed, d2, dist, rsum, thresh, dot, c, disc, reachsum, diffr, extent, margin;
  float xa, lena, lenb, sum;
  float hmargin, tval, newt, u, vrel;
  float dxv, dyv, dzv;
  double vacc;
  float vel_ab[6];   /* surface velocities of A (0..2) and B (3..5) at the contact point */
  int kind_a, kind_b;
  MflBodyNode *node_a, *node_b;
  MflBodyVel *va, *vb;
  MflContact *ct;
  MflCollector *cs;
  unsigned n, j, k;
  float *p;

  xa = pair->a->shape->material->measure(pair->a->shape) + 2.0f * pair->a->shape->margin;
  reach = ((pair->b->shape->material->measure(pair->b->shape) + 2.0f * pair->b->shape->margin) + xa) * w->f28;
  minstep = w->f24 * w->f00;

  if (info_a) {
    node_a = pair->a->list->items[pair->a->slot].node;
    va = node_a->owner->set->vel + node_a->index;
    lena = (float)sqrt(va->ang[0] * va->ang[0] + va->ang[1] * va->ang[1] + va->ang[2] * va->ang[2]);
    rwp_pipeline_00572c80(pair->a->list, pair->a->slot, sphere_a);
  } else {
    rwp_sphere_0056c0b0(pair->a->shape, pair->a->xf, sphere_a);
    node_a = 0;
    va = 0;
    lena = 0.0f;
    CPY(limit, *info_b);
  }
  if (info_b) {
    node_b = pair->b->list->items[pair->b->slot].node;
    vb = node_b->owner->set->vel + node_b->index;
    lenb = (float)sqrt(vb->ang[0] * vb->ang[0] + vb->ang[1] * vb->ang[1] + vb->ang[2] * vb->ang[2]);
    rwp_pipeline_00572c80(pair->b->list, pair->b->slot, sphere_b);
    if (info_a) {
      if (*info_a <= *info_b) CPY(limit, *info_a);
      else CPY(limit, *info_b);
      rel[0] = va->lin[0] - vb->lin[0];
      rel[1] = va->lin[1] - vb->lin[1];
      rel[2] = va->lin[2] - vb->lin[2];
    } else {
      rel[0] = -vb->lin[0];
      rel[1] = -vb->lin[1];
      rel[2] = -vb->lin[2];
    }
  } else {
    rwp_sphere_0056c0b0(pair->b->shape, pair->b->xf, sphere_b);
    CPY(limit, *info_a);
    node_b = 0;
    vb = 0;
    lenb = 0.0f;
    CPY(rel[0], va->lin[0]);
    CPY(rel[1], va->lin[1]);
    CPY(rel[2], va->lin[2]);
  }

  /* bounding spheres: first t from the ray of the relative motion against the sphere of the summed radii */
  gap[0] = sphere_a[0] - sphere_b[0];
  gap[1] = sphere_a[1] - sphere_b[1];
  gap[2] = sphere_a[2] - sphere_b[2];
  rsum = sphere_b[3] + sphere_a[3];
  dist = rwp_cylinder_0056fd20(dir, gap);
  d2 = rel[1] * rel[1];
  d2 = d2 + rel[0] * rel[0];
  t = 0.0f;
  speed = (float)sqrt(d2 = d2 + rel[2] * rel[2]);
  twice = 2.0f * reach;
  thresh = twice + rsum;
  if (dist > thresh) {
    dot = gap[1] * rel[1] + gap[0] * rel[0] + gap[2] * rel[2];
    c = (dist - thresh) * (dist + thresh);
    if (dot > -9.99999997e-07f) return 0;
    if (c * d2 > dot * dot) return 0;
    t = (-dot - (float)sqrt(dot * dot - c * d2)) / d2;
  }
  if (t < minstep) t = minstep;
  sum = lenb + lena;
  if (sum > 1.17549435e-38f) step = 0.78539819f / sum;
  else step = FMAX;
  if (t >= limit) return 0;

  extent = sphere_b[3] * lenb + sphere_a[3] * lena;
  reachsum = pair->b->shape->margin + pair->a->shape->margin;
  diffr = pair->b->shape->margin - pair->a->shape->margin;
  extent = extent + speed;
  k = 0;
  for (;;) {
    if (node_a) POSE(pair->a, node_a, sphere_a);
    if (node_b) POSE(pair->b, node_b, sphere_b);
    cs = (MflCollector *)((char *)w + 0x37c);
    cs->count = 0;
    cs->points_used = 0;
    if (!rwp_aggregate_00585f20(cs, w, pair, (limit - t) * extent)) return 0;
    ct = cs->contacts;
    margin = reachsum + ct->margin;
    n = rwp_aggregate_00586f20(ct->a_pts, ct->a_count, ct->b_pts, ct->b_count, diffr, ct->off, ct->nrm, &margin, &kind_a, &kind_b, ct, res);
    hmargin = margin - reachsum;
    best = limit;
    minstep = hmargin;
    j = 0;
    p = res;
    if (n > 0) {
      do {
        rwp_convexhullmassprops_00583ca0(va, sphere_a, p, vel_ab);
        rwp_convexhullmassprops_00583ca0(vb, sphere_b, p, vel_ab + 3);
        dxv = vel_ab[0] - vel_ab[3];
        dyv = vel_ab[1] - vel_ab[4];
        dzv = vel_ab[2] - vel_ab[5];
        vacc = ct->nrm[1] * dyv;
        vacc = vacc + ct->nrm[0] * dxv;
        vacc = vacc + ct->nrm[2] * dzv;
        vrel = (float)vacc;
        if (vrel < 0.0f) {
          float q1, q2;
          q1 = ((ct->b_n[0] * p[0] + p[1] * ct->b_n[1]) + p[2] * ct->b_n[2] - ct->b_d) /
               ((ct->b_n[0] * ct->nrm[0] + ct->nrm[1] * ct->b_n[1]) + ct->nrm[2] * ct->b_n[2]);
          q2 = ((p[1] * ct->a_n[1] + p[0] * ct->a_n[0]) + ct->a_n[2] * p[2] - ct->a_d) /
               ((ct->nrm[1] * ct->a_n[1] + ct->nrm[0] * ct->a_n[0]) + ct->a_n[2] * ct->nrm[2]);
          tval = (q1 - q2) - reachsum;
          if (tval < minstep) tval = minstep;
          if (tval <= reach) goto done;
          u = tval - twice;
          newt = -(u / vrel);
          if (best > newt) best = newt;
        }
        j++;
        p += 4;
      } while (j < n);
    }
    if (best <= 0.0f) goto done;
    if (best > step) best = step;
    t = best + t;
    if (t > limit) return 0;
    k++;
    if (k >= 5) break;
  }
done:
  if (info_a) {
    if (*info_a > t) CPY(*info_a, t);
  }
  if (info_b) {
    if (*info_b > t) CPY(*info_b, t);
  }
  return 1;
}
