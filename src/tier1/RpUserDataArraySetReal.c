/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets float element i of a user data array. */
void RpUserDataArraySetReal(void *a, int i, float v)
{
  FLD(a, float *, 0xc)[i] = v;
}
