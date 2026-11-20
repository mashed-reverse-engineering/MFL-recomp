/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Increments the reference count of a patch mesh.
   Returns the mesh; 0x55a692 stores it (VOID_w16: was void). */
void *rpPatchMeshAddRef(void *m)
{
  FLD(m, int, 0x44)++;
  return m;
}
