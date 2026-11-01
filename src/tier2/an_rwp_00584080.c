#include "mfl_tier2.h"
extern int rwp_math_0056f020(float *v);

/* rwp_convexhullmassprops_00584080: Mirtich's polyhedral integrals of a convex hull (face by face, projected onto the
   plane of the dominant normal axis C; A, B the other two axes).
   first = integral of x, y, z (T1), second = of x^2, y^2, z^2 (T2), cross = of xy, yz, zx (TP); returns the volume (T0).
   Edges: 12 bytes, vertex ids at +4 and +6; face f owns the edges first[f] .. first[f+1]-1; vert = float[3] per id. */
typedef struct MflHullEdge {
  int pad0;
  unsigned short a, b;   /* 0x4, 0x6: vertex ids of the edge */
  int pad8;
} MflHullEdge;

typedef struct MflHullShape {
  int pad0;
  float *vert;           /* 0x4 */
  int pad8;
  float *norm;           /* 0xc: face normals */
  unsigned short *first; /* 0x10: first edge of each face */
  MflHullEdge *edge;     /* 0x14 */
  unsigned char pad18[6];
  unsigned short nface;  /* 0x1e */
} MflHullShape;

/* TIER2: unvalidated 2026-11-02 authority=diffemu */
float rwp_convexhullmassprops_00584080(MflHullShape *hull, float *first, float *second, float *cross)
{
  float T0 = 0.0f;
  unsigned int f, i, n;
  int A, B, C;
  float *nrm, *v0;
  MflHullEdge *e;
  float Fc, w, k1, k2, k3, k4, t1, wP1, nA, nB, nC, nBnA, nA2, nB2, qab, qaa, qbb, qu, qv, sum;
  float a0, b0, a1, b1, da, db, a0_2, a0_3, b0_2, b0_3, a1_2, b1_2, b1_3;
  float C1, Ca, Caa, Cb, Cbb, Cab, Kab, Cabb, Kabb;
  float P1, Pa, Paa, Pbb, Paaa, Pb, Pbbb, Pab, Paab, Pabb;
  float Faa, Fbb, Fcc, Faaa, Fbbb, Fccc, Faab, Fbbc, Fcca;

  first[0] = first[1] = first[2] = 0.0f;
  second[0] = second[1] = second[2] = 0.0f;
  cross[0] = cross[1] = cross[2] = 0.0f;
  for (f = 0; f < hull->nface; f++) {
    nrm = hull->norm + 3 * f;
    n = hull->first[f + 1] - hull->first[f];
    e = hull->edge + hull->first[f];
    v0 = hull->vert + 3 * e->a;
    w = v0[1] * nrm[1] + v0[0] * nrm[0];
    w = w + v0[2] * nrm[2];
    w = -w;
    C = rwp_math_0056f020(nrm);
    A = (1 << C) & 3;
    B = (1 << A) & 3;
    P1 = Pa = Paa = Pbb = Paaa = 0.0f;
    Pb = Pbbb = Pab = Paab = Pabb = 0.0f;
    for (i = n; i != 0; i--, e++) {
      a0 = hull->vert[3 * e->a + A];
      b0 = hull->vert[3 * e->a + B];
      a1 = hull->vert[3 * e->b + A];
      b1 = hull->vert[3 * e->b + B];
      da = a1 - a0;
      db = b1 - b0;
      a0_2 = a0 * a0;
      a0_3 = a0_2 * a0;
      b0_2 = b0 * b0;
      b0_3 = b0_2 * b0;
      a1_2 = a1 * a1;
      b1_2 = b1 * b1;
      b1_3 = b1_2 * b1;
      C1 = a1 + a0;
      Ca = a1 * C1 + a0_2;
      Caa = a1 * Ca + a0_3;
      Cb = (b1 + b0) * b1 + b0_2;
      Cbb = b1 * Cb + b0_3;
      Cab = 2.0f * a1 * a0 + (3.0f * a1_2 + a0_2);
      Kab = 2.0f * a1 * a0 + (3.0f * a0_2 + a1_2);
      P1 += db * C1;
      Pa += db * Ca;
      Paa += db * Caa;
      Paaa += db * (a1 * Caa + a0_3 * a0);
      Pb += da * Cb;
      Pbb += da * Cbb;
      Pbbb += da * (b1 * Cbb + b0_3 * b0);
      Pab += db * (b1 * Cab + b0 * Kab);
      Paab += db * (b1 * (a1_2 * a1 * 4.0f + Cab * a0) + b0 * (a1 * Kab + a0_3 * 4.0f));
      Kabb = 4.0f * b0_3 + 3.0f * (b0_2 * b1);
      Kabb = Kabb + 2.0f * (b1_2 * b0);
      Kabb = Kabb + b1_3;
      Cabb = 4.0f * b1_3 + 2.0f * (b0_2 * b1);
      Cabb = Cabb + 3.0f * (b1_2 * b0);
      Cabb = Cabb + b0_3;
      Pabb += da * (a0 * Kabb + a1 * Cabb);
    }
    P1 *= 0.5; Pa *= 1.0 / 6.0; Paa *= 1.0 / 12.0; Paaa *= 0.05;
    Pb *= -1.0 / 6.0; Pbb *= -1.0 / 12.0; Pbbb *= -0.05;
    Pab *= 1.0 / 24.0; Paab *= 1.0 / 60.0; Pabb *= -1.0 / 60.0;
    k1 = 1.0f / nrm[C];
    k2 = k1 * k1;
    k3 = k2 * k1;
    k4 = k3 * k1;
    nA = nrm[A]; nB = nrm[B]; nC = nrm[C];
    nBnA = nB * nA; nA2 = nA * nA; nB2 = nB * nB;
    Faa = k1 * Paa;
    Fbb = k1 * Pbb;
    t1 = nB * Pb + nA * Pa;
    wP1 = w * P1;
    qab = nBnA * Pab; qab = qab + qab;
    qaa = nA2 * Paa;
    qbb = nB2 * Pbb;
    sum = (2.0f * t1 + wP1) * w;
    sum = sum + qab;
    sum = sum + qaa;
    sum = sum + qbb;
    Fcc = sum * k3;
    Faaa = k1 * Paaa;
    Fbbb = k1 * Pbbb;
    sum = qab + qaa;
    sum = sum + qbb;
    sum = ((3.0f * t1 + wP1) * w + 3.0f * sum) * w;
    qu = nB2 * Pabb; qu = qu * nA;
    qv = nA2 * nB; qv = qv * Paab;
    sum = sum + 3.0f * (qu + qv);
    qu = nA * nA2; qu = qu * Paaa;
    sum = sum + qu;
    qv = nB * nB2; qv = qv * Pbbb;
    sum = sum + qv;
    Fccc = -(sum * k4);
    Faab = k1 * Paab;
    sum = Pbbb * nB + Pbb * w;
    sum = sum + Pabb * nA;
    Fbbc = -(sum * k2);
    sum = Pab * nB + Paa * nA;
    sum = (sum + sum + Pa * w) * w;
    sum = sum + 2.0f * (nBnA * Paab);
    sum = sum + nA2 * Paaa;
    sum = sum + nB2 * Pabb;
    Fcca = sum * k3;
    if (A == 0) T0 += nrm[0] * (k1 * Pa);
    else if (B == 0) T0 += nrm[0] * (k1 * Pb);
    else {
      Fc = -((t1 + wP1) * k2);
      T0 += nrm[0] * Fc;
    }
    first[A] += Faa * nA; first[B] += Fbb * nB; first[C] += Fcc * nC;
    second[A] += Faaa * nA; second[B] += Fbbb * nB; second[C] += Fccc * nC;
    cross[A] += Faab * nA; cross[B] += Fbbc * nB; cross[C] += Fcca * nC;
  }
  first[0] *= 0.5f; first[1] *= 0.5f; first[2] *= 0.5f;
  second[0] *= 0.33333334f; second[1] *= 0.33333334f; second[2] *= 0.33333334f;
  cross[0] *= 0.5f; cross[1] *= 0.5f; cross[2] *= 0.5f;
  return T0;
}
