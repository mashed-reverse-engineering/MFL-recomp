/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Index of a material in a material list searching from the end, or -1. */
int rpMaterialListFindMaterialIndex(void *l, void *m)
{
  int n = FLD(l, int, 4);
  void **p;
  if (n > 0) { p = FLD(l, void **, 0) + n; do { void *cur = *--p; n--; if (cur == m) return n; } while (n > 0); }
  return n - 1;
}
