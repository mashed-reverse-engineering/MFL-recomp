/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpShape { char pad[0x4c]; float radius; char pad2[0x10]; } RwpShape;
typedef struct RwpShapeList { RwpShape *shapes; unsigned int count; } RwpShapeList;
extern RwV3d *rwp_sphere_0056c5d0(RwpShape *s, RwpShape *s2, RwV3d *dir, float *tmp);
/* Support point of a shape list along dir: the shape point with the largest extent (dot with dir plus radius) is written to out;
   returns out, or 0 when the list is empty. (Rwp library) */
RwV3d *rwp_aggregate_005895c0(RwpShapeList *list, RwV3d *dir, RwV3d *out)
{
  float best;
  unsigned int count;
  RwpShapeList *l;
  RwV3d *o;
  RwV3d *d;
  int i;
  unsigned int n;
  RwV3d *p;
  RwpShape *shapes;
  float tmp[3];
  float r;
  float s;

  best = -3.4028235e+38f;
  i = 0;
  count = list->count;
  if (count > 0) {
    n = count;
    l = list;
    o = out;
    d = dir;
    do {
      shapes = l->shapes;
      p = rwp_sphere_0056c5d0(&shapes[i], &shapes[i], d, &tmp[0]);
      if (p) {
        r = l->shapes[i].radius;
        s = RwV3dDotProductMacro(p, d) + r;
        if (s > best) {
          best = s;
          o->x = r * d->x + p->x;
          o->y = r * d->y + p->y;
          o->z = r * d->z + p->z;
        }
      }
      n = i + 1;
      i = n;
    } while (!(n >= l->count));
    if (best > -3.4028235e+38f) {
      return o;
    }
  }
  return 0;
}
