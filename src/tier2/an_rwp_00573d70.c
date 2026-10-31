#include "mfl_tier2.h"

extern float rwp_cylinder_0056fd20(float *out, float *in);
extern float rwp_math_0056f200(float *out, float *a, float *b, float *c);

/* A triangle: index, plugin data and sector (set by the caller), the three vertex pointers at 0xc, a margin at 0x18 and the unit normal at 0x1c. */
typedef struct MflTriangle {
  int index, plugin_data;
  void *sector;   /* 0x8 */
  float *v[3];   /* 0xc */
  float margin;   /* 0x18 */
  float n[3];   /* 0x1c */
} MflTriangle;

/* Incentre / inradius record: centre, radius and a kind field. */
typedef struct MflIncircle {
  float c[3];
  float radius;   /* 0xc */
  int pad10[2];
  int kind;   /* 0x18 */
} MflIncircle;

/* Sets up a triangle: normal n (cross product of its edges, normalised), the incircle of the triangle into s (kind 3), margin = a tenth
   of the inradius, and the radius of s widened by 20% but at least 5% of the longest side. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_pipeline_00573d70(MflTriangle *t, MflIncircle *s)
{
  float *a = t->v[0], *b = t->v[1], *c = t->v[2];
  double acc, widened;
  float longest, floor_r;

  acc = (a[2] - b[2]) * c[1];
  acc = acc + (b[2] - c[2]) * a[1];
  acc = acc + (c[2] - a[2]) * b[1];
  t->n[0] = acc;
  acc = (c[0] - a[0]) * b[2];
  acc = acc + (a[0] - b[0]) * c[2];
  acc = acc + (b[0] - c[0]) * a[2];
  t->n[1] = acc;
  acc = (b[1] - c[1]) * a[0];
  acc = acc + (c[1] - a[1]) * b[0];
  acc = acc + (a[1] - b[1]) * c[0];
  t->n[2] = acc;
  rwp_cylinder_0056fd20(t->n, t->n);
  longest = rwp_math_0056f200((float *)s, t->v[0], t->v[1], t->v[2]);
  t->margin = s->radius * 0.1f;
  widened = s->radius * 1.2f;
  s->kind = 3;
  s->radius = widened;
  floor_r = longest * 0.05f;
  if (widened < floor_r) *(int *)&s->radius = *(int *)&floor_r;
}
