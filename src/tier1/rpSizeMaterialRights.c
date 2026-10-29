/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stream rights chunk size of a material: 8 when its plugin rights callback exists, otherwise 0. */
int rpSizeMaterialRights(void *m)
{
  void *p = FLD(m, void *, 8);
  if (p && FLD(p, int, 0x2c)) return 8;
  return 0;
}
