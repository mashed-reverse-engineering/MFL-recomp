/* rwp_triangle_mass_props (0x0058b630), rwp_triangle_interval (0x0058c040): virtual methods of the Rwp triangle shape */
/* compile: /G5 */
#include "mfl_tier2.h"

/*0058b4d0*/ extern float *rwp_triangle_bounding_sphere(float *s, float *out);
/*0056fd20*/ extern float rwp_cylinder_0056fd20(float *out, float *v);   /* normalise v into out, returns the length */

/* The triangle shape: three vertices of 4 floats (x, y, z, pad) at 0x0, 0x10, 0x20; s[19] (0x4c) is its thickness margin. */

/* Mass properties of the triangle, approximated by its bounding sphere: centre of mass = sphere centre (com), inertia diagonal
   0.4 r^2 with the last row (0, 0, 1) in the 7 float record `inertia`.  Returns the volume 2 r^2 margin. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
float rwp_triangle_mass_props(float *s, float *inertia, float *com)
{
  float sph[4];
  float k, v;

  rwp_triangle_bounding_sphere(s, sph);
  ((int *)com)[0] = ((int *)sph)[0];
  ((int *)com)[1] = ((int *)sph)[1];
  ((int *)com)[2] = ((int *)sph)[2];
  k = sph[3] * sph[3] * 0.4f;
  inertia[0] = k;
  inertia[1] = k;
  inertia[2] = k;
  inertia[3] = 0.0f;
  inertia[4] = 0.0f;
  inertia[5] = 0.0f;
  inertia[6] = 1.0f;
  v = sph[3] * s[19] * sph[3];
  return v + v;
}

/* Support vertex of the triangle in direction dir: the vertex with the largest projection on dir (smallest -dot, chosen by the
   comparison table) is copied to out.  Returns out. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
float *rwp_triangle_support_vertex(float *s, float *dir, float *out)
{
  float d0, d1, d2;
  float *v;
  int sh;

  d0 = -(s[0] * dir[0] + s[1] * dir[1] + s[2] * dir[2]);
  d1 = -(s[4] * dir[0] + s[5] * dir[1] + s[6] * dir[2]);
  d2 = -(s[8] * dir[0] + s[9] * dir[1] + s[10] * dir[2]);
  sh = ((((d0 > d1) << 1 | (d0 > d2)) << 1) | (d1 > d2)) << 2;
  v = s + ((0x21312300 >> sh) & 3) * 4;
  out[0] = v[0];
  out[1] = v[1];
  out[2] = v[2];
  return out;
}

/* Edge frame of the triangle, optionally moved by the matrix m (rows at 0x0 / 0x10 / 0x20, translation at 0x30; m = 0 or m = s: no
   move): out[0..2] = the (rotated) normal s[12..14], out[3] = out[4] = s[7]; e[0..2] = v1 - v0, e[3..5] = v2 - v0, e[6..8] = v2 - v1,
   each normalised in place (rwp_cylinder_0056fd20).  Returns TRUE. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwp_triangle_edge_frame(float *s, float *m, float *out, float *e)
{
  float t[15];   /* moved vertices (4 floats each) and the moved normal at t[12] */
  float x, y;
  volatile float z;   /* the original rounds the z sum to a frame float before the translation add */
  float *v = m;

  if (m != s) {
    if (!m) v = s;
    else {
      x = (s[0] * m[0] + s[2] * m[8]) + s[1] * m[4];
      y = (s[0] * m[1] + s[2] * m[9]) + s[1] * m[5];
      z = (s[1] * m[6] + s[0] * m[2]) + s[2] * m[10];
      t[0] = m[12] + x;
      t[1] = m[13] + y;
      t[2] = m[14] + z;
      x = (s[4] * m[0] + s[6] * m[8]) + s[5] * m[4];
      y = (s[4] * m[1] + s[6] * m[9]) + s[5] * m[5];
      z = (s[6] * m[10] + s[5] * m[6]) + s[4] * m[2];
      t[4] = m[12] + x;
      t[5] = m[13] + y;
      t[6] = m[14] + z;
      x = (s[10] * m[8] + s[9] * m[4]) + s[8] * m[0];
      y = (s[10] * m[9] + s[9] * m[5]) + s[8] * m[1];
      z = (s[8] * m[2] + s[10] * m[10]) + s[9] * m[6];
      t[8] = m[12] + x;
      t[9] = m[13] + y;
      t[10] = m[14] + z;
      t[12] = (s[12] * m[0] + s[14] * m[8]) + s[13] * m[4];
      t[13] = (s[12] * m[1] + s[14] * m[9]) + s[13] * m[5];
      t[14] = (s[14] * m[10] + s[13] * m[6]) + s[12] * m[2];
      v = t;
    }
  }
  ((int *)out)[0] = ((int *)v)[12];
  ((int *)out)[1] = ((int *)v)[13];
  ((int *)out)[2] = ((int *)v)[14];
  out[4] = s[7];
  out[3] = s[7];
  e[0] = v[4] - v[0];
  e[1] = v[5] - v[1];
  e[2] = v[6] - v[2];
  e[3] = v[8] - v[0];
  e[4] = v[9] - v[1];
  e[5] = v[10] - v[2];
  e[6] = v[8] - v[4];
  e[7] = v[9] - v[5];
  e[8] = v[10] - v[6];
  rwp_cylinder_0056fd20(e, e);
  rwp_cylinder_0056fd20(e + 3, e + 3);
  rwp_cylinder_0056fd20(e + 6, e + 6);
  return 1;
}

/* Contact feature of the triangle against dir.  nd = normal . dir decides:
   - |nd| <= 0.7071 (seen edge on): with d = -projections on dir, `imax` = the vertex of the smallest d (largest projection), `imid`
     the middle one: the edge imid -> imax (normalised); when edge . dir < 0.5 the feature is that edge (2 vertices, normal =
     dir - (edge . dir) edge, normalised, id g_data_5f72f4[imin]), else the single vertex imax (normal = dir, id g_data_5f72f4[3 + imax]);
   - otherwise the face (3 vertices, normal = +-s[12..14] facing dir, winding by `flip`, id 0).
   out->depth = the largest projection.  Returns out. */
typedef struct MflTriFeature {
  float n[3];   /* 0x0: contact normal */
  float depth;   /* 0xc */
  float v[4][4];   /* 0x10: feature vertices */
  float (*fv)[4];   /* 0x50: = v */
  short count;   /* 0x54: 1 vertex, 2 edge, 3 face */
  short id;   /* 0x56 */
} MflTriFeature;

/*005f72f4*/ extern unsigned short g_data_5f72f4[6];   /* feature ids: edges by imin (4, 8, 2), vertices by imax (10, 6, 12) */

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
MflTriFeature *rwp_triangle_feature(float *s, int flip, float *dir, MflTriFeature *out)
{
  float *vp[3];
  float d[3];
  float edge[3];
  float nd, ed;
  float *a, *b;
  int sh, imax, imid, imin, f;

  vp[0] = s;
  vp[1] = s + 4;
  vp[2] = s + 8;
  nd = (s[13] * dir[1] + s[12] * dir[0]) + s[14] * dir[2];
  d[0] = -((s[1] * dir[1] + s[0] * dir[0]) + s[2] * dir[2]);
  d[1] = -((s[5] * dir[1] + s[4] * dir[0]) + s[6] * dir[2]);
  d[2] = -((s[9] * dir[1] + dir[0] * s[8]) + s[10] * dir[2]);
  sh = ((((d[0] > d[1]) << 1 | (d[0] > d[2])) << 1) | (d[1] > d[2])) << 2;
  imin = (0x321312 >> sh) & 3;
  imid = (0x12300321 >> sh) & 3;
  imax = (0x21312300 >> sh) & 3;
  if (!(nd > 0.7071f) && !(nd < -0.7071f)) {
    a = vp[imax];
    b = vp[imid];
    edge[0] = a[0] - b[0];
    edge[1] = a[1] - b[1];
    edge[2] = a[2] - b[2];
    rwp_cylinder_0056fd20(edge, edge);
    ed = (dir[1] * edge[1] + edge[0] * dir[0]) + dir[2] * edge[2];
    out->fv = out->v;
    ((int *)out->fv[0])[0] = ((int *)a)[0];
    ((int *)out->fv[0])[1] = ((int *)a)[1];
    ((int *)out->fv[0])[2] = ((int *)a)[2];
    if (ed < 0.5f) {
      ((int *)out->fv[1])[0] = ((int *)b)[0];
      ((int *)out->fv[1])[1] = ((int *)b)[1];
      ((int *)out->fv[1])[2] = ((int *)b)[2];
      out->count = 2;
      ed = -ed;
      out->n[0] = edge[0] * ed + dir[0];
      out->n[1] = edge[1] * ed + dir[1];
      out->n[2] = ed * edge[2] + dir[2];
      rwp_cylinder_0056fd20(out->n, out->n);
      out->id = g_data_5f72f4[imin];
    } else {
      out->count = 1;
      ((int *)out->n)[0] = ((int *)dir)[0];
      ((int *)out->n)[1] = ((int *)dir)[1];
      ((int *)out->n)[2] = ((int *)dir)[2];
      out->id = g_data_5f72f4[3 + imax];
    }
  } else {
    ((int *)out->n)[0] = ((int *)s)[12];
    ((int *)out->n)[1] = ((int *)s)[13];
    out->fv = out->v;
    ((int *)out->n)[2] = ((int *)s)[14];
    if (nd < 0.0f) {
      out->n[0] = -out->n[0];
      out->n[1] = -out->n[1];
      out->n[2] = -out->n[2];
      f = flip == 0;
    } else f = flip;
    a = vp[1];
    b = vp[2];
    ((int *)out->v[0])[0] = ((int *)s)[0];
    ((int *)out->v[0])[1] = ((int *)s)[1];
    ((int *)out->v[0])[2] = ((int *)s)[2];
    if (f) {
      ((int *)out->fv[1])[0] = ((int *)a)[0];
      ((int *)out->fv[1])[1] = ((int *)a)[1];
      ((int *)out->fv[1])[2] = ((int *)a)[2];
      ((int *)out->fv[2])[0] = ((int *)b)[0];
      ((int *)out->fv[2])[1] = ((int *)b)[1];
      ((int *)out->fv[2])[2] = ((int *)b)[2];
    } else {
      ((int *)out->fv[1])[0] = ((int *)b)[0];
      ((int *)out->fv[1])[1] = ((int *)b)[1];
      ((int *)out->fv[1])[2] = ((int *)b)[2];
      ((int *)out->fv[2])[0] = ((int *)a)[0];
      ((int *)out->fv[2])[1] = ((int *)a)[1];
      ((int *)out->fv[2])[2] = ((int *)a)[2];
    }
    out->count = 3;
    out->id = 0;
  }
  out->depth = -d[imax];
  return out;
}

/* Bounding box of the triangle, optionally moved by the matrix m (m = 0 or m = s: no move; the vertex transform of
   rwp_triangle_edge_frame): out->sup / out->inf per axis from the vertex order of the three pairwise comparisons, then grown by the
   margin s[19] when it is positive.  The third argument is not used.  Returns out. */
typedef struct MflTriBox {
  float sup[3];
  float pad0;
  float inf[3];   /* 0x10 */
  float pad1;
} MflTriBox;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
MflTriBox *rwp_triangle_bbox(float *s, float *m, void *unused, MflTriBox *out)
{
  float t[12];
  float x, y, r;
  volatile float z;   /* the original rounds the z sum to a frame float before the translation add */
  float *v = m;
  float *vp[3];
  int sh;

  if (!m) v = s;
  else if (m != s) {
    x = (s[0] * m[0] + s[2] * m[8]) + s[1] * m[4];
    y = (s[0] * m[1] + s[2] * m[9]) + s[1] * m[5];
    z = (s[1] * m[6] + s[0] * m[2]) + s[2] * m[10];
    t[0] = m[12] + x;
    t[1] = m[13] + y;
    t[2] = m[14] + z;
    x = (s[4] * m[0] + s[6] * m[8]) + s[5] * m[4];
    y = (s[4] * m[1] + s[6] * m[9]) + s[5] * m[5];
    z = (s[6] * m[10] + s[5] * m[6]) + s[4] * m[2];
    t[4] = m[12] + x;
    t[5] = m[13] + y;
    t[6] = m[14] + z;
    x = (s[10] * m[8] + s[9] * m[4]) + s[8] * m[0];
    y = (s[10] * m[9] + s[9] * m[5]) + s[8] * m[1];
    z = (s[8] * m[2] + s[10] * m[10]) + s[9] * m[6];
    t[8] = m[12] + x;
    t[9] = m[13] + y;
    t[10] = m[14] + z;
    v = t;
  }
  vp[0] = v;
  vp[1] = v + 4;
  vp[2] = v + 8;
  sh = ((((vp[0][0] > vp[1][0]) << 1 | (vp[0][0] > vp[2][0])) << 1) | (vp[1][0] > vp[2][0])) << 2;
  ((int *)out->inf)[0] = ((int *)vp[(0x21312300 >> sh) & 3])[0];
  ((int *)out->sup)[0] = ((int *)vp[(0x321312 >> sh) & 3])[0];
  sh = ((((vp[0][1] > vp[1][1]) << 1 | (vp[0][1] > vp[2][1])) << 1) | (vp[1][1] > vp[2][1])) << 2;
  ((int *)out->inf)[1] = ((int *)vp[(0x21312300 >> sh) & 3])[1];
  ((int *)out->sup)[1] = ((int *)vp[(0x321312 >> sh) & 3])[1];
  sh = ((((vp[0][2] > vp[1][2]) << 1 | (vp[0][2] > vp[2][2])) << 1) | (vp[1][2] > vp[2][2])) << 2;
  ((int *)out->inf)[2] = ((int *)vp[(0x21312300 >> sh) & 3])[2];
  ((int *)out->sup)[2] = ((int *)vp[(0x321312 >> sh) & 3])[2];
  if (s[19] > 0.0f) {
    r = s[19];
    out->sup[0] = r + out->sup[0];
    out->sup[1] = out->sup[1] + r;
    out->sup[2] = out->sup[2] + r;
    out->inf[0] = out->inf[0] - r;
    out->inf[1] = out->inf[1] - r;
    out->inf[2] = out->inf[2] - r;
  }
  return out;
}

/* Projections of the three vertices on dir: the smallest goes to *lo, the largest to *hi (copied bit for bit; the index pair comes from
   the three pairwise comparisons, ties resolved by the table).  Returns TRUE. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwp_triangle_interval(float *s, float *dir, float *lo, float *hi)
{
  float d[3];
  int sh;

  d[0] = s[0] * dir[0] + s[1] * dir[1] + s[2] * dir[2];
  d[1] = s[4] * dir[0] + s[5] * dir[1] + s[6] * dir[2];
  d[2] = s[8] * dir[0] + s[9] * dir[1] + s[10] * dir[2];
  sh = ((((d[0] > d[1]) << 1 | (d[0] > d[2])) << 1) | (d[1] > d[2])) << 2;
  *(int *)lo = ((int *)d)[(0x21312300 >> sh) & 3];
  *(int *)hi = ((int *)d)[(0x321312 >> sh) & 3];
  return 1;
}
