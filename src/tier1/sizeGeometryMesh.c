/* compile: /G5 */
#include "mfl_tier2.h"
extern int __cdecl rpMeshSize(void *a, void *b);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stream size of a geometry's mesh header. */
int sizeGeometryMesh(void *g)
{
  return rpMeshSize(FLD(g, void *, 0x54), g);
}
