/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Increments the reference count of a patch mesh. */
void rpPatchMeshAddRef(void *m)
{
  FLD(m, int, 0x44)++;
}
