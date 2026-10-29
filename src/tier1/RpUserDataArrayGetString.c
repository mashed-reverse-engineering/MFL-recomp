/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* String pointer number i of a user data array. */
void *RpUserDataArrayGetString(void *l, int i)
{
  return FLD(l, void **, 0xc)[i];
}
