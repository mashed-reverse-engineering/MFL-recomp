/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stream rights chunk size of a world sector: 8 when its plugin rights callback exists, otherwise 0. */
int rpSizeSectRights(void *m)
{
  void *p = FLD(m, void *, 0x7c);
  if (p && FLD(p, int, 0x2c)) return 8;
  return 0;
}
