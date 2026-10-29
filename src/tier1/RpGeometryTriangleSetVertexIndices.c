/* compile: /G5 */
#include "mfl_tier2.h"
/* Sets the three vertex indices of a triangle; returns the geometry. */
void *RpGeometryTriangleSetVertexIndices(void *g, unsigned short *t, unsigned short a, unsigned short b, unsigned short c)
{
  t[0] = a;
  t[1] = b;
  t[2] = c;
  return g;
}
