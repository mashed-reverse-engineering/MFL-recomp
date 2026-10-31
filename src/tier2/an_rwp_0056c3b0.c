#include "mfl_tier2.h"

/*004d3cd0*/ extern RwMatrix * RwMatrixMultiply(RwMatrix *p0, RwMatrix *p1, RwMatrix *p2);

/* Casts the query of the shape o against the segment p0..p1: when o has no fixed frame (shape flags 0x40 bit 1 clear and
   object flag 0x20000 clear) and `other` is given, both points are first moved into the frame of `other` relative to o (matrix
   product o * other, rotated by the transposed matrix); the shape's virtual query (slot 10) is run on them and, on a hit, the
   hit point (hit[1..3]) and normal (hit[4..6]) are moved back by that matrix.  Returns the query result.  The vector temporaries
   are frame floats (an array keeps them rounded and in memory like the original). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_sphere_0056c3b0(void *o, void *other, float *p0, float *p1, float *hit)
{
  float buf[13];
  float v[7];
  float *m;
  float *a, *b;
  double dx, dy, dz, X, Y, acc;
  int r;

  if (!(*(unsigned char *)(*(int *)((char *)o + 0x5c) + 0x40) & 2) && !(*(int *)((char *)o + 0xc) & 0x20000)) {
    if (other)
      m = (float *)RwMatrixMultiply((RwMatrix *)buf, (RwMatrix *)o, (RwMatrix *)other);
    else
      m = (float *)o;
  } else {
    m = (float *)other;
  }
  if (m) {
    dx = p0[0] - m[12];
    dy = p0[1] - m[13];
    dz = p0[2] - m[14];
    X = m[1] * dy;
    X = X + dx * m[0];
    v[0] = X + m[2] * dz;
    X = m[5] * dy;
    X = X + m[4] * dx;
    v[1] = X + m[6] * dz;
    X = m[9] * dy;
    X = X + m[8] * dx;
    v[2] = X + m[10] * dz;
    dx = p1[0] - m[12];
    dy = p1[1] - m[13];
    dz = p1[2] - m[14];
    X = m[1] * dy;
    X = X + dx * m[0];
    v[3] = X + m[2] * dz;
    X = m[5] * dy;
    X = X + m[4] * dx;
    v[4] = X + m[6] * dz;
    X = m[9] * dy;
    X = X + m[8] * dx;
    v[5] = X + m[10] * dz;
    a = &v[0];
    b = &v[3];
  } else {
    a = p0;
    b = p1;
  }
  r = ((int (*)())*(int *)(*(int *)((char *)o + 0x5c) + 0x28))(o, a, b, hit);
  if (r && m) {
    X = hit[1] * m[0];
    X = X + m[4] * hit[2];
    X = X + hit[3] * m[8];
    Y = m[5] * hit[2];
    Y = Y + hit[1] * m[1];
    Y = Y + m[9] * hit[3];
    acc = m[10] * hit[3];
    acc = acc + hit[2] * m[6];
    v[6] = acc + hit[1] * m[2];
    hit[1] = m[12] + X;
    hit[2] = m[13] + Y;
    *(int *)&v[0] = *(int *)&hit[4];
    *(int *)&v[1] = *(int *)&hit[5];
    *(int *)&v[2] = *(int *)&hit[6];
    hit[3] = m[14] + v[6];
    acc = v[1] * m[4];
    acc = acc + v[0] * m[0];
    hit[4] = acc + v[2] * m[8];
    acc = v[1] * m[5];
    acc = acc + v[2] * m[9];
    hit[5] = acc + v[0] * m[1];
    acc = v[2] * m[10];
    acc = acc + v[0] * m[2];
    hit[6] = acc + v[1] * m[6];
  }
  return r;
}
