// cluster-entries: triangle_normal_cross
/* Installed cluster triangle_normal_cross (tools/cluster_install.py from output/cluster_pilot_kb/triangle_normal_cross.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/* triangle_normal_cross */

/* "out = cross of two edges of triangle (a,b,c)"; out[1].x = -dot(normal, a) is the plane distance */

void triangle_normal_cross(RwV3d *out, float *a, float *b, float *c)

{
  double ax;  /* a - b, kept in extended precision like the original's x87 stack */
  double ay;
  double az;
  volatile float cx; /* c - b: x and y are spilled to floats in the original, z stays extended */
  volatile float cy;
  double cz;
  double dot; /* summed y, x, z like the original */

  ax = a[0] - b[0];
  ay = a[1] - b[1];
  az = a[2] - b[2];
  cx = c[0] - b[0];
  cy = c[1] - b[1];
  cz = c[2] - b[2];
  out->x = cy * az - cz * ay;
  out->y = cz * ax - cx * az;
  out->z = cx * ay - cy * ax;
  RwV3dNormalize(out,out);
  dot = out->y * a[1];
  dot = dot + out->x * a[0];
  dot = dot + out->z * a[2];
  out[1].x = -dot;
  return;
}

