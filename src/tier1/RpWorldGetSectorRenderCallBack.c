/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sector render callback of a world. */
void *RpWorldGetSectorRenderCallBack(void *w)
{
  return FLD(w, void *, 0x68);
}
