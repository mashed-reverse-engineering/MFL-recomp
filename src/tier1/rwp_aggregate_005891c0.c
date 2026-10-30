/* compile: /G5 */
typedef struct RwpShape { char pad0[8]; void *body; char pad1[4]; void *mat; } RwpShape;
typedef struct RwpPair { RwpShape *a; RwpShape *b; } RwpPair;
extern int rwp_sphere_0056c8a0(void *body, void *mat, float *axis, float *min, float *max);
/* Projects both shapes of the pair on the axis (min/max each) and returns the larger gap; the axis is reversed and the gap taken from the other side when the second interval lies below the first, and the support value is stored in out[0]. The axis is copied to a local first (this selects the frame slots of the intervals). */
float rwp_aggregate_005891c0(RwpPair *pair, float *dir, float *out)
{
  float a, b, c, d, t1, t2;
  float *axis = dir;
  rwp_sphere_0056c8a0(pair->a->body, pair->a->mat, axis, &a, &b);
  rwp_sphere_0056c8a0(pair->b->body, pair->b->mat, axis, &c, &d);
  t1 = a - d;
  t2 = c - b;
  if (t1 > t2) {
    out[0] = d;
    return t1;
  }
  axis[0] = axis[0] * -1.0f;
  axis[1] = axis[1] * -1.0f;
  axis[2] = axis[2] * -1.0f;
  out[0] = -c;
  return t2;
}
