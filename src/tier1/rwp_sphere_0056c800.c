/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpSphereObj RwpSphereObj;
typedef struct RwpSphereVt { void *f[6]; void *(*query)(RwpSphereObj *o, int a, RwV3d *dir, int b); } RwpSphereVt;
struct RwpSphereObj { char pad[0x5c]; RwpSphereVt *vt; };
extern void rwp_sphere_0056c6c0(void *r, RwMatrix *m);
/* Rotates dir into the frame of m (dot products with the matrix rows) when a matrix is given, runs the object's
   query slot (vtable entry 6) with it and lets rwp_sphere_0056c6c0 transform the result back. (Rwp library) */
void *rwp_sphere_0056c800(RwpSphereObj *o, RwMatrix *m, int a, RwV3d *dir, int b)
{
  RwV3d v;
  RwV3d *d;
  void *r;

  if (m) {
    v.x = RwV3dDotProductMacro(&m->right, dir);
    v.y = RwV3dDotProductMacro(&m->up, dir);
    v.z = RwV3dDotProductMacro(&m->at, dir);
    d = &v;
  } else d = dir;
  r = o->vt->query(o, a, d, b);
  if (r && m) rwp_sphere_0056c6c0(r, m);
  return r;
}
