/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* ORs the lock flags into a patch mesh. */
void RpPatchMeshLock(void *m, int f)
{
  FLD(m, int, 0x40) |= f;
}
