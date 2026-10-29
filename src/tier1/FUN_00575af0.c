/* compile: /G5 */
#include "mfl_tier2.h"
/* memset-style fill of n bytes (inline rep stos expansion), nothing for n == 0. (Rwp library, proposed name rwp_capsule_00575af0) */
void FUN_00575af0(void *dst, int c, unsigned n)
{
  if (n > 0) memset(dst, c, n);
}
