/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Zeroes a material list (materials, count, space). */
void *rpMaterialListInitialize(void *l)
{
  FLD(l, int, 8) = 0;
  FLD(l, int, 0) = 0;
  FLD(l, int, 4) = 0;
}
