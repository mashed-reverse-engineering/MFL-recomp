/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets integer element i of a user data array. */
void RpUserDataArraySetInt(void *a, int i, int v)
{
  int *arr = FLD(a, int *, 0xc);
  arr[i] = v;
}
