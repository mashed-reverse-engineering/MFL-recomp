/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpHullProps { RwV3d centre; float pad; RwV3d w; } RwpHullProps;
/* Velocity of a point on a rigid hull: centre velocity plus w x (b - a); zero without a hull. The difference
   vector is a double vector (x87-resident temps, RWP-X11), the SDK macros give the schedule (RWP-X27). (Rwp library) */
void rwp_convexhullmassprops_00583ca0(RwpHullProps *h, RwV3d *a, RwV3d *b, RwV3d *out)
{
  struct { double x, y, z; } d;

  if (!h) { out->z = 0; out->y = 0; out->x = 0; return; }
  RwV3dSubMacro(&d, b, a);
  RwV3dCrossProductMacro(out, &h->w, &d);
  RwV3dAddMacro(out, &h->centre, out);
}
