/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Float number i of a user data array. */
float RpUserDataArrayGetReal(void *l, int i)
{
  return FLD(l, float *, 0xc)[i];
}
