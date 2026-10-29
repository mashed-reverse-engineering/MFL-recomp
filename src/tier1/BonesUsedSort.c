/* compile: /G5 */
#include "mfl_tier2.h"
/* qsort comparator on single bytes (bone indices). */
int BonesUsedSort(unsigned char *a, unsigned char *b)
{
  return *a - *b;
}
