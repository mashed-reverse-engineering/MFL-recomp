/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Material number i of a patch mesh. */
void *RpPatchMeshGetMaterial(void *l, int i)
{
  return FLD(l, void **, 0x48)[i];
}
