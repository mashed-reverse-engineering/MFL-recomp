/* compile: /G5 */
#include "mfl_tier2.h"
extern void __cdecl rpMeshDestroy(void *m);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* ORs the lock flags into a geometry and drops its mesh header when the mesh flag is locked; returns the geometry. */
void *RpGeometryLock(void *g, unsigned flags)
{
  FLD(g, unsigned short, 0xc) |= (unsigned short)flags;
  if (flags & 1) { void *mh = FLD(g, void *, 0x54); if (mh) { rpMeshDestroy(mh); FLD(g, int, 0x54) = 0; } }
  return g;
}
