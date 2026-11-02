#include "mfl_tier2.h"
#include "rwp_common.h"

/* Builds the constraint rows of one contact block: for every contact of the block a normal constraint (separating velocity with the
   penetration term) and, unless friction is off, two friction constraints along an orthonormal tangent pair, each stored into the
   constraint stream (57f8a0) after its two-body rows (580180) were filled. */

typedef struct MflMatBank { char *base; } MflMatBank;      /* 64 byte matrices, the translation row at +0x30 */
typedef struct MflObjOwner { unsigned char pad00[0x10]; MflMatBank *mats; } MflObjOwner;
typedef struct MflObj {
  MflObjOwner *owner;   /* 0x0 */
  unsigned idx;   /* 0x4: matrix index */
  unsigned char pad08[0x18];
  short slot;   /* 0x20 */
} MflObj;
typedef struct MflObjRec { int pad0; MflObj *obj; int pad8; } MflObjRec;      /* 12 bytes */
typedef struct MflObjList { unsigned char pad00[0x10]; MflObjRec *recs; } MflObjList;      /* +0x10: records by id */
/* Per contact properties (flag 1: tangent given, 2: friction limits given, 4: normal constraint values given, 8: contact disabled). */
typedef struct MflContactProps {
  int c18;   /* 0x0 */
  int c19;   /* 0x4 */
  float damping;   /* 0x8 */
  int c24;   /* 0xc */
  float tangent[3];   /* 0x10 */
  int flags;   /* 0x1c */
  float limit;   /* 0x20 */
  int c18f;   /* 0x24 */
  float limit2;   /* 0x28 */
  int c18g;   /* 0x2c */
} MflContactProps;
typedef struct MflContact {
  float pos[3];   /* 0x0 */
  float depth;   /* 0xc */
  MflContactProps *props;   /* 0x10 */
  float k[3];   /* 0x14 */
  unsigned char pad20[8];
} MflContact;
typedef struct MflContactBlock {
  float normal[3];   /* 0x0 */
  MflContact contact[1];   /* 0xc: `count` contacts of 0x28 bytes (the block is 0x100 bytes) */
  unsigned char pad34[0x78];
  unsigned count;   /* 0xac */
  unsigned char padb0[4];
  MflObjList *a;   /* 0xb4 */
  unsigned short a_id;   /* 0xb8 */
  unsigned char padba[2];
  MflObjList *b;   /* 0xbc */
  unsigned short b_id;   /* 0xc0 */
  unsigned char padc2[2];
  float stiff;   /* 0xc4 */
  float damp;   /* 0xc8 */
  unsigned char padcc[2];
  unsigned char flags;   /* 0xce */
  unsigned char padcf;
  float scale;   /* 0xd0 */
} MflContactBlock;
typedef struct MflConstraintStream {
  unsigned char pad00[0xac];
  int (*pairs)[2];   /* 0xac */
  unsigned char padb0[8];
  int *words;   /* 0xb8 */
  int nblocks;   /* 0xbc */
  unsigned char padc0[0x38];
  int cursor;   /* 0xf8 */
} MflConstraintStream;

#define AXIS(b) ((fabs((b)->normal[0]) < 0.99f) ? g_data_5f6cc4 : g_data_5f6cd0)
extern float g_data_5f6ca4[3], g_data_5f6cc4[3], g_data_5f6cd0[3], k_f_0_5f6cb0[];
extern void rwp_body_point_velocity(MflObj *h, float *p, float *out);
extern float *rwp_constraint_rows_two_body(float *row, float *n, float *r_a, float *r_b, MflObj *use_a, MflObj *use_b);
extern void rwp_constraint_store(MflConstraintStream *s, float *c);
extern void rwp_constraints_0057f750(MflConstraintStream *s);

/* TIER2: unvalidated 2026-11-02 authority=diffemu */
void rwp_constraints_0057fa00(MflConstraintStream *s, MflContactBlock *blk, float dt)
{
  MflObj *A, *B;
  float *posA, *posB;
  float F0, F2;
  double z, xx, yy, zz, ss;
  float p[3], p2;
  float dA[3], dB[3], rA[3], rB[3], w[3], t[3], u[3], v[3];
  float c[30];      /* 57f8a0 stores 27 words; the original frame leaves 120 bytes behind c */
  unsigned i;
  MflContact *rec;

  posA = g_data_5f6ca4;
  posB = g_data_5f6ca4;
  F0 = k_f_0_5f6cb0[blk->count] * blk->scale;
  A = (blk->a_id != 0xffff) ? blk->a->recs[blk->a_id].obj : 0;
  B = (blk->b_id != 0xffff) ? blk->b->recs[blk->b_id].obj : 0;
  if (A) posA = (float *)(A->owner->mats->base + A->idx * 64 + 0x30);
  if (B) posB = (float *)(B->owner->mats->base + B->idx * 64 + 0x30);
  s->words[s->cursor] = 0;
  s->nblocks++;
  s->pairs[s->cursor][0] = A ? A->slot : -1;
  s->pairs[s->cursor][1] = B ? B->slot : -1;
  for (i = 0, rec = blk->contact; i < blk->count; i++, rec++) {
    MflContactProps *pr = rec->props;
    float F1;
    rA[0] = rA[1] = rA[2] = 0;
    rB[0] = rB[1] = rB[2] = 0;
    if (pr && (pr->flags & 8)) continue;
    dA[0] = rec->pos[0] - posA[0];
    dA[1] = rec->pos[1] - posA[1];
    dA[2] = rec->pos[2] - posA[2];
    dB[0] = rec->pos[0] - posB[0];
    dB[1] = rec->pos[1] - posB[1];
    dB[2] = rec->pos[2] - posB[2];
    if (A) rwp_body_point_velocity(A, rec->pos, rA);
    if (B) rwp_body_point_velocity(B, rec->pos, rB);
    w[0] = (rA[0] - rB[0]) + rec->k[0];
    w[1] = (rA[1] - rB[1]) + rec->k[1];
    w[2] = (rA[2] - rB[2]) + rec->k[2];
    rwp_constraint_rows_two_body(c, blk->normal, dA, dB, A, B);
    if (rec->depth <= 0.0f) *(int *)&c[16] = *(int *)&rec->depth;
    else c[16] = 0;
    if (pr && (pr->flags & 4)) {
      *(int *)&c[18] = pr->c18;
      *(int *)&c[24] = pr->c24;
      *(int *)&c[19] = pr->c19;
      F1 = pr->damping;
    } else {
      F1 = blk->damp;
      c[18] = 0;
      c[19] = 0;
      c[24] = 0.8f;
    }
    c[23] = 3.4028235e38f;
    c[22] = 0;
    c[21] = 3.4028235e38f;
    c[20] = 0;
    {
      float dot = blk->normal[1] * w[1] + w[0] * blk->normal[0] + blk->normal[2] * w[2];
      if (dot < dt * -0.001f) c[17] = -(dot * F1);
      else c[17] = 0;
    }
    rwp_constraint_store(s, c);
    if (blk->flags & 1) continue;
    c[16] = 0; c[24] = 0; c[17] = 0; c[19] = 0; c[21] = 0; c[20] = 0;
    if (pr && (pr->flags & 1)) {
      *(int *)&t[0] = *(int *)&pr->tangent[0];
      *(int *)&t[1] = *(int *)&pr->tangent[1];
      *(int *)&t[2] = *(int *)&pr->tangent[2];
      u[0] = blk->normal[2] * t[1] - blk->normal[1] * t[2];
      u[1] = t[2] * blk->normal[0] - blk->normal[2] * t[0];
      u[2] = blk->normal[1] * t[0] - t[1] * blk->normal[0];
    } else {
      v[0] = blk->normal[1] * w[2] - blk->normal[2] * w[1];
      v[1] = blk->normal[2] * w[0] - w[2] * blk->normal[0];
      z = w[1] * blk->normal[0] - blk->normal[1] * w[0];
      if (z * z + v[1] * v[1] + v[0] * v[0] < 0.0001f) {
        v[0] = AXIS(blk)[2] * blk->normal[1] - AXIS(blk)[1] * blk->normal[2];
        v[1] = blk->normal[2] * AXIS(blk)[0] - AXIS(blk)[2] * blk->normal[0];
        z = AXIS(blk)[1] * blk->normal[0] - blk->normal[1] * AXIS(blk)[0];
      }
      yy = v[1] * v[1];
      zz = z * z;
      xx = v[0] * v[0];
      ss = xx; ss += yy; ss += zz;
      v[0] = v[0] * (1.0f / sqrt(ss));
      xx = v[0] * v[0];
      ss = xx; ss += yy; ss += zz;
      v[1] = v[1] * (1.0f / sqrt(ss));
      yy = v[1] * v[1];
      ss = yy; ss += xx; ss += zz;
      z = z * (1.0f / sqrt(ss));
      p[0] = blk->normal[2] * v[1] - blk->normal[1] * z;
      p[1] = z * blk->normal[0] - blk->normal[2] * v[0];
      p2 = blk->normal[1] * v[0] - v[1] * blk->normal[0];
      t[0] = (p[0] + v[0]) * 0.70710677f;
      t[1] = (p[1] + v[1]) * 0.70710677f;
      t[2] = p2 + z;
      u[0] = v[0] - p[0];
      u[1] = (v[1] - p[1]) * 0.70710677f;
      u[2] = z - p2;
      t[2] *= 0.70710677f;
      u[0] *= 0.70710677f;
      u[2] *= 0.70710677f;
    }
    if (pr && (pr->flags & 2)) {
      *(int *)&c[18] = pr->c18f;
      *(int *)&c[23] = *(int *)&pr->limit;
      F2 = pr->limit;
    } else {
      F2 = F0 * blk->stiff * 0.70710677f;
      c[18] = 0;
      c[23] = F2;
    }
    c[22] = -F2;
    rwp_constraint_rows_two_body(c, t, dA, dB, A, B);
    rwp_constraint_store(s, c);
    if (pr && (pr->flags & 2)) {
      *(int *)&c[18] = pr->c18g;
      *(int *)&c[23] = *(int *)&pr->limit2;
      c[22] = -pr->limit2;
    }
    rwp_constraint_rows_two_body(c, u, dA, dB, A, B);
    rwp_constraint_store(s, c);
  }
  rwp_constraints_0057f750(s);
}
