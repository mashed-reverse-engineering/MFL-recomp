/* rwp_pipeline_00571c60 (0x00571c60): solver input builder - gathers forces of the group items, builds the joint / contact rows and runs the per-phase solver steps */
#include "mfl_tier2.h"
#include <float.h>
#include <math.h>

extern int g_data_9488e4;
extern void rwp_batch_005782b0(void *s, void *a3);
extern void rwp_batch_0057f670(void *s, int n);
extern void rwp_constraint_linear_row_axis(void *s, void *joint, int rate, int dt);
extern void rwp_constraints_0057fa00(void *s, void *node, int dt);
extern void rwp_batch_0057f6d0(void *s);
extern void rwp_batch_0057ed30(int a0, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12, int a13, int dt);
extern void rwp_grid_00577c80(void *a5, void *a4, int a2, int a3);
extern float rwp_cylinder_0056fd20(float *out, float *in);

/* by-value blocks of the callees (the original copies them onto the stack) */
typedef struct MflW3 { int w[3]; } MflW3;
typedef struct MflW4 { int w[4]; } MflW4;
typedef struct MflW38 { int w[14]; } MflW38;
typedef struct MflW88 { int w[34]; } MflW88;

extern void rwp_batch_0057e3f0(MflW4 head, MflW3 a34, MflW3 a40, MflW3 a4c, MflW3 a58, MflW3 a64, MflW3 a1c, MflW3 a28, MflW4 tail);
extern void rwp_batch_0057daa0(MflW4 head, MflW3 a34, MflW3 a40, MflW3 a4c, MflW3 a58, MflW3 a64, MflW3 a1c, MflW3 a28, MflW4 tail);
extern void rwp_batch_0057d720(void *a4, int h0, int h1, int h2, int h3, int a0, int a1, int a2, int b0, int b1, int b2);
extern void rwp_rows_gather_strided(void *out, int table, int u3, int u4, void *list);
extern void rwp_joint_accumulators_rebuild(int p0, int p1, void *a30, void *a3c, void *a48, int p5, int p6, MflW4 head, MflW3 aac, MflW3 a3c0, MflW3 a30v);
extern void rwp_joint_matrix_columns_build(int p0, int p1, int v0, int v1, int v2, int p3, int p4, int c0, int c1, int c2);
extern void rwp_bodies_integrate_list_ordered(int p0, int p1, int p2, int p3, MflW3 v30, int p5, int p6, int p7, int p8, int one);
extern void rwp_batch_0057c4a0(int p0, int p1, int p2, int p3, int dt);

/* a list of ids: pointer and count */
typedef struct MflIdList { int *p; unsigned int n; } MflIdList;

/* a 12 byte descriptor of the solver state: array pointer, count, capacity */
typedef struct MflArr3 { int *p; int n; int cap; } MflArr3;

typedef struct MflSolve {
  int head[4];   /* 0x0 */
  MflArr3 a[23];   /* 0x10 + 12 * k */
  int dtw;   /* 0x124: step time (raw bits) */
  int f128;
  int pad12c[2];
  int f134;
  int f138;
  int pad13c;
  int f140;
  float lim144;
  int pad148;
  int f14c;
  int pad150[2];
  int f158;
} MflSolve;

typedef void (*MflStepFn)(MflW3 x94, MflW88 a4, MflW3 a10, MflW3 a1c, MflW3 a28, MflW3 a118, MflW3 a10c, int dt);
typedef void (*MflPostFn)(char *p0, char *p1, char *p2, char *a5, MflW88 a4, MflW3 s118, MflW3 sa, MflW3 sb, MflW3 a4v, MflW3 sc, MflSolve *s, MflW38 st, void *w2);

typedef struct MflBody { unsigned char pad0[4]; int index; unsigned char pad8[0xc]; float damp_lin; float damp_ang; unsigned char pad1c[4]; unsigned short slot; } MflBody;
typedef struct MflLink { void *shape; MflBody *body; int pad8; } MflLink;
typedef struct MflNode { unsigned char pad0[0xac]; int count; int tag; unsigned char padb4[0x28]; struct MflNode *next; } MflNode;
typedef struct MflItem {
  unsigned char pad0[0xc];
  unsigned short nlinks;   /* 0xc */
  unsigned short pade;
  MflLink *links;   /* 0x10 */
  unsigned short n18;   /* 0x14 */
  unsigned short pad16;
  char **arr18;   /* 0x18 */
} MflItem;
typedef struct MflGroup {
  MflItem **items;   /* 0x0 */
  MflNode *chain;   /* 0x4 */
  char **arr8;   /* 0x8 */
  unsigned int nitems;   /* 0xc */
  unsigned int n8;   /* 0x10 */
  int s14;   /* 0x14 */
  unsigned int flags;   /* 0x18 */
  unsigned char pad1c[0xc];
} MflGroup;
typedef struct MflBatch { int c4; unsigned int n; int pad8[2]; MflGroup *groups; } MflBatch;

/* the row set of the caller followed by its output window */
typedef struct MflRowPair { int a; int p; } MflRowPair;
typedef struct MflRowSet {
  MflRowPair r[11];
  int *wptr;   /* 0x58 */
  int wrows;   /* 0x5c */
  int wcount;   /* 0x60 */
} MflRowSet;

typedef struct MflWorld {
  int dtw;   /* 0x0 */
  int w4;
  unsigned char pad8[0x44];
  float grav[3];   /* 0x4c */
  int mode;   /* 0x58: 2 = the SSE solver */
  int w5c;
  int pad60;
  float soft;   /* 0x64 */
  unsigned int flags;   /* 0x68 */
} MflWorld;

#define AT(p, o) ((char *)(p) + (o))
#define W3(p, o) (*(MflW3 *)AT(p, o))
#define FP(p, o) (*(float **)AT(p, o))
#define SW3(k) (*(MflW3 *)&S->a[k])
#define IP(p, o) (*(int *)AT(p, o))

/* TIER2: unvalidated 2026-11-02 authority=diffemu */
void rwp_pipeline_00571c60(MflRowSet *rs, MflIdList *a1, MflSolve *S, char *a3, char *a4, char *a5, char *a6, MflBatch *b, MflWorld *w,
                           int dt9, MflStepFn fn_step, void *w_f8, MflPostFn fn_post, char *w2)
{
  unsigned int total, gi, ii, li, si, i, n;
  int idx, dt0, k;
  MflGroup *g;
  MflItem *it;
  MflBody *bd;
  MflNode *c;
  MflArr3 *ar;
  float *f, *v, *m, *R, *F, *V, *M, *P;
  float w0, w1, sA, uA, vA, last, s, dv, q, disc, r, t;
  volatile float t3c, t48, t40, t44, t34, t38;
  float inv, nrm[3];
  MflW4 tail;
  MflGroup *gp;
  MflIdList *lst;
  int *rec;

  total = 0;
  if (b->n) {
    gp = b->groups;
    for (i = b->n; i; i--, gp++)
      if (!(gp->flags & 3)) total += gp->s14;
  }
  rwp_batch_005782b0(S, a3);
  dt0 = w->dtw;
  rs->wrows = 0;
  a1->n = 0;
  if (b->n != 0) {
    for (gi = 0; gi != b->n; gi++) {
      g = &b->groups[gi];
      if (g->flags & 3) continue;
      for (ii = 0; ii < g->nitems; ii++) {
        it = g->items[ii];
        for (li = 0; li < it->nlinks; li++) {
          bd = it->links[li].body;
          idx = bd->index;
          f = (float *)AT(rs->r[6].a, idx * 32);
          v = (float *)AT(rs->r[1].a, idx * 32);
          m = (float *)AT(rs->r[4].a, idx * 48);
          bd->slot = (unsigned short)rs->wrows;
          rs->wptr[rs->wrows] = idx;
          rs->wrows++;
          f[0] = m[3] * w->grav[0] + f[0];
          f[1] = m[3] * w->grav[1] + f[1];
          f[2] = m[3] * w->grav[2] + f[2];
          f[0] = (-(m[3] * bd->damp_lin)) * v[0] + f[0];
          f[1] = (-(m[3] * bd->damp_lin)) * v[1] + f[1];
          f[2] = (-(m[3] * bd->damp_lin)) * v[2] + f[2];
          if (((int *)rs->r[3].a)[idx] & 2) {
            R = (float *)AT(rs->r[0].a, idx * 64);
            w0 = (R[1] * v[5] + R[0] * v[4]) + R[2] * v[6];
            w1 = (v[5] * R[5] + v[4] * R[4]) + v[6] * R[6];
            t3c = (v[5] * R[9] + R[8] * v[4]) + v[6] * R[10];
            t48 = m[2] * w0;
            t40 = m[4] * w1 + w0 * m[0];
            t44 = m[5] * w1 + m[1] * w0;
            sA = (m[6] * w1 + t48) + m[10] * t3c;
            uA = m[8] * t3c + t40;
            vA = m[9] * t3c + t44;
            t34 = (sA * R[8] + vA * R[4]) + uA * R[0];
            t38 = (vA * R[5] + sA * R[9]) + uA * R[1];
            last = (sA * R[10] + uA * R[2]) + vA * R[6];
            f[4] = (-bd->damp_ang) * t34 + f[4];
            f[5] = (-bd->damp_ang) * t38 + f[5];
            f[6] = (-bd->damp_ang) * last + f[6];
          } else {
            f[4] = (-(m[11] * bd->damp_ang)) * v[4] + f[4];
            f[5] = (-(m[11] * bd->damp_ang)) * v[5] + f[5];
            f[6] = (-(m[11] * bd->damp_ang)) * v[6] + f[6];
          }
        }
      }
      rwp_batch_0057f670(S, g->s14);
      for (si = 0; si < g->n8; si++) {
        ar = (MflArr3 *)(a3 + 0x3c);
        ar->p[ar->n] = (int)(g->arr8[si] + 0xc);
        ar->n++;
        rwp_constraint_linear_row_axis(S, g->arr8[si], 0, dt0);
      }
      for (ii = 0; ii < g->nitems; ii++) {
        it = g->items[ii];
        for (si = 0; si < it->n18; si++) {
          ar = (MflArr3 *)(a3 + 0x3c);
          ar->p[ar->n] = (int)(it->arr18[si] + 0xc);
          ar->n++;
          rwp_constraint_linear_row_axis(S, it->arr18[si], 0, dt0);
        }
      }
      for (c = g->chain; c; c = c->next) {
        if (c->count > 0) {
          a1->p[a1->n] = S->a[20].cap;
          a1->n++;
          ar = (MflArr3 *)(a3 + 0x3c);
          ar->p[ar->n] = c->tag;
          ar->n++;
          rwp_constraints_0057fa00(S, c, dt9);
        }
      }
      rwp_batch_0057f6d0(S);
    }
  }
  S->f134 = 0;
  S->lim144 = 200.0f;
  S->dtw = w->dtw;
  S->f14c = w->mode;
  S->f158 = (w->flags >> 2) & 1;
  rwp_batch_0057ed30(rs->r[6].a, rs->r[6].p, (int)&S->a[22], (int)&S->a[21], rs->r[4].a, rs->r[4].p, rs->r[5].a, rs->r[5].p,
                     rs->r[0].a, rs->r[0].p, rs->r[1].a, rs->r[1].p, rs->r[3].a, rs->r[3].p, w->dtw);
  if (total != 0) {
    rwp_grid_00577c80(a5, a4, S->a[20].cap, (int)S->a[19].p);
    S->head[2] = (int)S->a[19].p;
    if (S->a[7].n != 0) {
      for (i = 0, k = 0; i != (unsigned int)S->a[7].n; i++, k += 4) {
        ((float *)S->a[7].p)[k + 0] += w->soft;
        ((float *)S->a[7].p)[k + 1] += w->soft;
        ((float *)S->a[7].p)[k + 2] += w->soft;
        ((float *)S->a[7].p)[k + 3] += w->soft;
      }
    }
    inv = 1.0f / *(float *)&S->dtw;
    S->f128 = 0;
    tail.w[0] = S->a[22].p ? (int)S->a[22].p : 0;
    tail.w[1] = S->a[22].n;
    tail.w[2] = S->a[22].cap;
    *(float *)&tail.w[3] = inv;
    if (w->mode == 2)
      rwp_batch_0057e3f0(*(MflW4 *)S, SW3(3), SW3(4), SW3(5), SW3(6), SW3(7), SW3(1), SW3(2), tail);
    else
      rwp_batch_0057daa0(*(MflW4 *)S, SW3(3), SW3(4), SW3(5), SW3(6), SW3(7), SW3(1), SW3(2), tail);
    rwp_batch_0057d720(a4, ((int *)S)[0], ((int *)S)[1], ((int *)S)[2], ((int *)S)[3], SW3(17).w[0], SW3(17).w[1], SW3(17).w[2], SW3(18).w[0], SW3(18).w[1], SW3(18).w[2]);
    fn_step(W3(a4, 0x94), *(MflW88 *)a4, SW3(0), SW3(1), SW3(2), SW3(22), SW3(21), S->dtw);
    if (S->a[2].n != 0) {
      for (i = 0, k = 0; i != (unsigned int)S->a[2].n; i++, k += 4) {
        FP(a4, 0x88)[k + 0] = ((float *)S->a[2].p)[k + 0] * ((float *)S->a[10].p)[k + 0];
        FP(a4, 0x88)[k + 1] = ((float *)S->a[2].p)[k + 1] * ((float *)S->a[10].p)[k + 1];
        FP(a4, 0x88)[k + 2] = ((float *)S->a[2].p)[k + 2] * ((float *)S->a[10].p)[k + 2];
        FP(a4, 0x88)[k + 3] = ((float *)S->a[2].p)[k + 3] * ((float *)S->a[10].p)[k + 3];
        S->a[8].p[k + 0] = 0;
        S->a[8].p[k + 1] = 0;
        S->a[8].p[k + 2] = 0;
        S->a[8].p[k + 3] = 0;
        S->a[9].p[k + 0] = 0;
        S->a[9].p[k + 1] = 0;
        S->a[9].p[k + 2] = 0;
        S->a[9].p[k + 3] = 0;
      }
    }
    S->f138 = w->w4;
    S->f140 = w->w5c;
    g_data_9488e4 = 0;
    fn_post(a3, a3 + 0x18, a3 + 0x54, a5, *(MflW88 *)a4, SW3(22), SW3(3), SW3(4), W3(a4, 0x88), SW3(8), S, *(MflW38 *)AT(S, 0x124), w_f8);
    rwp_rows_gather_strided(a6, IP(a3, 0x18), IP(a3, 0x1c), IP(a3, 0x20), a1);
    S->f138 = IP(w, 8);
    S->f140 = IP(w, 0x60);
    g_data_9488e4 = 1;
    fn_post(a3 + 0xc, a3 + 0x24, a3 + 0x60, a5, *(MflW88 *)a4, SW3(22), SW3(5), SW3(6), W3(a4, 0x94), SW3(9), S, *(MflW38 *)AT(S, 0x124), w_f8);
    rwp_joint_accumulators_rebuild(rs->r[9].a, rs->r[9].p, a3 + 0x30, a3 + 0x3c, a3 + 0x48, rs->r[6].a, rs->r[6].p, *(MflW4 *)S, SW3(13), W3(a3, 0xc), W3(a3, 0));
    rwp_joint_matrix_columns_build(rs->r[8].a, rs->r[8].p, W3(a3, 0x30).w[0], W3(a3, 0x30).w[1], W3(a3, 0x30).w[2], rs->r[9].a, rs->r[9].p, SW3(22).w[0], SW3(22).w[1], SW3(22).w[2]);
  }
  rwp_bodies_integrate_list_ordered(rs->r[0].a, rs->r[0].p, rs->r[2].a, rs->r[2].p, W3(a3, 0x30), rs->r[10].a, rs->r[10].p, rs->r[3].a, rs->r[3].p, 0x3f800000);
  rwp_batch_0057c4a0(rs->r[1].a, rs->r[1].p, rs->r[8].a, rs->r[8].p, w->dtw);
  lst = (MflIdList *)rs->r[10].p;
  if (lst->n != 0) {
    for (i = 0; i < ((MflIdList *)rs->r[10].p)->n; i++) {
      idx = ((MflIdList *)rs->r[10].p)->p[i];
      F = (float *)AT(rs->r[6].a, idx * 32);
      R = (float *)AT(*(char **)(a3 + 0x30), i * 32);
      s = R[1] * F[1];
      s = s + F[0] * R[0];
      s = s + R[2] * F[2];
      if (!(s >= -FLT_MIN)) {
        rwp_cylinder_0056fd20(nrm, F);
        V = (float *)AT(rs->r[1].a, idx * 32);
        dv = V[1] * nrm[1];
        dv = dv + V[0] * nrm[0];
        dv = dv + V[2] * nrm[2];
        M = (float *)AT(rs->r[4].a, idx * 48);
        q = s / M[3];
        q = q + q;
        disc = dv * dv;
        disc = q + disc;
        if (disc <= FLT_MIN) {
          r = 0.0f;
        } else {
          r = ((dv < 0.0f) ? -1.0f : 1.0f) * (float)sqrt(disc);
        }
        t = r - dv;
        V[0] = nrm[0] * t + V[0];
        V[1] = nrm[1] * t + V[1];
        V[2] = t * nrm[2] + V[2];
      }
    }
  }
  F = (float *)rs->r[6].a;
  lst = (MflIdList *)rs->r[6].p;
  if (lst->n != 0) {
    for (i = 0; i != lst->n; i++) {
      rec = (int *)AT(F, lst->p[i] * 32);
      rec[2] = 0;
      rec[1] = 0;
      rec[0] = 0;
      rec[6] = 0;
      rec[5] = 0;
      rec[4] = 0;
    }
  }
}
