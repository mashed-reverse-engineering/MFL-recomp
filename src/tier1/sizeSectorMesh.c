/* compile: /G5 */
#include "mfl_tier2.h"
extern int __cdecl rpMeshSize(void *a, void *b);
/*004f56f0*/ extern void *__cdecl RpWorldSectorGetWorld(void *s);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stream size of a world sector's mesh header (needs the sector's world). */
int sizeSectorMesh(void *s)
{
  return rpMeshSize(FLD(s, void *, 0x78), RpWorldSectorGetWorld(s));
}
